#include "SceneElement_RadarSweep.h"

ASceneElement_RadarSweep::ASceneElement_RadarSweep(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}
