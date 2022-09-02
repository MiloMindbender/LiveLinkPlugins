// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LiveLinkSourceSettings.h"
#include "LiveLinkBlissSourceSettings.generated.h"

USTRUCT()
struct FBlissEncoderData
{
	GENERATED_BODY()

	/** Is this encoder data valid? */
	UPROPERTY(EditAnywhere, Category = "Encoder Data")
	bool bIsValid;

	/** Invert the encoder input direction */
	UPROPERTY(EditAnywhere, Category = "Encoder Data", meta = (EditCondition = "bIsValid"))
	bool bInvertEncoder;

	/** Use manual Min/Max values for the encoder normalization (normally uses dynamic auto ranging based on inputs) */
	UPROPERTY(EditAnywhere, Category = "Encoder Data", meta = (EditCondition = "bIsValid"))
	bool bUseManualRange;

	/** Minimum raw encoder value */
	UPROPERTY(EditAnywhere, Category = "Encoder Data", meta = (ClampMin = 0, ClampMax = 0x00ffffff, EditCondition = "bIsValid && bUseManualRange"))
	int32 Min;

	/** Maximum raw encoder value */
	UPROPERTY(EditAnywhere, Category = "Encoder Data", meta = (ClampMin = 0, ClampMax = 0x00ffffff, EditCondition = "bIsValid && bUseManualRange"))
	int32 Max;

	/** Mask bits for raw encoder value */
	UPROPERTY(EditAnywhere, Category = "Encoder Data", meta = (ClampMin = 0, ClampMax = 0x00ffffff, EditCondition = "bIsValid"))
	int32 MaskBits;
};

UENUM(BlueprintType)
enum class EBlissDefaultConfigs : uint8
{
	Generic,
	Panasonic,
	Sony,
	Stype UMETA(DisplayName="stYpe"),
	Mosys,
	Ncam
};

UCLASS()
class LIVELINKBLISS_API ULiveLinkBlissSourceSettings : public ULiveLinkSourceSettings
{
	GENERATED_BODY()

public:
	/** Send extra string meta data (Camera ID and FrameCounter) */
	UPROPERTY(EditAnywhere, Category = "Source")
	bool bSendExtraMetaData = false;

	/** Default configurations for specific manufacturers */
	UPROPERTY(EditAnywhere, Category = "Source")
	EBlissDefaultConfigs DefaultConfig = EBlissDefaultConfigs::Generic;

	/** Raw focus distance (in cm) encoder parameters for this camera - 24 bits max */
	UPROPERTY(EditAnywhere, Category = "Source")
	FBlissEncoderData FocusDistanceEncoderData = { true, false, false, 0x00ffffff, 0, 0x00ffffff };

	/** Raw focal length/zoom (in mm) encoder parameters for this camera - 24 bits max */
	UPROPERTY(EditAnywhere, Category = "Source")
	FBlissEncoderData FocalLengthEncoderData = { true, false, false, 0x00ffffff, 0, 0x00ffffff };

	/** Raw user defined/spare data encoder (normally used for Aperture) parameters for this camera - 16 bits max */
	UPROPERTY(EditAnywhere, Category = "Source")
	FBlissEncoderData UserDefinedEncoderData = { false, false, false, 0x0000ffff, 0, 0x0000ffff };
};
