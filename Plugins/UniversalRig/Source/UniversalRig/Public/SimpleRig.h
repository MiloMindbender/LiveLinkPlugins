// Universal camera rig tools
// Copyright 2022 by Greg Corson & Kangaroo Koncepts

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"
#include "SimpleRig.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class UNIVERSALRIG_API ASimpleRig : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimpleRig(const FObjectInitializer& ObjectInitialier);

	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;

	// Properties of the rig
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Measurements", meta = (Units = cm, ToolTip = "Horizontal offset from the Entrance Pupil (green cube"))
		float MeasuredHorizontal;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Measurements", meta = (Units = cm, ToolTip = "Vertical offset from the Entrance Pupil (blue cube"))
		float MeasuredVertical;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Measurements", meta = (Units = cm, ToolTip = "Forward/back offset to the Entrance Pupil (red cube)"))
		float MeasuredPupil;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Measurements", meta = (UIMin = "-360", UIMax = "360", Units = deg, ToolTip = "Pan offset at the tracker end (yellow cube)"))
		float MeasuredPan;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Measurements", meta = (UIMin = "-360", UIMax = "360", Units = deg, ToolTip = "Tilt offset at the tracker end (yellow cube)"))
		float MeasuredTilt;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Measurements", meta = (UIMin = "-360", UIMax = "360", Units = deg, ToolTip = "Roll offset at the tracker end (yellow cube)"))
		float MeasuredRoll;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Fine Tuning", meta = (UIMin = "-5", UIMax = "5", Units = cm, ToolTip = "For fine tuning (more sensitive)"))
		float AdjustHorizontal;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Fine Tuning", meta = (UIMin = "-5", UIMax = "5", Units = cm, ToolTip = "For fine tuning (more sensitive)"))
		float AdjustVertical;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Fine Tuning", meta = (UIMin = "-5", UIMax = "5", Units = cm, ToolTip = "For fine tuning (more sensitive)"))
		float AdjustPupil;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Fine Tuning", meta = (UIMin = "-10", UIMax = "10", Units = deg, ToolTip = "For fine tuning (more sensitive)"))
		float AdjustPan;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Fine Tuning", meta = (UIMin = "-10", UIMax = "10", Units = deg, ToolTip = "For fine tuning (more sensitive)"))
		float AdjustTilt;
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Rig Fine Tuning", meta = (UIMin = "-10", UIMax = "10", Units = deg, ToolTip = "For fine tuning (more sensitive)"))
		float AdjustRoll;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;
#endif	
	
	virtual class USceneComponent* GetDefaultAttachComponent() const override;

private:
#if WITH_EDITORONLY_DATA
	void UpdatePreviewMeshes();
#endif
	void UpdateASimpleRigComponents();

	UPROPERTY(EditDefaultsOnly, Category = "SimpleRig Components")
		TObjectPtr<USceneComponent> TransformComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "SimpleRig Components")
		TObjectPtr<USceneComponent> TrackerBase;

	UPROPERTY(EditDefaultsOnly, Category = "SimpleRig Components")
		TObjectPtr<USceneComponent> TrackerVertical;

	UPROPERTY(EditDefaultsOnly, Category = "SimpleRig Components")
		TObjectPtr<USceneComponent> TrackerHorizontal;

	UPROPERTY(EditDefaultsOnly, Category = "SimpleRig Components")
		TObjectPtr<USceneComponent> TrackerPupil;

#if WITH_EDITORONLY_DATA
	/** Preview meshes for visualization */
	UPROPERTY(Transient)
		TObjectPtr<UStaticMeshComponent> PreviewMesh_TrackerBase;

	UPROPERTY(Transient)
		TObjectPtr<UStaticMeshComponent> PreviewMesh_TrackerVertical;

	UPROPERTY(Transient)
		TObjectPtr<UStaticMeshComponent> PreviewMesh_VerticalRod;

	UPROPERTY(Transient)
		TObjectPtr<UStaticMeshComponent> PreviewMesh_TrackerHorizontal;

	UPROPERTY(Transient)
		TObjectPtr<UStaticMeshComponent> PreviewMesh_HorizontalRod;

	UPROPERTY(Transient)
		TObjectPtr<UStaticMeshComponent> PreviewMesh_TrackerPupil;

	UPROPERTY(Transient)
		TObjectPtr<UStaticMeshComponent> PreviewMesh_PupilRod;

#endif
};
