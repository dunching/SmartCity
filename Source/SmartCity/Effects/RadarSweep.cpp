
#include "RadarSweep.h"

ARadarSweep::ARadarSweep(
	const FObjectInitializer& ObjectInitializer
	)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	RootComponent = StaticMeshComponent;

}
