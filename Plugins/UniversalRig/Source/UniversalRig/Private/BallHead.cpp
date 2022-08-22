// Universal camera rig tools
// Copyright 2022 by Greg Corson & Kangaroo Koncepts


#include "BallHead.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/CollisionProfile.h"

#define LOCTEXT_NAMESPACE "BallHead"

// Sets default values
ABallHead::ABallHead(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// default control values
	BaseDiameter		= 3.0f;
	BallCenterHeight	= 4.0f;
	ShaftLength			= 4.0f;
	BallPan				= 0.0f;
	BallTilt			= 0.0f;
	ShaftRotate			= 0.0f;
	BaseRotate			= 0.0f;

	// create the root component
	TransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TransformComponent"));
	RootComponent = TransformComponent;

	ShaftJoint = CreateDefaultSubobject<USceneComponent>(TEXT("Ball Head Shaft"));
	ShaftJoint->SetupAttachment(TransformComponent);
	ShaftJoint->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	ShaftJoint->SetRelativeRotation(FRotator(0.0f, ShaftRotate, 0.0f));

	BallJoint = CreateDefaultSubobject<USceneComponent>(TEXT("Ball Head Ball"));
	BallJoint->SetupAttachment(ShaftJoint);
	BallJoint->SetRelativeLocation(FVector(0.0f, 0.0f, -ShaftLength));
	BallJoint->SetRelativeRotation(FRotator(BallTilt, BallPan, 0.0f));

	BaseJoint = CreateDefaultSubobject<USceneComponent>(TEXT("Ball Head Base"));
	BaseJoint->SetupAttachment(BallJoint);
	BaseJoint->SetRelativeLocation(FVector(0.0f, 0.0f, -BallCenterHeight));
	BaseJoint->SetRelativeRotation(FRotator(0.0f, BaseRotate, 0.0f));

#if WITH_EDITORONLY_DATA
	// create preview meshes

	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> BallHeadBaseMesh(TEXT("StaticMesh'/UniversalRig/BallHeadBase.BallHeadBase'"));
		PreviewMesh_BallHeadBase = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh_BallHeadBase"));
		if (PreviewMesh_BallHeadBase)
		{
			PreviewMesh_BallHeadBase->SetStaticMesh(BallHeadBaseMesh.Object);
			PreviewMesh_BallHeadBase->SetIsVisualizationComponent(true);
			PreviewMesh_BallHeadBase->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			PreviewMesh_BallHeadBase->bHiddenInGame = false;
			PreviewMesh_BallHeadBase->CastShadow = false;

			PreviewMesh_BallHeadBase->SetupAttachment(BaseJoint);
			PreviewMesh_BallHeadBase->SetRelativeLocation(FVector(0.0f, 0.0f, BallCenterHeight/2.0f));
			PreviewMesh_BallHeadBase->SetRelativeScale3D(FVector(BaseDiameter/100.0f, BaseDiameter/100.0f, BallCenterHeight/100.0f));
		}

		static ConstructorHelpers::FObjectFinder<UStaticMesh> BallHeadBallMesh(TEXT("StaticMesh'/UniversalRig/BallHeadBall.BallHeadBall'"));
		PreviewMesh_BallHeadBall = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh_BallHeadBall"));
		if (PreviewMesh_BallHeadBall)
		{
			PreviewMesh_BallHeadBall->SetStaticMesh(BallHeadBallMesh.Object);
			PreviewMesh_BallHeadBall->SetIsVisualizationComponent(true);
			PreviewMesh_BallHeadBall->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			PreviewMesh_BallHeadBall->bHiddenInGame = false;
			PreviewMesh_BallHeadBall->CastShadow = false;

			PreviewMesh_BallHeadBall->SetupAttachment(BallJoint);
			PreviewMesh_BallHeadBall->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
			PreviewMesh_BallHeadBall->SetRelativeScale3D(FVector(BaseDiameter * 0.008, BaseDiameter * 0.008, BaseDiameter * 0.008));
		}

		static ConstructorHelpers::FObjectFinder<UStaticMesh> BallHeadShaftMesh(TEXT("StaticMesh'/UniversalRig/BallHeadShaft.BallHeadShaft'"));
		PreviewMesh_BallHeadShaft = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh_BallHeadShaft"));
		if (PreviewMesh_BallHeadShaft)
		{
			PreviewMesh_BallHeadShaft->SetStaticMesh(BallHeadShaftMesh.Object);
			PreviewMesh_BallHeadShaft->SetIsVisualizationComponent(true);
			PreviewMesh_BallHeadShaft->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			PreviewMesh_BallHeadShaft->bHiddenInGame = false;
			PreviewMesh_BallHeadShaft->CastShadow = false;

			PreviewMesh_BallHeadShaft->SetupAttachment(ShaftJoint);
			PreviewMesh_BallHeadShaft->SetRelativeLocation(FVector(0.0f, 0.0f, -ShaftLength / 2.0f));
			PreviewMesh_BallHeadShaft->SetRelativeScale3D(FVector(BaseDiameter * 0.003, BaseDiameter * 0.003, ShaftLength * 0.01f));
		}

		static ConstructorHelpers::FObjectFinder<UStaticMesh> BallHeadTopMesh(TEXT("StaticMesh'/UniversalRig/BallHeadTop.BallHeadTop'"));
		PreviewMesh_BallHeadTop = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh_BallHeadTop"));
		if (PreviewMesh_BallHeadTop)
		{
			PreviewMesh_BallHeadTop->SetStaticMesh(BallHeadTopMesh.Object);
			PreviewMesh_BallHeadTop->SetIsVisualizationComponent(true);
			PreviewMesh_BallHeadTop->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			PreviewMesh_BallHeadTop->bHiddenInGame = false;
			PreviewMesh_BallHeadTop->CastShadow = false;

			PreviewMesh_BallHeadTop->SetupAttachment(ShaftJoint);
			PreviewMesh_BallHeadTop->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
			PreviewMesh_BallHeadTop->SetRelativeScale3D(FVector(BaseDiameter * 0.005, BaseDiameter * 0.005, 0.001f));
		}

		UpdatePreviewMeshes();

	}

