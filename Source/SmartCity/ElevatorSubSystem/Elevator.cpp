#include "Elevator.h"

#include "ActorSequenceComponent.h"
#include "AssetRefMap.h"

#include "CollisionDataStruct.h"
#include "FloorHelper.h"
#include "GameplayTagsLibrary.h"
#include "SmartCitySuiteTags.h"

AElevator::AElevator(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
}

void AElevator::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);
	
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall)&&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator)
			)
		{
			SetActorHiddenInGame(false);

			TArray<UStaticMeshComponent*> StaticMeshComponents;
			GetComponents<UStaticMeshComponent>(StaticMeshComponents);

			for (auto Iter : StaticMeshComponents)
			{
				Iter->SetRenderCustomDepth(true);
				Iter->SetCustomDepthStencilValue(1);
			}
			
			return;
		}
	}

	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}
		
		QuitAllState();

		return;
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
			if (!Iter.Value->bIsSup && (Iter.Value->FloorIndex == FloorIndex))
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

void AElevator::QuitAllState()
{
	Super::QuitAllState();
	
	SetActorHiddenInGame(true);

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (auto Iter : StaticMeshComponents)
	{
		Iter->SetRenderCustomDepth(false);
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
