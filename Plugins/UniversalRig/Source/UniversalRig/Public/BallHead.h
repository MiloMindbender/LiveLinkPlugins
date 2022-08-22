// Universal camera rig tools
// Copyright 2022 by Greg Corson & Kangaroo Koncepts

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"
#include "BallHead.generated.h"

// Class definition for the ballhead class that inherits from an AActor
UCLASS(Blueprintable)
class UNIVERSALRIG_API ABallHead : public AActor
{
	GENERATED_BODY()
	
public:	
	// Constructor, Sets default values for this actor's properties
	ABallHead(const FObjectInitializer& ObjectInitialier);

	// These 

	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Ball Head Measurements", meta = (UIMin = "1", UIMax = "100", Units = cm, ToolTip = "Diameter of the bottom part of the ballhead"))
		float BaseDiameter;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Ball Head Measurements", meta = (UIMin = "1", UIMax = "100", Units = cm, ToolTip = "Measured from the bottom of the base to the center of the ball"))
		float BallCenterHeight;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Ball Head Measurements", meta = (UIMin = "1", UIMax = "100", Units = cm, ToolTip = "Measured from the center of the ball to the end"))
		float ShaftLength;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Ball Head Position", meta = (UIMin = "-360", UIMax = "360", Units = deg, ToolTip = "Pan angle at the center of the ball"))
		float BallPan;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Ball Head Position", meta = (UIMin = "-360", UIMax = "360", Units = deg, ToolTip = "Tilt angle at the center of the ball"))
		float BallTilt;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Ball Head Position", meta = (UIMin = "-360", UIMax = "360", Units = deg, ToolTip = "Rotation of the shaft around it's axis"))
		float ShaftRotate;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Ball Head Position", meta = (UIMin = "-360", UIMax = "360", Units = deg, ToolTip = "Rotation of the base of the ballhead"))
		float BaseRotate;

	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual class USceneComponent* GetDefaultAttachComponent() const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;
#endif	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
#if WITH_EDITORONLY_DATA
	void UpdatePreviewMeshes();
#endif
	void UpdateABallHeadComponents();

	UPROPERTY(EditDefaultsOnly, Category = "Ball Head Components")
		TObjectPtr<USceneComponent> TransformComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Ball Head Components")
		TObjectPtr<USceneComponent> BaseJoint;
	UPROPERTY(EditDefaultsOnly, Category = "Ball Head Components")
		TObjectPtr<USceneComponent> BallJoint;
	UPROPERTY(EditDefaultsOnly, Category = "Ball Head Components")
		TObjectPtr<USceneComponent> ShaftJoint;

#if WITH_EDITORONLY_DATA
	/** Preview meshes for visualization */
	UPROPERTY(Transient)
		TObjectPtr<UStaticMeshComponent> PreviewMesh_BallHeadBase;
	UPROPERTY(Transient)
		TObjectPtr<UStaticMeshComponent> PreviewMesh_BallHeadBall;
	UPROPERTY(Transient)
		TObjectPtr<UStaticMeshComponent> PreviewMesh_BallHeadShaft;
	UPROPERTY(Transient)
		TObjectPtr<UStaticMeshComponent> PreviewMesh_BallHeadTop;

#endif
};
