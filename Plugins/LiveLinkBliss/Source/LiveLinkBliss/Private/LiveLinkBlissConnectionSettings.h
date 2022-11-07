// Portions Copyright Epic Games, Inc. All Rights Reserved.
// Portions Copyright Greg Corson, All Rights Reserved
// This LiveLink code is based on the original Epic FreeD plugin, this was necessary because Epic does not supply documentation on how to use LiveLink
// I have added comments to make the code easier to understand.

#pragma once

#include "CoreMinimal.h"
#include "LiveLinkBlissConnectionSettings.generated.h"

// This structure has the default IP address and default port number that will be filled in by livelink when the source is created
USTRUCT()
struct LIVELINKBLISS_API FLiveLinkBlissConnectionSettings
{
	GENERATED_BODY()

public:
	/** Default IP address of the Bliss tracking source */
	UPROPERTY(EditAnywhere, Category = "Connection Settings")
	FString IPAddress = TEXT("127.0.0.1");

	/** Default UDP port number */
	UPROPERTY(EditAnywhere, Category = "Connection Settings")
	uint16 UDPPortNumber = 50000;
};
