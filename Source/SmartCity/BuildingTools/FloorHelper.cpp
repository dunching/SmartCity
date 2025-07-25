
#include "FloorHelper.h"

#include "ActorSequenceComponent.h"

#include "CollisionDataStruct.h"

AFloorHelper::AFloorHelper(
	const FObjectInitializer& ObjectInitializer
	):Super(ObjectInitializer)
{
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

}
