#include "FloorHelper.h"

ABuilding_Floor_Mask::ABuilding_Floor_Mask(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void ABuilding_Floor_Mask::SetFloor(
	AFloorHelper* FloorPtr
	)
{
	if (FloorPtr)
	{
		FVector Origin;
		FVector BoxExtent;

		FloorPtr->GetActorBounds(false, Origin, BoxExtent);

		SetActorLocation(Origin - FVector(0, 0, BoxExtent.Z));
	}
}
