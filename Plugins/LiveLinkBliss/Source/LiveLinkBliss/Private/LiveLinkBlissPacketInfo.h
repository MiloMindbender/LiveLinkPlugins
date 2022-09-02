// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"

//
// Bliss protocol is changing, this is the current one for 0.9
//
#if 0
// All floats
struct BlissPacketDefinition
{
	static const float X;				// First group of transforms is the kalhman filtered ones
	static const float Y;
	static const float Z;
	static const float Roll;
	static const float Pitch;
	static const float Yaw;
	static const float Unfiltered_X;	// next group of transformms are unfiltered
	static const float Unfiltered_Y;
	static const float Unfiltered_Z;
	static const float Unfiltered_Roll;
	static const float Unfiltered_Pitch;
	static const float Unfiltered_Yaw;
	static const float Host_Time;		// Timestamp when the data was read at the host
	static const float Sensor_Time;		// Timestamp generated on the sensor
};

#endif

// !!!GAC need to redefine this.
struct BlissPacketDefinition
{
	static const uint8 PacketTypeD1;
	static const uint8 PacketSizeD1;

	static const uint8 PacketType;
	static const uint8 CameraID;
	static const uint8 Yaw;
	static const uint8 Pitch;
	static const uint8 Roll;
	static const uint8 X;
	static const uint8 Y;
	static const uint8 Z;
	static const uint8 FocalLength;
	static const uint8 FocusDistance;
	static const uint8 UserDefined;
	static const uint8 Checksum;
};
