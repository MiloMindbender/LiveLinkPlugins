// Portions Copyright Epic Games, Inc. All Rights Reserved.
// Portions Copyright Greg Corson, All Rights Reserved
// This LiveLink code is based on the original Epic FreeD plugin, this was necessary because Epic does not supply documentation on how to use LiveLink
// I have added comments to make the code easier to understand.
#pragma once

#include "CoreTypes.h"

// This structure stores the offsets to each data item in the messages RETracker software sends for the bliss tracker
// The actual offset values are at the beginning of LiveLinkBlissSource.cpp
// Some changes are being made as new features are added
// The current protocol was setup so the old bliss software will still work with this plugin
// In the future this may change.  "Message_Type" is used to identify which version of the protocol is
// being used so future plugins can automatically adapt.

#if 0
struct BlissPacketDefinition
{
	static const uint8 PacketSize;

	static const uint8 X;				// First group of transforms is the kalhman filtered ones
	static const uint8 Y;
	static const uint8 Z;
	static const uint8 Roll;
	static const uint8 Pitch;
	static const uint8 Yaw;
	static const uint8 Unfiltered_X;	// next group of transformms are unfiltered
	static const uint8 Unfiltered_Y;
	static const uint8 Unfiltered_Z;
	static const uint8 Unfiltered_Roll;
	static const uint8 Unfiltered_Pitch;
	static const uint8 Unfiltered_Yaw;
	static const uint8 Confidence;
	static const uint8 Host_Time;		// Timestamp when the data was read at the host
	static const uint8 Sensor_Time;		// Timestamp generated on the sensor
};
#else
struct BlissPacketDefinition
{
	static const uint8 PacketSize;

	static const uint8 X;				// First group of transforms is the kalhman filtered ones
	static const uint8 Y;
	static const uint8 Z;
	static const uint8 Roll;
	static const uint8 Pitch;
	static const uint8 Yaw;
	static const uint8 Padding1;	// next group of transformms are unfiltered
	static const uint8 Padding2;
	static const uint8 Padding3;
	static const uint8 Padding4;
	static const uint8 Camera_ID;
	static const uint8 Message_Type;
	static const uint8 Confidence;
	static const uint8 Host_Time;		// Timestamp when the data was read at the host
	static const uint8 Sensor_Time;		// Timestamp generated on the sensor
};
#endif
