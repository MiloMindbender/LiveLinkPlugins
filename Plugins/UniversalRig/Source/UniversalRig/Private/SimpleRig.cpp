// Universal camera rig tools
// Copyright 2022 by Greg Corson & Kangaroo Koncepts

#include "SimpleRig.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/CollisionProfile.h"

#define LOCTEXT_NAMESPACE "SimpleRig"

// Sets default values
ASimpleRig::ASimpleRig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// default control values
	MeasuredHorizontal	= 10.0f;
	MeasuredVertical	= 20.0f;
	MeasuredPupil		= 30.0f;
	MeasuredPan			= 0.0f;
	MeasuredTilt		= 0.0f;
	MeasuredRoll		= 0.0f;
	AdjustHorizontal	= 0.0f;
	AdjustVertical		= 0.0f;
	AdjustPupil			= 0.0f;
	AdjustPan			= 0.0f;
	AdjustTilt			= 0.0f;
	AdjustRoll			= 0.0f;

	// create the root component
	TransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TransformComponent"));
	RootComponent = TransformComponent;

	TrackerBase = CreateDefaultSubobject<USceneComponent>(TEXT("TrackerBase"));
	TrackerBase->SetupAttachment(TransformComponent);
	TrackerBase->SetRelativeRotation(FRotator(MeasuredTilt + AdjustTilt, MeasuredPan + AdjustPan, MeasuredRoll + AdjustRoll ));

	TrackerVertical = CreateDefaultSubobject<USceneComponent>(TEXT("TrackerVertical"));
	TrackerVertical->SetupAttachment(TrackerBase);
	TrackerVertical->SetRelativeLocation(FVector(0.f, 0.f, MeasuredVertical + AdjustVertical));

	TrackerHorizontal = CreateDefaultSubobject<USceneComponent>(TEXT("TrackerHorizontal"));
	TrackerHorizontal->SetupAttachment(TrackerVertical);
	TrackerHorizontal->SetRelativeLocation(FVector(0.f, MeasuredHorizontal + AdjustHorizontal, 0.0f));

	TrackerPupil = CreateDefaultSubobject<USceneComponent>(TEXT("TrackerPupil"));
	TrackerPupil->SetupAttachment(TrackerHorizontal);
	TrackerPupil->SetRelativeLocation(FVector(MeasuredPupil + AdjustPupil, 0.f, 0.0f));

