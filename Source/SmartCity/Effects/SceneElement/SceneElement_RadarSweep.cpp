#include "SceneElement_RadarSweep.h"

#include "Components/BoxComponent.h"
#include "Marks/PersonMark.h"

#include "AssetRefMap.h"
#include "FloorHelper.h"
#include "GameplayTagsLibrary.h"
#include "SmartCitySuiteTags.h"
#include "PersonMark.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionWorldSystem.h"
#include "TemplateHelper.h"

ASceneElement_RadarSweep::ASceneElement_RadarSweep(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	AnchorComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnchorComponent"));
	AnchorComponent->SetupAttachment(StaticMeshComponent);

	SweepEffectStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(
		 TEXT("SweepEffectStaticMeshComponent")
		);
	SweepEffectStaticMeshComponent->SetupAttachment(AnchorComponent);
}

void ASceneElement_RadarSweep::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);

	AnchorComponent->SetRelativeScale3D(
	                                    FVector(
	                                            Area / MeshSize.X,
	                                            Area / MeshSize.Y,
	                                            Deepth / MeshSize.Z
	                                           )
	                                   );
}

void ASceneElement_RadarSweep::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(true);
			
			QuitQuery();
			
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			SweepEffectStaticMeshComponent->SetHiddenInGame(false);

			EntryQuery();
			
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			SweepEffectStaticMeshComponent->SetHiddenInGame(true);
			
			QuitQuery();
			
			return;
		}
	}

	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}
		SetActorHiddenInGame(true);
		
		QuitQuery();
		
		return;
	}
}

void ASceneElement_RadarSweep::EntryQuery()
{
	GetWorld()->GetTimerManager().SetTimer(
	                                       QueryTimerHadnle,
	                                       std::bind(&ThisClass::RadarQuery, this),
#if TEST_RADAR
	                                       1.f,
#else
	                                       UGameOptions::GetInstance()->RadarQueryFrequency,
#endif
	                                       true
	                                      );
}

void ASceneElement_RadarSweep::QuitQuery()
{
	GetWorld()->GetTimerManager().ClearTimer(
	                                         QueryTimerHadnle
	                                        );

	for (auto Iter : GeneratedMarkers)
	{
		Iter->Destroy();
	}
	GeneratedMarkers.Empty();
}

void ASceneElement_RadarSweep::RadarQuery()
{
#if TEST_RADAR
	QueryComplete();
#else

#endif
}

void ASceneElement_RadarSweep::QueryComplete()
{
#if TEST_RADAR
	int32 Num = FMath::RandRange(1, 10);

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );
	if (AreaDecoratorSPtr)
	{
		for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			if (Iter.Value->FloorTag == AreaDecoratorSPtr->GetCurrentInteraction_Area())
			{
				for (int32 Index = 0; Index < Num; Index++)
				{
					const auto Pt = FMath::RandPointInBox(
					                                      FBox::BuildAABB(
					                                                      Iter.Value->BoxComponentPtr->
					                                                           GetComponentLocation(),
					                                                      Iter.Value->BoxComponentPtr->
					                                                           GetScaledBoxExtent()
					                                                     )
					                                     );
					if (GeneratedMarkers.IsValidIndex(Index))
					{
						GeneratedMarkers[Index]->Update(Pt);
					}
					else
					{
						auto NewMarkPtr = GetWorldImp()->SpawnActor<APersonMark>(
							 UAssetRefMap::GetInstance()->PersonMarkClass
							);
						NewMarkPtr->Update(Pt);

						GeneratedMarkers.Add(NewMarkPtr);
					}
				}

				break;
			}
		}
		for (int32 Index = GeneratedMarkers.Num() - 1; Index >= Num; Index--)
		{
			GeneratedMarkers[Index]->Destroy();
			GeneratedMarkers.RemoveAt(Index);
		}
	}
#else

#endif
}
