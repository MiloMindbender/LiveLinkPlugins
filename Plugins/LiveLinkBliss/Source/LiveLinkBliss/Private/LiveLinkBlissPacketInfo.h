// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"

//
// Bliss protocol is changing, this is the current one for 0.9.2
//
// All floats

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
