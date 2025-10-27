#include "FloorHelper.h"

#include "Components/BoxComponent.h"

#include "AssetRefMap.h"
#include "FloorHelper_Description.h"
#include "SmartCitySuiteTags.h"

AFloorHelper::AFloorHelper(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	FloorHelper_DescriptionAttachTransform = CreateDefaultSubobject<USceneComponent>(
		 TEXT("FloorHelper_DescriptionAttachTransform")
		);
	FloorHelper_DescriptionAttachTransform->SetMobility(EComponentMobility::Movable);

	FloorHelper_DescriptionAttachTransform->SetupAttachment(BoxComponentPtr);
}

void AFloorHelper::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);

	const auto Box = BoxComponentPtr->GetLocalBounds();
	FloorHelper_DescriptionAttachTransform->SetRelativeLocation(FVector(-Box.BoxExtent.X, -Box.BoxExtent.Y, 0));
	FloorHelper_DescriptionAttachTransform->SetRelativeRotation(FRotator(0, 180, 0));
}

void AFloorHelper::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall)
		)
		{
			if (FloorHelper_DescriptionPtr)
			{
			}
			else
			{
				if (bDisplayDescritpion)
				{
					FActorSpawnParameters SpawnParameters;

					SpawnParameters.CustomPreSpawnInitalization = [this](
						auto ActorPtr
						)
						{
							auto FloorPtr = Cast<AFloorHelper_Description>(ActorPtr);
							if (FloorPtr)
							{
								FloorPtr->SetFloor(this);
							}
						};
					FloorHelper_DescriptionPtr = GetWorld()->SpawnActor<AFloorHelper_Description>(
						 UAssetRefMap::GetInstance()->FloorHelper_DescriptionClass,
						 SpawnParameters
						);
				}
			}
			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor)
		)
		{
			if (FloorHelper_DescriptionPtr)
			{
			}
			else
			{
				if (bDisplayDescritpion)
				{
					FActorSpawnParameters SpawnParameters;

					SpawnParameters.CustomPreSpawnInitalization = [this](
						auto ActorPtr
						)
						{
							auto FloorPtr = Cast<AFloorHelper_Description>(ActorPtr);
							if (FloorPtr)
							{
								FloorPtr->SetFloor(this);
							}
						};
					FloorHelper_DescriptionPtr = GetWorld()->SpawnActor<AFloorHelper_Description>(
						 UAssetRefMap::GetInstance()->FloorHelper_DescriptionClass,
						 SpawnParameters
						);
				}
			}
			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_Focus)
		)
		{
			if (FloorHelper_DescriptionPtr)
			{
				FloorHelper_DescriptionPtr->Destroy();
			}
			FloorHelper_DescriptionPtr = nullptr;

			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}

		if (FloorHelper_DescriptionPtr)
		{
			FloorHelper_DescriptionPtr->Destroy();
		}
		FloorHelper_DescriptionPtr = nullptr;

		return;
	}
}

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
