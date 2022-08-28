// Copyright 2022 by Greg Corson and Kangaroo Koncepts
// Software offered as is, no warranties
// You may use and redistribute this software as long as my copyright and "free" notice is retained.
// All redistributions and derivative works based on this software must be offered free
//


// Standard includes

#include "BlissTrackerSource.h"

#include "ILiveLinkClient.h"
#include "LiveLinkTypes.h"
#include "Roles/LiveLinkAnimationRole.h"
#include "Roles/LiveLinkAnimationTypes.h"

#include "Async/Async.h"
#include "Common/UdpSocketBuilder.h"
#include "HAL/RunnableThread.h"
#include "Json.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

#include "Roles/LiveLinkCameraTypes.h"
#include "Roles/LiveLinkCameraRole.h"
#include "Roles/LiveLinkLightRole.h"
#include "Roles/LiveLinkLightTypes.h"
#include "Roles/LiveLinkTransformRole.h"
#include "Roles/LiveLinkTransformTypes.h"

// enable logging step 2

DEFINE_LOG_CATEGORY(ModuleLog)

// Define the namespace

#define LOCTEXT_NAMESPACE "BlissTrackerSourceFactory"

// This is the buffer size for receiving UDP packets.

#define RECV_BUFFER_SIZE 1024 * 1024

//----------------------------------------------------------------------------------------------------------------------------
// FBlissTrackerSource constructor
//
FBlissTrackerSource::FBlissTrackerSource(FIPv4Endpoint InEndpoint)
    : Socket(nullptr)
    , Stopping(false)
    , Thread(nullptr)
    , WaitTime(FTimespan::FromMilliseconds(10))
{
    // defaults
    DeviceEndpoint = InEndpoint;

    SourceStatus = LOCTEXT("SourceStatus_DeviceNotFound", "Device Not Found");
    // This name will show up in the LiveLink add source menu
    SourceType = LOCTEXT("BlissTrackerSourceType", "Bliss Tracker");
    SourceMachineName = LOCTEXT("BlissTrackerSourceMachineName", "localhost");

    UE_LOG(ModuleLog, Warning, TEXT("Setup socket"));
    //setup socket
    if (DeviceEndpoint.Address.IsMulticastAddress())
    {
        Socket = FUdpSocketBuilder(TEXT("JSONSOCKET"))
            .AsNonBlocking()
            .AsReusable()
            .BoundToPort(DeviceEndpoint.Port)
            .WithReceiveBufferSize(RECV_BUFFER_SIZE)

            .BoundToAddress(FIPv4Address::Any)
            .JoinedToGroup(DeviceEndpoint.Address)
            .WithMulticastLoopback()
            .WithMulticastTtl(2);

    }
    else
    {
        Socket = FUdpSocketBuilder(TEXT("JSONSOCKET"))
            .AsNonBlocking()
            .AsReusable()
            .BoundToAddress(DeviceEndpoint.Address)
            .BoundToPort(DeviceEndpoint.Port)
            .WithReceiveBufferSize(RECV_BUFFER_SIZE);
    }

    RecvBuffer.SetNumUninitialized(RECV_BUFFER_SIZE);

    if ((Socket != nullptr) && (Socket->GetSocketType() == SOCKTYPE_Datagram))
    {
        SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

        Start();

        SourceStatus = LOCTEXT("SourceStatus_Receiving", "Receiving");
    }
}

//----------------------------------------------------------------------------------------------------------------------------
// FBlissTrackerSource destructor
//
FBlissTrackerSource::~FBlissTrackerSource()
{
    Stop();
    if (Thread != nullptr)
    {
        Thread->WaitForCompletion();
        delete Thread;
        Thread = nullptr;
    }
    if (Socket != nullptr)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
    }
}

//----------------------------------------------------------------------------------------------------------------------------
void FBlissTrackerSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
    Client = InClient;
    SourceGuid = InSourceGuid;
}


