// Portions Copyright Epic Games, Inc. All Rights Reserved.
// Portions Copyright Greg Corson, All Rights Reserved
// This LiveLink code is based on the original Epic FreeD plugin, this was necessary because Epic does not supply documentation on how to use LiveLink
// I have added comments to make the code easier to understand.

#include "LiveLinkBlissSource.h"
#include "LiveLinkBliss.h"
#include "ILiveLinkClient.h"
#include "Engine/Engine.h"
#include "Async/Async.h"
#include "LiveLinkBlissSourceSettings.h"
#include "Misc/CoreDelegates.h"
#include "Roles/LiveLinkCameraRole.h"

#include "Misc/Timespan.h"
#include "Common/UdpSocketBuilder.h"

#define LOCTEXT_NAMESPACE "LiveLinkBlissSourceFactory"

// These consts must be defined here in the CPP for non-MS compiler issues

#if 0
const uint8 BlissPacketDefinition::PacketSize = 60;

const uint8 BlissPacketDefinition::X =					0;
const uint8 BlissPacketDefinition::Y =					4;
const uint8 BlissPacketDefinition::Z =					8;
const uint8 BlissPacketDefinition::Roll =				12;
const uint8 BlissPacketDefinition::Pitch =				16;
const uint8 BlissPacketDefinition::Yaw =				20;
const uint8 BlissPacketDefinition::Unfiltered_X =		24;
const uint8 BlissPacketDefinition::Unfiltered_Y =		28;
const uint8 BlissPacketDefinition::Unfiltered_Z =		32;
const uint8 BlissPacketDefinition::Unfiltered_Roll =	36;
const uint8 BlissPacketDefinition::Unfiltered_Pitch =	40;
const uint8 BlissPacketDefinition::Unfiltered_Yaw =		44;
const uint8 BlissPacketDefinition::Confidence =			48;
const uint8 BlissPacketDefinition::Host_Time =			52;
const uint8 BlissPacketDefinition::Sensor_Time =		56;
#else
const uint8 BlissPacketDefinition::PacketSize = 60;

const uint8 BlissPacketDefinition::X = 0;
const uint8 BlissPacketDefinition::Y = 4;
const uint8 BlissPacketDefinition::Z = 8;
const uint8 BlissPacketDefinition::Roll = 12;
const uint8 BlissPacketDefinition::Pitch = 16;
const uint8 BlissPacketDefinition::Yaw = 20;
const uint8 BlissPacketDefinition::Padding1 = 24;
const uint8 BlissPacketDefinition::Padding2 = 28;
const uint8 BlissPacketDefinition::Padding3 = 32;
const uint8 BlissPacketDefinition::Padding4 = 36;
const uint8 BlissPacketDefinition::Message_Type = 40;
const uint8 BlissPacketDefinition::Camera_ID = 44;
const uint8 BlissPacketDefinition::Confidence = 48;
const uint8 BlissPacketDefinition::Host_Time = 52;
const uint8 BlissPacketDefinition::Sensor_Time = 56;
#endif

//-------------------------------------------------------------------------------------------------------------------------------
// FLiveLinkBlissSource constructor
// This opens up the UDP socket we will be receiving data on
//