#if WITH_EDITORONLY_DATA
	// create preview meshes

	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> TrackerBaseMesh(TEXT("StaticMesh'/UniversalRig/TrackerJoint.TrackerJoint'"));
		PreviewMesh_TrackerBase = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh_TrackerBase"));
		if (PreviewMesh_TrackerBase)
		{
			PreviewMesh_TrackerBase->SetStaticMesh(TrackerBaseMesh.Object);
			PreviewMesh_TrackerBase->SetIsVisualizationComponent(true);
			PreviewMesh_TrackerBase->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			PreviewMesh_TrackerBase->bHiddenInGame = false;
			PreviewMesh_TrackerBase->CastShadow = false;

			PreviewMesh_TrackerBase->SetupAttachment(TrackerBase);		
			PreviewMesh_TrackerBase->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.01f));
		}

		static ConstructorHelpers::FObjectFinder<UStaticMesh> VerticalRodMesh(TEXT("StaticMesh'/UniversalRig/VerticalRod.VerticalRod'"));
		PreviewMesh_VerticalRod = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh_VerticalRod"));
		if (PreviewMesh_VerticalRod)
		{
			PreviewMesh_VerticalRod->SetStaticMesh(VerticalRodMesh.Object);
			PreviewMesh_VerticalRod->SetIsVisualizationComponent(true);
			PreviewMesh_VerticalRod->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			PreviewMesh_VerticalRod->bHiddenInGame = false;
			PreviewMesh_VerticalRod->CastShadow = false;

			PreviewMesh_VerticalRod->SetupAttachment(TrackerBase);		
			float RodLength = MeasuredVertical + AdjustVertical;
			PreviewMesh_VerticalRod->SetRelativeLocation(FVector(0.0f, 0.0f, RodLength/2.0f));
			PreviewMesh_VerticalRod->SetRelativeScale3D(FVector(0.01f, 0.01f, RodLength/100.0f));
		}

		static ConstructorHelpers::FObjectFinder<UStaticMesh> TrackerVerticalMesh(TEXT("StaticMesh'/UniversalRig/VerticalJoint.VerticalJoint'"));
		PreviewMesh_TrackerVertical = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh_TrackerVertical"));
		if (PreviewMesh_TrackerVertical)
		{
			PreviewMesh_TrackerVertical->SetStaticMesh(TrackerVerticalMesh.Object);
			PreviewMesh_TrackerVertical->SetIsVisualizationComponent(true);
			PreviewMesh_TrackerVertical->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			PreviewMesh_TrackerVertical->bHiddenInGame = false;
			PreviewMesh_TrackerVertical->CastShadow = false;

			PreviewMesh_TrackerVertical->SetupAttachment(TrackerVertical);		
			PreviewMesh_TrackerVertical->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.01f));
		}

		static ConstructorHelpers::FObjectFinder<UStaticMesh> HorizontalRodMesh(TEXT("StaticMesh'/UniversalRig/HorizontalRod.HorizontalRod'"));
		PreviewMesh_HorizontalRod = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh_HorizontalRod"));
		if (PreviewMesh_HorizontalRod)
		{
			PreviewMesh_HorizontalRod->SetStaticMesh(HorizontalRodMesh.Object);
			PreviewMesh_HorizontalRod->SetIsVisualizationComponent(true);
			PreviewMesh_HorizontalRod->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			PreviewMesh_HorizontalRod->bHiddenInGame = false;
			PreviewMesh_HorizontalRod->CastShadow = false;

			PreviewMesh_HorizontalRod->SetupAttachment(TrackerVertical);
			float RodLength = MeasuredHorizontal + AdjustHorizontal;
			PreviewMesh_HorizontalRod->SetRelativeLocation(FVector(0.0f, 0.0f , 0.0f));
			PreviewMesh_HorizontalRod->SetRelativeScale3D(FVector(0.01f, 0.01f, 1.0f));
			PreviewMesh_HorizontalRod->SetRelativeRotation(FRotator(90.0f, 90.0f, 0.0f));
		}

		static ConstructorHelpers::FObjectFinder<UStaticMesh> TrackerHorizontalMesh(TEXT("StaticMesh'/UniversalRig/HorizontalJoint.HorizontalJoint'"));
		PreviewMesh_TrackerHorizontal = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh_TrackerHorizontal"));
		if (PreviewMesh_TrackerHorizontal)
		{
			PreviewMesh_TrackerHorizontal->SetStaticMesh(TrackerHorizontalMesh.Object);
			PreviewMesh_TrackerHorizontal->SetIsVisualizationComponent(true);
			PreviewMesh_TrackerHorizontal->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			PreviewMesh_TrackerHorizontal->bHiddenInGame = false;
			PreviewMesh_TrackerHorizontal->CastShadow = false;

			PreviewMesh_TrackerHorizontal->SetupAttachment(TrackerHorizontal);		// sibling of yawcontrol
			PreviewMesh_TrackerHorizontal->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.01f));
		}

		static ConstructorHelpers::FObjectFinder<UStaticMesh> PupilRodMesh(TEXT("StaticMesh'/UniversalRig/PupilRod.PupilRod'"));
		PreviewMesh_PupilRod = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh_PupilRod"));
		if (PreviewMesh_PupilRod)
		{
			PreviewMesh_PupilRod->SetStaticMesh(PupilRodMesh.Object);
			PreviewMesh_PupilRod->SetIsVisualizationComponent(true);
			PreviewMesh_PupilRod->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			PreviewMesh_PupilRod->bHiddenInGame = false;
			PreviewMesh_PupilRod->CastShadow = false;

			PreviewMesh_PupilRod->SetupAttachment(TrackerHorizontal);
			float RodLength = MeasuredPupil + AdjustPupil;
			PreviewMesh_PupilRod->SetRelativeLocation(FVector(RodLength / 2.0f, 0.0f, 0.0));
			PreviewMesh_PupilRod->SetRelativeScale3D(FVector(0.01f, 0.01f, RodLength / 100.0f));
			PreviewMesh_PupilRod->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

		}

		static ConstructorHelpers::FObjectFinder<UStaticMesh> TrackerPupilMesh(TEXT("StaticMesh'/UniversalRig/PupilJoint.PupilJoint'"));
		PreviewMesh_TrackerPupil = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh_TrackerPupil"));
		if (PreviewMesh_TrackerPupil)
		{
			PreviewMesh_TrackerPupil->SetStaticMesh(TrackerPupilMesh.Object);
			PreviewMesh_TrackerPupil->SetIsVisualizationComponent(true);
			PreviewMesh_TrackerPupil->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			PreviewMesh_TrackerPupil->bHiddenInGame = false;
			PreviewMesh_TrackerPupil->CastShadow = false;

			PreviewMesh_TrackerPupil->SetupAttachment(TrackerPupil);		// sibling of yawcontrol
			PreviewMesh_TrackerPupil->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.01f));

		}

		UpdatePreviewMeshes();

	}