//----------------------------------------------------------------------------------------------------------------------------
bool FBlissTrackerSource::IsSourceStillValid() const
{
    // Source is valid if we have a valid thread and socket
    bool bIsSourceValid = !Stopping && Thread != nullptr && Socket != nullptr;
    return bIsSourceValid;
}


//----------------------------------------------------------------------------------------------------------------------------
bool FBlissTrackerSource::RequestSourceShutdown()
{
    Stop();

    return true;
}
// FRunnable interface

//----------------------------------------------------------------------------------------------------------------------------
void FBlissTrackerSource::Start()
{
    ThreadName = "JSON UDP Receiver ";
    ThreadName.AppendInt(FAsyncThreadIndex::GetNext());

    Thread = FRunnableThread::Create(this, *ThreadName, 128 * 1024, TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask());
}

//----------------------------------------------------------------------------------------------------------------------------
void FBlissTrackerSource::Stop()
{
    Stopping = true;
}

//----------------------------------------------------------------------------------------------------------------------------
uint32 FBlissTrackerSource::Run()
{
    TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();

    while (!Stopping)
    {
        if (Socket->Wait(ESocketWaitConditions::WaitForRead, WaitTime))
        {
            uint32 Size = 0;

            while (Socket->HasPendingData(Size))
            {
                int32 Read = 0;

                if (Socket->RecvFrom(RecvBuffer.GetData(), RecvBuffer.Num(), Read, *Sender))
                {
                    if (Read > 0)
                    {
                        TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData = MakeShareable(new TArray<uint8>());
                        ReceivedData->SetNumUninitialized(Read);
                        memcpy(ReceivedData->GetData(), RecvBuffer.GetData(), Read);
                        AsyncTask(ENamedThreads::GameThread, [this, ReceivedData]() { HandleReceivedData(ReceivedData); });
                    }
                }
            }
        }
    }
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------
// FBlissTrackerSource::HandleReceivedData
// This takes the data received by UDP converts it as necessary and sends it into livelink
//
void FBlissTrackerSource::HandleReceivedData(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData)
{

    // This is where you deal with the data you send through UDP.

    // UE_LOG(ModuleLog, Warning, TEXT("HandleReceiveData"));

    // Currently the data contains 15 floats

    float* fdata;
    fdata = (float*)(ReceivedData->GetData());

    // Take data from the packet and turn it into unreal transform structure

    FVector tLocation = FVector((*(fdata + 2)) * 100.0, (*(fdata + 0)) * 100.0, (*(fdata + 1)) * 100.0);
    FRotator tRotator = FRotator(*(fdata + 3), -(*(fdata + 4)), *(fdata + 5));
    FQuat       tQuat = FQuat(tRotator);
    FVector    tScale = FVector(1.0, 1.0, 1.0);
    FTransform tTransform = FTransform(tQuat, tLocation, tScale);

    FName SubjectName("Bliss 1");

    // Push static data into livelink

    FLiveLinkStaticDataStruct TransformStaticDataStruct = FLiveLinkStaticDataStruct(FLiveLinkTransformStaticData::StaticStruct());
    FLiveLinkTransformStaticData& TransformStaticData = *TransformStaticDataStruct.Cast<FLiveLinkTransformStaticData>();
    Client->PushSubjectStaticData_AnyThread({ SourceGuid, SubjectName }, ULiveLinkTransformRole::StaticClass(), MoveTemp(TransformStaticDataStruct));

    // Push periodic frame data
 
    FLiveLinkFrameDataStruct TransformFrameDataStruct = FLiveLinkFrameDataStruct(FLiveLinkTransformFrameData::StaticStruct());
    FLiveLinkTransformFrameData& TransformFrameData = *TransformFrameDataStruct.Cast<FLiveLinkTransformFrameData>();
    TransformFrameData.Transform = tTransform;
 //   TransformFrameData.WorldTime = *(fdata + 13);
    Client->PushSubjectFrameData_AnyThread({ SourceGuid, SubjectName }, MoveTemp(TransformFrameDataStruct));


}

#undef LOCTEXT_NAMESPACE
