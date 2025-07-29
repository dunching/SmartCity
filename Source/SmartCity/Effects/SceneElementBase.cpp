
#include "SceneElementBase.h"

#include "ActorSequenceComponent.h"

#include "CollisionDataStruct.h"

ASceneElementBase::ASceneElementBase(
	const FObjectInitializer& ObjectInitializer
	):Super(ObjectInitializer)
{
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}
