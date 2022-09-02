// Copyright Epic Games, Inc. All Rights Reserved.

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
const uint8 BlissPacketDefinition::PacketTypeD1 = 0xD1;
const uint8 BlissPacketDefinition::PacketSizeD1 = 0x1D;
const uint8 BlissPacketDefinition::PacketType = 0x00;
const uint8 BlissPacketDefinition::CameraID = 0x01;
const uint8 BlissPacketDefinition::Yaw = 0x02;
const uint8 BlissPacketDefinition::Pitch = 0x05;
const uint8 BlissPacketDefinition::Roll = 0x08;
const uint8 BlissPacketDefinition::X = 0x0B;
const uint8 BlissPacketDefinition::Y = 0x0E;
const uint8 BlissPacketDefinition::Z = 0x11;
const uint8 BlissPacketDefinition::FocalLength = 0x14;
const uint8 BlissPacketDefinition::FocusDistance = 0x17;
const uint8 BlissPacketDefinition::UserDefined = 0x1A;
const uint8 BlissPacketDefinition::Checksum = 0x1C;

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

//
// Specific manufacturer default data (we still use auto-ranging as the default)
//
// Name			UDP port	Zoom (wide)	(tele)		Focus (near)	(far)		Spare
// Generic		40000		0x0			0x10000		0x0				0x10000		Unused
// Panasonic	1111		0x555		0xfff		0x555			0xfff		Iris 0x555 (close) - 0xfff (open)
// Sony			52380		0x0			0x7ac0		0x1000			0xf000		Lower 12 bits - Iris (F value * 100); Upper 4 bits - Frame number
// Mosys		8001		0x0			0xffff		0x0				0xffff		Lower 8 bits - Tracking quality 0-3 (undef, good, caution, bad)
// Stype		6301		0x0			0xffffff	0x0				0xffffff	Unused
// Ncam			6301		0x0			0xffffff	0x0				0xffffff	Unused
//

