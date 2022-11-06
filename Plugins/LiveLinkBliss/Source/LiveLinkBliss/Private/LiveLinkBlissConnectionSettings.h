// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LiveLinkBlissConnectionSettings.generated.h"

USTRUCT()
struct LIVELINKBLISS_API FLiveLinkBlissConnectionSettings
{
	GENERATED_BODY()

public:
	/** IP address of the Bliss tracking source */
	UPROPERTY(EditAnywhere, Category = "Connection Settings")
	FString IPAddress = TEXT("127.0.0.1");

	/** UDP port number */
	UPROPERTY(EditAnywhere, Category = "Connection Settings")
	uint16 UDPPortNumber = 50000;
};
