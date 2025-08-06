
#include "FloorHelper.h"

#include "Components/BoxComponent.h"

#include "ActorSequenceComponent.h"
#include "CollisionDataStruct.h"

AFloorHelper::AFloorHelper(
	const FObjectInitializer& ObjectInitializer
	):Super(ObjectInitializer)
{
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	BoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponentPtr->SetupAttachment(RootComponent);
}