#endif
}
#if WITH_EDITORONLY_DATA
void ABallHead::UpdatePreviewMeshes()
{
	if (PreviewMesh_BallHeadBase)
	{
		PreviewMesh_BallHeadBase->SetRelativeLocation(FVector(0.0f, 0.0f, BallCenterHeight/2.0f));
		PreviewMesh_BallHeadBase->SetRelativeScale3D(FVector(BaseDiameter/100.0f, BaseDiameter/100.0f, BallCenterHeight/100.0f));
	}
	if (PreviewMesh_BallHeadBall)
	{
		PreviewMesh_BallHeadBall->SetRelativeScale3D(FVector(BaseDiameter * 0.008, BaseDiameter * 0.008, BaseDiameter * 0.008));
	}
	if (PreviewMesh_BallHeadShaft)
	{
		PreviewMesh_BallHeadShaft->SetRelativeLocation(FVector(0.0f, 0.0f, -ShaftLength / 2.0f));
		PreviewMesh_BallHeadShaft->SetRelativeScale3D(FVector(BaseDiameter * 0.003, BaseDiameter * 0.003, ShaftLength * 0.01f));
	}
	if (PreviewMesh_BallHeadTop)
	{
		PreviewMesh_BallHeadTop->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		PreviewMesh_BallHeadTop->SetRelativeScale3D(FVector(BaseDiameter * 0.005, BaseDiameter * 0.005, 0.001f));
	}

}
#endif

void ABallHead::UpdateABallHeadComponents()
{
	ShaftJoint->SetRelativeRotation(FRotator(0.0f, ShaftRotate, 0.0f));
	BallJoint->SetRelativeLocation(FVector(0.0f, 0.0f, -ShaftLength));
	BallJoint->SetRelativeRotation(FRotator(BallTilt, BallPan, 0.0f));
	BaseJoint->SetRelativeLocation(FVector(0.0f, 0.0f, -BallCenterHeight));
	BaseJoint->SetRelativeRotation(FRotator(0.0f, BaseRotate, 0.0f));

#if WITH_EDITORONLY_DATA
	UpdatePreviewMeshes();
#endif
}

// Called when the game starts or when spawned
void ABallHead::BeginPlay()
{
	Super::BeginPlay();
	
}

#if WITH_EDITOR
void ABallHead::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateABallHeadComponents();
}

void ABallHead::PostEditUndo()
{
	Super::PostEditUndo();

	UpdateABallHeadComponents();
}
#endif // WITH_EDITOR

// Called every frame
void ABallHead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateABallHeadComponents();

}

USceneComponent* ABallHead::GetDefaultAttachComponent() const
{
	return BaseJoint;
}

bool ABallHead::ShouldTickIfViewportsOnly() const
{
	return true;
}
#undef LOCTEXT_NAMESPACE