//-------------------------------------------------------------------------------------------------------------------------------
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

			if (PropertyName == NAME_DefaultConfig)
			{
				switch (SourceSettings->DefaultConfig)
				{
					case EBlissDefaultConfigs::Generic:		SourceSettings->FocusDistanceEncoderData = FBlissEncoderData({ true, false, false, 0, 0xffff, 0x0000ffff });
															SourceSettings->FocalLengthEncoderData = FBlissEncoderData({ true, false, false, 0, 0xffff, 0x0000ffff });
															SourceSettings->UserDefinedEncoderData = FBlissEncoderData({ false });
															break;

					case EBlissDefaultConfigs::Panasonic:	SourceSettings->FocusDistanceEncoderData = FBlissEncoderData({ true, false, false, 0x0555, 0x0fff, 0x0000ffff });
															SourceSettings->FocalLengthEncoderData = FBlissEncoderData({ true, false, false, 0x0555, 0x0fff, 0x0000ffff });
															SourceSettings->UserDefinedEncoderData = FBlissEncoderData({ true, true, false, 0x0555, 0x0fff, 0x0000ffff });
															break;

					case EBlissDefaultConfigs::Sony:		SourceSettings->FocusDistanceEncoderData = FBlissEncoderData({ true, false, false, 0x1000, 0xf000, 0x0000ffff });
															SourceSettings->FocalLengthEncoderData = FBlissEncoderData({ true, false, false, 0, 0x7ac0, 0x0000ffff });
															SourceSettings->UserDefinedEncoderData = FBlissEncoderData({ true, false, false, 0, 0x0fff, 0x00000fff });
															break;

					case EBlissDefaultConfigs::Mosys:		SourceSettings->FocusDistanceEncoderData = FBlissEncoderData({ true, false, false, 0, 0xffff, 0x0000ffff });
															SourceSettings->FocalLengthEncoderData = FBlissEncoderData({ true, false, false, 0, 0xffff, 0x0000ffff });
															SourceSettings->UserDefinedEncoderData = FBlissEncoderData({ false });
															break;

					case EBlissDefaultConfigs::Stype:		SourceSettings->FocusDistanceEncoderData = FBlissEncoderData({ true, false, false, 0, 0xffffff, 0x00ffffff });
															SourceSettings->FocalLengthEncoderData = FBlissEncoderData({ true, false, false, 0, 0xffffff, 0x00ffffff });
															SourceSettings->UserDefinedEncoderData = FBlissEncoderData({ false });
															break;

					case EBlissDefaultConfigs::Ncam:		SourceSettings->FocusDistanceEncoderData = FBlissEncoderData({ true, false, false, 0, 0xffffff, 0x00ffffff });
															SourceSettings->FocalLengthEncoderData = FBlissEncoderData({ true, false, false, 0, 0xffffff, 0x00ffffff });
															SourceSettings->UserDefinedEncoderData = FBlissEncoderData({ false });
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
	// Free-D max data rate is 100Hz
	const FTimespan SocketTimeout(FTimespan::FromMilliseconds(10));

	while (!Stopping)
	{
		if (Socket && Socket->Wait(ESocketWaitConditions::WaitForRead, SocketTimeout))
		{
			uint32 PendingDataSize = 0;
			while (Socket && Socket->HasPendingData(PendingDataSize))
			{
				int32 ReceivedDataSize = 0;
				if (Socket && Socket->Recv(ReceiveBuffer.GetData(), ReceiveBufferSize, ReceivedDataSize))
				{
					if (ReceivedDataSize > 0)
					{
						if (SavedSourceSettings == nullptr)
						{
							UE_LOG(LogLiveLinkBliss, Error, TEXT("LiveLinkBlissSource: Received a packet, but we don't have a valid SavedSourceSettings!"));
						}
						else if (ReceiveBuffer[BlissPacketDefinition::PacketType] == BlissPacketDefinition::PacketTypeD1)
						{
							// The only message that we care about is the 0xD1 message which contains PnO data, zoom, focus, and a user defined field (usually iris)
							uint8 CameraId = ReceiveBuffer[BlissPacketDefinition::CameraID];
							FRotator Orientation;
							Orientation.Yaw = Decode_Signed_8_15(&ReceiveBuffer[BlissPacketDefinition::Yaw]);
							Orientation.Pitch = Decode_Signed_8_15(&ReceiveBuffer[BlissPacketDefinition::Pitch]);
							Orientation.Roll = Decode_Signed_8_15(&ReceiveBuffer[BlissPacketDefinition::Roll]);

							// Bliss has the X and Y axes flipped from Unreal
							FVector Position;
							Position.X = Decode_Signed_17_6(&ReceiveBuffer[BlissPacketDefinition::Y]);
							Position.Y = Decode_Signed_17_6(&ReceiveBuffer[BlissPacketDefinition::X]);
							Position.Z = Decode_Signed_17_6(&ReceiveBuffer[BlissPacketDefinition::Z]);

							int32 FocalLengthInt = Decode_Unsigned_24(&ReceiveBuffer[BlissPacketDefinition::FocalLength]);
							int32 FocusDistanceInt = Decode_Unsigned_24(&ReceiveBuffer[BlissPacketDefinition::FocusDistance]);
							int32 UserDefinedDataInt = Decode_Unsigned_16(&ReceiveBuffer[BlissPacketDefinition::UserDefined]);

							float FocalLength = ProcessEncoderData(SavedSourceSettings->FocalLengthEncoderData, FocalLengthInt);
							float FocusDistance = ProcessEncoderData(SavedSourceSettings->FocusDistanceEncoderData, FocusDistanceInt);
							float UserDefinedData = ProcessEncoderData(SavedSourceSettings->UserDefinedEncoderData, UserDefinedDataInt);

							uint8 Checksum = CalculateChecksum(&ReceiveBuffer[BlissPacketDefinition::PacketType], ReceivedDataSize - 1);
							if (Checksum != ReceiveBuffer[BlissPacketDefinition::Checksum])
							{
								UE_LOG(LogLiveLinkBliss, Warning, TEXT("LiveLinkBlissSource: Received packet checksum error - received 0x%02x, calculated 0x%02x"), ReceiveBuffer[BlissPacketDefinition::Checksum], Checksum);
							}

							if (ReceivedDataSize != BlissPacketDefinition::PacketSizeD1)
							{
								UE_LOG(LogLiveLinkBliss, Warning, TEXT("LiveLinkBlissSource: Received packet length mismatch - received 0x%02x, calculated 0x%02x"), ReceivedDataSize, BlissPacketDefinition::PacketSizeD1);
							}

							FLiveLinkFrameDataStruct FrameData(FLiveLinkCameraFrameData::StaticStruct());
							FLiveLinkCameraFrameData* CameraFrameData = FrameData.Cast<FLiveLinkCameraFrameData>();
							CameraFrameData->Transform = FTransform(Orientation, Position);

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

							CameraSubjectName = FString::Printf(TEXT("Camera %d"), CameraId);
							Send(&FrameData, FName(CameraSubjectName));

							FrameCounter++;
						}
						else
						{
							UE_LOG(LogLiveLinkBliss, Warning, TEXT("LiveLinkBlissSource: Unsupported Bliss message type 0x%02x"), ReceiveBuffer[BlissPacketDefinition::PacketType]);
						}
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