#endif
}
#if WITH_EDITORONLY_DATA
void ASimpleRig::UpdatePreviewMeshes()
{
	if (PreviewMesh_VerticalRod)
	{
		float RodLength = MeasuredVertical + AdjustVertical;
		PreviewMesh_VerticalRod->SetRelativeLocation(FVector(0.0f, 0.0f, RodLength / 2.0f));
		PreviewMesh_VerticalRod->SetRelativeScale3D(FVector(0.01f, 0.01f, RodLength / 100.0f));
	}
	if (PreviewMesh_HorizontalRod)
	{
		float RodLength = MeasuredHorizontal + AdjustHorizontal;
		PreviewMesh_HorizontalRod->SetRelativeLocation(FVector(0.0f, RodLength / 2.0f, 0.0f));
		PreviewMesh_HorizontalRod->SetRelativeScale3D(FVector(0.01f, 0.01f, RodLength/100.0f));
	}
	if (PreviewMesh_PupilRod)
	{
		float RodLength = MeasuredPupil + AdjustPupil;
		PreviewMesh_PupilRod->SetRelativeLocation(FVector(RodLength/2.0f, 0.0f, 0.0));
		PreviewMesh_PupilRod->SetRelativeScale3D(FVector(0.01f, 0.01f, RodLength/100.0f));
	}
	#if 0
	if (PreviewMesh_CraneArm)
	{
		// note to explain the math here:
		// this tailored to the dimensions of the asset (29cm in scaling direction, pivot at the edge)
		float const CraneArmYScale = CraneArmLength / CraneArmMesh_DefaultMeshSize;

		FVector NewRelScale3D = PreviewMesh_CraneArm->GetRelativeScale3D();
		NewRelScale3D.Y = CraneArmYScale * 0.7f;

		PreviewMesh_CraneArm->SetRelativeScale3D(NewRelScale3D);
	}

	if (PreviewMesh_CraneMount)
	{
		const float ZOffset = 40.f;		// make it appear in the right place
		FVector const NewLoc(CraneArmLength, 0.f, ZOffset);
		PreviewMesh_CraneMount->SetRelativeLocation(NewLoc);
	}
#endif
}
#endif
void ASimpleRig::UpdateASimpleRigComponents()
{
	TrackerBase->SetRelativeRotation(FRotator(MeasuredTilt + AdjustTilt, MeasuredPan + AdjustPan, MeasuredRoll + AdjustRoll));
	TrackerVertical->SetRelativeLocation(FVector(0.f, 0.f, MeasuredVertical + AdjustVertical));
	TrackerHorizontal->SetRelativeLocation(FVector(0.f, MeasuredHorizontal + AdjustHorizontal, 0.0f));
	TrackerPupil->SetRelativeLocation(FVector(MeasuredPupil + AdjustPupil, 0.f, 0.0f));
#if WITH_EDITORONLY_DATA
	UpdatePreviewMeshes();
#endif
}

// Called every frame
void ASimpleRig::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateASimpleRigComponents();

}

#if WITH_EDITOR
void ASimpleRig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateASimpleRigComponents();
}

void ASimpleRig::PostEditUndo()
{
	Super::PostEditUndo();

	UpdateASimpleRigComponents();
}
#endif // WITH_EDITOR


USceneComponent* ASimpleRig::GetDefaultAttachComponent() const
{
	return TrackerPupil;
}

bool ASimpleRig::ShouldTickIfViewportsOnly() const
{
	return true;
}
#undef LOCTEXT_NAMESPACE
