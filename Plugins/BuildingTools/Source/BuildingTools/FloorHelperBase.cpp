#include "FloorHelperBase.h"

#include "Components/BoxComponent.h"

AFloorHelperBase::AFloorHelperBase(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	SceneComponentPtr = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SceneComponentPtr->SetMobility(EComponentMobility::Movable);
	SceneComponentPtr->SetupAttachment(RootComponent);

	BoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponentPtr->SetupAttachment(RootComponent);
	BoxComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InnerBoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("InnerBoxComponent"));
	InnerBoxComponentPtr->SetupAttachment(RootComponent);
	InnerBoxComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFloorHelperBase::BeginPlay()
{
	Super::BeginPlay();

	InnerBoxComponentPtr->SetRelativeTransform(BoxComponentPtr->GetRelativeTransform());
	auto Box = BoxComponentPtr->GetScaledBoxExtent();

	Box.X = Box.X - 200;
	Box.Y = Box.Y - 200;
	Box.Z = Box.Z - 200;
	
	InnerBoxComponentPtr->SetBoxExtent(Box);
}

void AFloorHelperBase::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);
}

void AFloorHelperBase::PostActorCreated()
{
	Super::PostActorCreated();
}

void AFloorHelperBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AFloorHelperBase::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
}

TPair<FTransform, float> AFloorHelperBase::GetCameraSeat(
	const FRotator& Rot,
	float FOV
	) const
{
	TPair<FTransform, float> Result;

	auto Box = BoxComponentPtr->GetLocalBounds();
	
	Result.Key.SetLocation(BoxComponentPtr->GetComponentLocation());
	Result.Key.SetRotation(Rot.Quaternion());

	const auto Radians = FMath::DegreesToRadians(45.0f);
	const auto Value = FMath::Cos(Radians);
	Result.Value = Box.BoxExtent.Size() / Value;

	DrawDebugBox(
	             GetWorld(),
	             BoxComponentPtr->GetComponentLocation(),
	             Box.BoxExtent,
	             FColor::Green,
	             false,
	             10
	            );

	return Result;
}
