#include "Elevator.h"

#include "ActorSequenceComponent.h"
#include "AssetRefMap.h"

#include "CollisionDataStruct.h"
#include "FloorHelper.h"

AElevator::AElevator(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AElevator::SwitchState(
	bool bIsActive
	)
{
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (auto Iter : StaticMeshComponents)
	{
		if (bIsActive)
		{
			Iter->SetRenderCustomDepth(true);
			Iter->SetCustomDepthStencilValue(1);
		}
		else
		{
			Iter->SetRenderCustomDepth(false);
		}
	}
}

void AElevator::ChangeTargetFloorIndex(
	int32 FloorIndex
	)
{
	if (TargetFloorIndex != FloorIndex)
	{
		TargetFloorIndex = FloorIndex;

		for (const auto &Iter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			if (Iter.Value->FloorIndex == FloorIndex)
			{
				TargetFloorLocation = Iter.Value->GetActorLocation();
			
				const auto CurrentLocation = GetActorLocation();

				bIsUp = CurrentLocation.Z < TargetFloorLocation.Z;
			
				GetWorldTimerManager().SetTimer(MoveTimerHandle, this, &ThisClass::MoveElevator, Frequence, true);
				
				break;
			}
		}
	}
}

void AElevator::MoveElevator()
{
	if (bIsUp)
	{
		const auto CurrentLocation = GetActorLocation();

		const auto NewLocation = CurrentLocation + (FVector::UpVector * Frequence * MoveSpeed);
		if (NewLocation.Z < TargetFloorLocation.Z)
		{
			SetActorLocation(NewLocation);
		}
		else
		{
			SetActorLocation(FVector(NewLocation.X,NewLocation.Y,TargetFloorLocation.Z));
			GetWorldTimerManager().ClearTimer(MoveTimerHandle);
		}
	}
	else
	{
		const auto CurrentLocation = GetActorLocation();

		const auto NewLocation = CurrentLocation + (FVector::DownVector * Frequence * MoveSpeed);
		if (NewLocation.Z > TargetFloorLocation.Z)
		{
			SetActorLocation(NewLocation);
		}
		else
		{
			SetActorLocation(FVector(NewLocation.X,NewLocation.Y,TargetFloorLocation.Z));
			GetWorldTimerManager().ClearTimer(MoveTimerHandle);
		}
	}
}
