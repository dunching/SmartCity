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

FBox ASceneElement_RadarSweep::GetComponentsBoundingBox(
	bool bNonColliding,
	bool bIncludeFromChildActors
	) const
{
	FBox Box(ForceInit);

	ForEachComponent<UPrimitiveComponent>(
	                                      bIncludeFromChildActors,
	                                      [&](
	                                      const UPrimitiveComponent* InPrimComp
	                                      )
	                                      {
		                                      if (InPrimComp == SweepEffectStaticMeshComponent)
		                                      {
			                                      return;
		                                      }

		                                      // Only use collidable components to find collision bounding box.
		                                      if (InPrimComp->IsRegistered() && (
			                                          bNonColliding || InPrimComp->IsCollisionEnabled()))
		                                      {
			                                      Box += InPrimComp->Bounds.GetBox();
		                                      }
	                                      }
	                                     );

	return Box;
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

			QuitQuery();

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception.GetTag());

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
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Interaction.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception.GetTag());

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
#if TEST_RADAR
	RadarQuery();
#else
	GetWorld()->GetTimerManager().SetTimer(
	                                       QueryTimerHadnle,
	                                       std::bind(&ThisClass::RadarQuery, this),
	                                       UGameOptions::GetInstance()->RadarQueryFrequency,
	                                       true,
	                                       0
	                                      );
#endif
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
	int32 Num = FMath::RandRange(1, 5);

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
				const auto FloorLocation = Iter.Value->GetActorLocation();

				for (int32 Index = 0; Index < Num; Index++)
				{
					const auto Pt = FMath::RandPointInBox(
					                                      FBox(
					                                           FVector(0, -250, 0),
					                                           FVector(800, 250, 0)
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
						NewMarkPtr->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
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
