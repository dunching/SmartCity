
#include "Elevator.h"

#include "ActorSequenceComponent.h"

#include "CollisionDataStruct.h"

AElevator::AElevator(
	const FObjectInitializer& ObjectInitializer
	):Super(ObjectInitializer)
{
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1 / 30.f;
}

void AElevator::Tick(
	float DeltaSeconds
	)
{
	Super::Tick(DeltaSeconds);
	
}