FLiveLinkBlissSource::FLiveLinkBlissSource(const FLiveLinkBlissConnectionSettings& ConnectionSettings)
: Client(nullptr)
, Stopping(false)
, Thread(nullptr)
{
	UE_LOG(LogLiveLinkBliss, Log, TEXT("LiveLinkBlissSource: This is the NEW LiveLinkBliss"));
	SourceStatus = LOCTEXT("SourceStatus_NoData", "No data");
	SourceType = LOCTEXT("SourceType_Bliss", "Bliss");
	SourceMachineName = FText::Format(LOCTEXT("BlissSourceMachineName", "{0}:{1}"), FText::FromString(ConnectionSettings.IPAddress), FText::AsNumber(ConnectionSettings.UDPPortNumber, &FNumberFormattingOptions::DefaultNoGrouping()));

	FIPv4Address::Parse(ConnectionSettings.IPAddress, DeviceEndpoint.Address);
	DeviceEndpoint.Port = ConnectionSettings.UDPPortNumber;

	// Create the socket we will receive on

	Socket = FUdpSocketBuilder(TEXT("BlissListenerSocket"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(DeviceEndpoint)
		.WithReceiveBufferSize(ReceiveBufferSize);

	// If the socket opened up correctly, initialize the receive buffer and startup delegate
	// Otherwise, error.

	if ((Socket != nullptr) && (Socket->GetSocketType() == SOCKTYPE_Datagram))
	{
		ReceiveBuffer.SetNumUninitialized(ReceiveBufferSize);
		SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		DeferredStartDelegateHandle = FCoreDelegates::OnEndFrame.AddRaw(this, &FLiveLinkBlissSource::Start);

		UE_LOG(LogLiveLinkBliss, Log, TEXT("LiveLinkBlissSource: Opened UDP socket with IP address %s"), *DeviceEndpoint.ToString());
	}
	else
	{
		UE_LOG(LogLiveLinkBliss, Error, TEXT("LiveLinkBlissSource: Failed to open UDP socket with IP address %s"), *DeviceEndpoint.ToString());
	}
}
//-------------------------------------------------------------------------------------------------------------------------------
// FLiveLinkBlissSource destructor
// This is the final shutdown code
//

FLiveLinkBlissSource::~FLiveLinkBlissSource()
{
	// This could happen if the object is destroyed before FCoreDelegates::OnEndFrame calls FLiveLinkBlissSource::Start

	if (DeferredStartDelegateHandle.IsValid())
	{
		FCoreDelegates::OnEndFrame.Remove(DeferredStartDelegateHandle);
	}

	Stop();

	// Wait for the thread to shut down and then delete it.

	if (Thread != nullptr)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}

	// Close the UDP socket

	if (Socket != nullptr)
	{
		Socket->Close();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
void FLiveLinkBlissSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
	Client = InClient;
	SourceGuid = InSourceGuid;
}

//-------------------------------------------------------------------------------------------------------------------------------
void FLiveLinkBlissSource::InitializeSettings(ULiveLinkSourceSettings* Settings)
{
	// Save our source settings pointer so we can use it directly
	SavedSourceSettings = Cast<ULiveLinkBlissSourceSettings>(Settings);
}

//-------------------------------------------------------------------------------------------------------------------------------
bool FLiveLinkBlissSource::IsSourceStillValid() const
{
	// Source is valid if we have a valid thread
	bool bIsSourceValid = !Stopping && (Thread != nullptr) && (Socket != nullptr);
	return bIsSourceValid;
}

//-------------------------------------------------------------------------------------------------------------------------------
bool FLiveLinkBlissSource::RequestSourceShutdown()
{
	Stop();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------
// OnSettingsChanged --- Handles updating the default settings if someone changes the source type in the LiveLink window.
// Right now the only source is "Bliss" though in the future we may add more.
//
void FLiveLinkBlissSource::OnSettingsChanged(ULiveLinkSourceSettings* Settings, const FPropertyChangedEvent& PropertyChangedEvent)
{
	ILiveLinkSource::OnSettingsChanged(Settings, PropertyChangedEvent);

	FProperty* MemberProperty = PropertyChangedEvent.MemberProperty;
	FProperty* Property = PropertyChangedEvent.Property;
	if (Property && MemberProperty && (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive))
	{
		ULiveLinkBlissSourceSettings* SourceSettings = Cast<ULiveLinkBlissSourceSettings>(Settings);
		if (SavedSourceSettings != SourceSettings)
		{
			UE_LOG(LogLiveLinkBliss, Error, TEXT("LiveLinkBlissSource: OnSettingsChanged pointers don't match - this should never happen!"));
			return;
		}

		if (SourceSettings != nullptr)
		{
			static FName NAME_DefaultConfig = GET_MEMBER_NAME_CHECKED(ULiveLinkBlissSourceSettings, DefaultConfig);
			static FName NAME_FocusDistanceEncoderData = GET_MEMBER_NAME_CHECKED(ULiveLinkBlissSourceSettings, FocusDistanceEncoderData);
			static FName NAME_FocalLengthEncoderData = GET_MEMBER_NAME_CHECKED(ULiveLinkBlissSourceSettings, FocalLengthEncoderData);
			static FName NAME_UserDefinedEncoderData = GET_MEMBER_NAME_CHECKED(ULiveLinkBlissSourceSettings, UserDefinedEncoderData);
			const FName PropertyName = Property->GetFName();
			const FName MemberPropertyName = MemberProperty->GetFName();

			bool bFocusDistanceEncoderDataChanged = false;
			bool bFocalLengthEncoderDataChanged = false;
			bool bUserDefinedEncoderDataChanged = false;

			// Setup the default values for the type of sensor
			// Arguments for FBlissEncoderData are isValid, invert encoder, use manual range, min encoder value, max encoder value, mask bits for encoder value
			// Currently Bliss does not have encoder values so all these are turned off by default.

			if (PropertyName == NAME_DefaultConfig)
			{
				switch (SourceSettings->DefaultConfig)
				{
					case EBlissDefaultConfigs::Bliss:		
						SourceSettings->FocusDistanceEncoderData = FBlissEncoderData({ false, false, false, 0, 0xffff, 0x0000ffff });
						SourceSettings->FocalLengthEncoderData = FBlissEncoderData({ false, false, false, 0, 0xffff, 0x0000ffff });
						SourceSettings->UserDefinedEncoderData = FBlissEncoderData({ false });
						UE_LOG(LogLiveLinkBliss, Log, TEXT("Bliss time stamps ON (recommended)"));
						SavedSourceSettings->bUseTimestamps = true;
						SavedSourceSettings->bPrintTimeStats = false;
						break;
					case EBlissDefaultConfigs::BlissNoTimeStamps:		
						SourceSettings->FocusDistanceEncoderData = FBlissEncoderData({ false, false, false, 0, 0xffff, 0x0000ffff });
						SourceSettings->FocalLengthEncoderData = FBlissEncoderData({ false, false, false, 0, 0xffff, 0x0000ffff });
						SourceSettings->UserDefinedEncoderData = FBlissEncoderData({ false });
						UE_LOG(LogLiveLinkBliss, Log, TEXT("Bliss time stamps OFF (old, for test only)"));
						SavedSourceSettings->bUseTimestamps = false;
						SavedSourceSettings->bPrintTimeStats = false;
						break;
					case EBlissDefaultConfigs::BlissPrintStatistics:		
						SourceSettings->FocusDistanceEncoderData = FBlissEncoderData({ false, false, false, 0, 0xffff, 0x0000ffff });
						SourceSettings->FocalLengthEncoderData = FBlissEncoderData({ false, false, false, 0, 0xffff, 0x0000ffff });
						SourceSettings->UserDefinedEncoderData = FBlissEncoderData({ false });
						UE_LOG(LogLiveLinkBliss, Log, TEXT("Bliss Bliss time stamps ON and printing time stats"));
						SavedSourceSettings->bUseTimestamps = true;
						SavedSourceSettings->bPrintTimeStats = true;
						break;
				}

				bFocusDistanceEncoderDataChanged = true;
				bFocalLengthEncoderDataChanged = true;
				bUserDefinedEncoderDataChanged = true;
			}

			if (MemberPropertyName == NAME_FocusDistanceEncoderData)
			{
				bFocusDistanceEncoderDataChanged = true;
			}
			else if (MemberPropertyName == NAME_FocalLengthEncoderData)
			{
				bFocalLengthEncoderDataChanged = true;
			}
			else if (MemberPropertyName == NAME_UserDefinedEncoderData)
			{
				bUserDefinedEncoderDataChanged = true;
			}

			if (bFocusDistanceEncoderDataChanged)
			{
				UpdateEncoderData(&SourceSettings->FocusDistanceEncoderData);
			}

			if (bFocalLengthEncoderDataChanged)
			{
				UpdateEncoderData(&SourceSettings->FocalLengthEncoderData);
			}

			if (bUserDefinedEncoderDataChanged)
			{
				UpdateEncoderData(&SourceSettings->UserDefinedEncoderData);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
void FLiveLinkBlissSource::UpdateEncoderData(FBlissEncoderData* InEncoderData)
{
	if (InEncoderData->Min == InEncoderData->Max)
	{
		UE_LOG(LogLiveLinkBliss, Error, TEXT("LiveLinkBlissSource: EncoderData Min/Max values can't be equal (you may need to rack your encoder) - incoming data may be invalid!"));
	}

	// Update any changed encoder data to reset min/max values for auto-ranging
	if (!InEncoderData->bUseManualRange)
	{
		InEncoderData->Min = InEncoderData->MaskBits;
		InEncoderData->Max = 0;
	}

	// Remove the static data from the EncounteredSubjects list so it will be automatically updated during the next Send()
	EncounteredSubjects.Remove(FName(CameraSubjectName));
}

//-------------------------------------------------------------------------------------------------------------------------------
// FRunnable interface
void FLiveLinkBlissSource::Start()
{
	check(DeferredStartDelegateHandle.IsValid());

	FCoreDelegates::OnEndFrame.Remove(DeferredStartDelegateHandle);
	DeferredStartDelegateHandle.Reset();

	SourceStatus = LOCTEXT("SourceStatus_Receiving", "Receiving");

	ThreadName = "LiveLinkBliss Receiver ";
	ThreadName.AppendInt(FAsyncThreadIndex::GetNext());

	Thread = FRunnableThread::Create(this, *ThreadName, 128 * 1024, TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask());
}

//-------------------------------------------------------------------------------------------------------------------------------
void FLiveLinkBlissSource::Stop()
{
	Stopping = true;
}

//-------------------------------------------------------------------------------------------------------------------------------
// FLiveLinkBlissSource::Run() --- This is the main worker that receives the packets from bliss, decodes them and
// sends them on to livelink
//
uint32 FLiveLinkBlissSource::Run()
{
	static double lastSensorTime = 0.0;
	static double lastHostTime = 0.0;
	static double lastUnrealTime = 0.0;
	static double lastRawSensorTime = 0.0;
	static bool   lastFocalDistance = false;
	float debugger[60];

	// Free-D max data rate is 100Hz
	const FTimespan SocketTimeout(FTimespan::FromMilliseconds(10));

	// Only process packets if we are not stopping (shutting down)
	while (!Stopping)
	{
		// If the socket is valid, wait for data to happen
		if (Socket && Socket->Wait(ESocketWaitConditions::WaitForRead, SocketTimeout))
		{
			// While there is data to process, repeat this
			uint32 PendingDataSize = 0;
			while (Socket && Socket->HasPendingData(PendingDataSize))
			{
				// Do a single read, should return one UDP packet
				int32 ReceivedDataSize = 0;
				if (Socket && Socket->Recv(ReceiveBuffer.GetData(), ReceiveBufferSize, ReceivedDataSize))
				{
					// If we got some data, process it.
					if (ReceivedDataSize > 0)
					{
						memset(debugger, 0, 240);
						memcpy(debugger, &ReceiveBuffer[0], ReceivedDataSize);
						// !!!GAC not sure what the source settings do yet!
						if (SavedSourceSettings == nullptr)
						{
							UE_LOG(LogLiveLinkBliss, Error, TEXT("LiveLinkBlissSource: Received a packet, but we don't have a valid SavedSourceSettings!"));
						}
						else // This could be a if-else chain to handle different kinds of packets, right now there is just one
						{
							// Warn if we didn't get the right size packet

							if (ReceivedDataSize != BlissPacketDefinition::PacketSize)
							{
//								UE_LOG(LogLiveLinkBliss, Warning, TEXT("LiveLinkBlissSource: Received packet length mismatch - received 0x%02x, calculated 0x%02x"), ReceivedDataSize, BlissPacketDefinition::PacketSize);
								UE_LOG(LogLiveLinkBliss, Warning, TEXT("LiveLinkBlissSource: Received packet length mismatch - received %d, calculated %d"), ReceivedDataSize, BlissPacketDefinition::PacketSize);
							}

							// There is currently only one type of message to receive, this parses it out.
							// There is no byte order conversion being done right now.

							// Decode xyz, roll pitch yaw and generate a transform from it
							// Bliss sends meters, so we multiply by 100 to get CM for unreal, also do some axis swapping
							FVector tLocation = FVector(
								(*(float*)&ReceiveBuffer[BlissPacketDefinition::Z]) * 100.0,
								(*(float*)&ReceiveBuffer[BlissPacketDefinition::X]) * 100.0,
								(*(float*)&ReceiveBuffer[BlissPacketDefinition::Y]) * 100.0);

							FRotator tRotator = FRotator(
								*(float*)&ReceiveBuffer[BlissPacketDefinition::Roll],
								-(*(float*)&ReceiveBuffer[BlissPacketDefinition::Pitch]),
								*(float*)&ReceiveBuffer[BlissPacketDefinition::Yaw]);
							FQuat tQuat = FQuat(tRotator);
							FVector    tScale = FVector(1.0, 1.0, 1.0);
							FTransform tTransform = FTransform(tQuat, tLocation, tScale);

							// Convert the sensor time to seconds as a double

							double hostTime = (*(float*)&ReceiveBuffer[BlissPacketDefinition::Host_Time]);
							double rawSensorTime = (*(float*)&ReceiveBuffer[BlissPacketDefinition::Sensor_Time]);
							double unrealTime = FPlatformTime::Seconds();

							double sensorTime = rawSensorTime / 1000000.0;
							double sensorDelta = sensorTime - lastSensorTime;
							double hostDelta = hostTime - lastHostTime;
							double unrealDelta = unrealTime - lastUnrealTime;
							double arrivalJitter = sensorDelta - unrealDelta;

							if(lastRawSensorTime == rawSensorTime || lastSensorTime == sensorTime)
								UE_LOG(LogLiveLinkBliss, Warning, TEXT("LiveLinkBlissSource: Two times were the same! Sensor Time %f, lastSensorTime %f sensordelta %f, rawSensorDelta %f"), sensorTime, lastSensorTime, sensorDelta, rawSensorTime - lastRawSensorTime);
							
							lastSensorTime = sensorTime;
							lastHostTime = hostTime;
							lastUnrealTime = unrealTime;
							lastRawSensorTime = rawSensorTime;

#if 0						// This is how FreeD handled lens encoder data, bliss doesn't have it yet
							int32 FocalLengthInt = Decode_Unsigned_24(&ReceiveBuffer[BlissPacketDefinition::FocalLength]);
							int32 FocusDistanceInt = Decode_Unsigned_24(&ReceiveBuffer[BlissPacketDefinition::FocusDistance]);
							int32 UserDefinedDataInt = Decode_Unsigned_16(&ReceiveBuffer[BlissPacketDefinition::UserDefined]);

							float FocalLength = ProcessEncoderData(SavedSourceSettings->FocalLengthEncoderData, FocalLengthInt);
							float FocusDistance = ProcessEncoderData(SavedSourceSettings->FocusDistanceEncoderData, FocusDistanceInt);
							float UserDefinedData = ProcessEncoderData(SavedSourceSettings->UserDefinedEncoderData, UserDefinedDataInt);
#endif

							// Handle release 2 format and release 3 test format

							float messageType = *(float*)&ReceiveBuffer[BlissPacketDefinition::Message_Type];
							float CameraId    = *(float*)&ReceiveBuffer[BlissPacketDefinition::Camera_ID];

							if (messageType != 1.0f)
							{
								CameraId = 1;
							}
					
//							int CameraId = 1;
//							int CameraId = *(float*)&ReceiveBuffer[BlissPacketDefinition::Camera_ID];
							float FocalLength = 0;
							float FocusDistance = 0;
							float UserDefinedData = 0;

							// Make data structure to push into livelink

							FLiveLinkFrameDataStruct FrameData(FLiveLinkCameraFrameData::StaticStruct());
							FLiveLinkCameraFrameData* CameraFrameData = FrameData.Cast<FLiveLinkCameraFrameData>();
							CameraFrameData->Transform = tTransform;
							CameraSubjectName = FString::Printf(TEXT("Camera %.0f"), CameraId);

							// If timestamps are turned on, put them in the LiveLink data
							if (SavedSourceSettings->bUseTimestamps)
							{
//								UE_LOG(LogLiveLinkBliss, Log, TEXT("LiveLinkBlissSource: Timestamps ON"));
								CameraFrameData->WorldTime = sensorTime;
							}
							else 
							{ 
//								UE_LOG(LogLiveLinkBliss, Log, TEXT("LiveLinkBlissSource: Timestamps OFF"));
							}

							// If bPrintTimeStats are on, print them to the log
							if (SavedSourceSettings->bPrintTimeStats)
							{
								UE_LOG(LogLiveLinkBliss, Log, TEXT("LiveLinkBlissSource: Sensor Time %f sensordelta %f hostdelta %f unrealdelta %f delivery jitter %f"), sensorTime, sensorDelta, hostDelta, unrealDelta, arrivalJitter);
							}

#if 0						// This is how FreeD handled lens encoder data, bliss doesn't have it yet
							if (SavedSourceSettings->bSendExtraMetaData)
							{
								CameraFrameData->MetaData.StringMetaData.Add(FName(TEXT("CameraId")), FString::Printf(TEXT("%d"), CameraId));
								CameraFrameData->MetaData.StringMetaData.Add(FName(TEXT("FrameCounter")), FString::Printf(TEXT("%d"), FrameCounter));
							}

							if (SavedSourceSettings->FocalLengthEncoderData.bIsValid)
							{
								CameraFrameData->FocalLength = FocalLength;
							}
							if (SavedSourceSettings->FocusDistanceEncoderData.bIsValid)
							{
								CameraFrameData->FocusDistance = FocusDistance;
							}
							if (SavedSourceSettings->UserDefinedEncoderData.bIsValid)
							{
								CameraFrameData->Aperture = UserDefinedData;
							}
#endif
							// Send the bliss data into LiveLink

							Send(&FrameData, FName(CameraSubjectName));

							FrameCounter++;
						}
#if 0
						// If there were multiple packet types we would add an if-else here to handle them
						else
						{
							// If there were multiple packet types, we would complain about unknown types here.
							UE_LOG(LogLiveLinkBliss, Warning, TEXT("LiveLinkBlissSource: Unsupported Bliss message type 0x%02x"), ReceiveBuffer[BlissPacketDefinition::PacketType]);
						}
#endif
					}
				}
			}
		}
	}
	
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------
void FLiveLinkBlissSource::Send(FLiveLinkFrameDataStruct* FrameDataToSend, FName SubjectName)
{
	if (Stopping || (Client == nullptr))
	{
		return;
	}

	if (!EncounteredSubjects.Contains(SubjectName))
	{
		FLiveLinkStaticDataStruct StaticData(FLiveLinkCameraStaticData::StaticStruct());

		FLiveLinkCameraStaticData& CameraData = *StaticData.Cast<FLiveLinkCameraStaticData>();
		CameraData.bIsFocusDistanceSupported = SavedSourceSettings->FocusDistanceEncoderData.bIsValid;
		CameraData.bIsFocalLengthSupported = SavedSourceSettings->FocalLengthEncoderData.bIsValid;
		CameraData.bIsApertureSupported = SavedSourceSettings->UserDefinedEncoderData.bIsValid;
		Client->PushSubjectStaticData_AnyThread({ SourceGuid, SubjectName }, ULiveLinkCameraRole::StaticClass(), MoveTemp(StaticData));
		EncounteredSubjects.Add(SubjectName);
	}

	Client->PushSubjectFrameData_AnyThread({ SourceGuid, SubjectName }, MoveTemp(*FrameDataToSend));
}

//-------------------------------------------------------------------------------------------------------------------------------
float FLiveLinkBlissSource::ProcessEncoderData(FBlissEncoderData& EncoderData, int32 RawEncoderValueInt)
{
	float FinalEncoderValue = 0.0f;

	if (EncoderData.bIsValid)
	{
		RawEncoderValueInt &= EncoderData.MaskBits;

		// Auto-range the input
		if (!EncoderData.bUseManualRange)
		{
			if (RawEncoderValueInt < EncoderData.Min)
			{
				EncoderData.Min = RawEncoderValueInt;
			}
			if (RawEncoderValueInt > EncoderData.Max)
			{
				EncoderData.Max = RawEncoderValueInt;
			}
		}

		int32 Delta = EncoderData.Max - EncoderData.Min;
		if (Delta != 0)
		{
			FinalEncoderValue = FMath::Clamp((float)(RawEncoderValueInt - EncoderData.Min) / (float)Delta, 0.0f, 1.0f);
			if (EncoderData.bInvertEncoder)
			{
				FinalEncoderValue = 1.0f - FinalEncoderValue;
			}
		}
	}

	return FinalEncoderValue;
}

//-------------------------------------------------------------------------------------------------------------------------------
float FLiveLinkBlissSource::Decode_Signed_8_15(uint8* InBytes)
{
	int32 ret = (*InBytes << 16) | (*(InBytes + 1) << 8) | *(InBytes + 2);
	if (*InBytes & 0x80)
	{
		ret -= 0x00ffffff;
	}
	return (float)ret / 32768.0f;
}

//-------------------------------------------------------------------------------------------------------------------------------
float FLiveLinkBlissSource::Decode_Signed_17_6(uint8* InBytes)
{
	int32 ret = (*InBytes << 16) | (*(InBytes + 1) << 8) | *(InBytes + 2);
	if (*InBytes & 0x80)
	{
		ret -= 0x00ffffff;
	}
	return (float)ret / 640.0f;
}

//-------------------------------------------------------------------------------------------------------------------------------
uint32 FLiveLinkBlissSource::Decode_Unsigned_24(uint8* InBytes)
{
	uint32 ret = (*InBytes << 16) | (*(InBytes + 1) << 8) | *(InBytes + 2);
	return ret;
}

//-------------------------------------------------------------------------------------------------------------------------------
uint16 FLiveLinkBlissSource::Decode_Unsigned_16(uint8* InBytes)
{
	uint16 ret = (*InBytes << 8) | *(InBytes + 1);
	return ret;
}

//-------------------------------------------------------------------------------------------------------------------------------
uint8 FLiveLinkBlissSource::CalculateChecksum(uint8* InBytes, uint32 Size)
{
	uint8 sum = 0x40;
	for (uint32 i = 0; i < Size; i++)
	{
		sum -= *(InBytes + i);
	}
	return sum;
}

#undef LOCTEXT_NAMESPACE
