
#include "RadarSweep.h"

inline ARadarSweep::ARadarSweep(
	const FObjectInitializer& ObjectInitializer
	)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	RootComponent = StaticMeshComponent;

}
