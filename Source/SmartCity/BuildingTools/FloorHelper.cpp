#include "FloorHelper.h"

#include "Components/BoxComponent.h"
#include "Components/RectLightComponent.h"

#include "AssetRefMap.h"
#include "ComputerMark.h"
#include "Dynamic_SkyBase.h"
#include "FloorHelper_Description.h"
#include "SceneElementCategory.h"
#include "SceneElement_Computer.h"
#include "SceneInteractionDecorator_Area.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "ViewerPawnBase.h"
#include "WeatherSystem.h"

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

void AFloorHelper::BeginPlay()
{
	Super::BeginPlay();

	auto Handle = UWeatherSystem::GetInstance()->GetDynamicSky()->OnHourChanged.AddCallback(
		 std::bind(
		           &ThisClass::OnHourChanged,
		           this,
		           std::placeholders::_1
		          )
		);
	Handle->bIsAutoUnregister = false;
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
			OnExternalWall();
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
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_Periphery)
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
		if ((ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)))
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

void AFloorHelper_Computer::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);
}

void AFloorHelper_Computer::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall)
		)
		{
			SetActorHiddenInGame(true);

			if (ComputerMarkRef.ToSoftObjectPath().IsValid())
			{
				ComputerMarkRef.LoadSynchronous()->SetActorHiddenInGame(true);
			}
			
			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_Periphery)
		)
		{
			SetActorHiddenInGame(true);

			if (ComputerMarkRef.ToSoftObjectPath().IsValid())
			{
				ComputerMarkRef.LoadSynchronous()->SetActorHiddenInGame(true);
			}
			
			return;
		}
	}
	{
		if ((ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)))
		{
			SetActorHiddenInGame(false);

			if (ComputerMarkRef.ToSoftObjectPath().IsValid())
			{
				ComputerMarkRef.LoadSynchronous()->SetActorHiddenInGame(false);
			}
			
			return;
		}
	}
}

TMap<FString, TSoftObjectPtr<AViewerPawnBase>> AFloorHelper_Computer::GetPresetBuildingCameraSeat() const
{
	auto Result = Super::GetPresetBuildingCameraSeat();

	for (const auto& Iter : SceneElementCategoryMap)
	{
		TArray<AActor*> OutActors;
	
		Iter.Value->GetAttachedActors(OutActors, true, true);
	
		for (auto ActorIter : OutActors)
		{
			auto SceneElementBasePtr = Cast<ASceneElement_Computer>(ActorIter);
			if (SceneElementBasePtr)
			{
				Result.Add(SceneElementBasePtr->DeviceTypeStr, nullptr);
			}
		}
	}

	return Result;
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

void AFloorHelper::OnHourChanged(
	int32 Hour
	)
{
	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );

	if (!AreaDecoratorSPtr)
	{
		for (auto Iter : RectLightComponentAry)
		{
			Iter->SetHiddenInGame(true);
		}
		return;
	}

	if (!AreaDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Area_ExternalWall))
	{
		for (auto Iter : RectLightComponentAry)
		{
			Iter->SetHiddenInGame(true);
		}
		return;
	}

	if (Hour > 18 || Hour < 8)
	{
		for (auto Iter : RectLightComponentAry)
		{
			// Iter->SetHiddenInGame(false	);
		}
		return;
	}
	for (auto Iter : RectLightComponentAry)
	{
		Iter->SetHiddenInGame(true);
	}
}

void AFloorHelper::OnExternalWall()
{
	const auto Hour = UWeatherSystem::GetInstance()->GetDynamicSky()->GetCurrentHour();

	if (Hour > 18 || Hour < 8)
	{
		for (auto Iter : RectLightComponentAry)
		{
			// Iter->SetHiddenInGame(false	);
		}
		return;
	}
	for (auto Iter : RectLightComponentAry)
	{
		Iter->SetHiddenInGame(true);
	}
}
