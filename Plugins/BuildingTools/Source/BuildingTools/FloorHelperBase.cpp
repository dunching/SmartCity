#include "FloorHelperBase.h"

#include "Components/BoxComponent.h"

AFloorHelperBase::AFloorHelperBase(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	SceneComponentPtr = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SceneComponentPtr->SetMobility(EComponentMobility::Movable);

	RootComponent = SceneComponentPtr;
	
	BoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponentPtr->SetupAttachment(RootComponent);

	BoxComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFloorHelperBase::BeginPlay()
{
	Super::BeginPlay();
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
