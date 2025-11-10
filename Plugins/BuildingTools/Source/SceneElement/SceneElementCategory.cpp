#include "SceneElementCategory.h"

ASceneElementCategory::ASceneElementCategory(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}
