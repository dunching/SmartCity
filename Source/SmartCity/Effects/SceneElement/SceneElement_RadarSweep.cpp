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
#include "SceneInteractionDecorator_Area.h"

ASceneElement_RadarSweep::ASceneElement_RadarSweep(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RelativeTransformComponent);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASceneElement_RadarSweep::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);

	const auto Scale = Deepth / 10;
	StaticMeshComponent->SetRelativeScale3D(FVector(Scale, Scale, 1));
}

void ASceneElement_RadarSweep::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneElement_RadarSweep::Tick(
	float DeltaTime
	)
{
	Super::Tick(DeltaTime);

	UpdateMeshEffect();
}

FBox ASceneElement_RadarSweep::GetComponentsBoundingBox(
	bool bNonColliding,
	bool bIncludeFromChildActors
	) const
{
	FBox Box(ForceInit);

	return Box;
}

void ASceneElement_RadarSweep::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	// Super::SwitchInteractionType(ConditionalSet);

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
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar)
			)
		{
			SetActorHiddenInGame(false);


			QuitQuery();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
			)
		{
			SetActorHiddenInGame(false);

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception)
			)
		{
			SetActorHiddenInGame(false);


			EntryQuery();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Interaction) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception)
			)
		{
			SetActorHiddenInGame(false);


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
			SetActorHiddenInGame(true);

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
	                                       QueryTimerHandle,
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
	                                         QueryTimerHandle
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
	// int32 Num = FMath::RandRange(1, 5);
	int32 Num = 5;

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );
	if (AreaDecoratorSPtr)
	{
		const auto FloorBox = BelongFloor->BoxComponentPtr->CalcBounds(BelongFloor->BoxComponentPtr->GetComponentToWorld());
		const auto Offset = FloorBox.GetBox().GetExtent().Z;
		
		for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			if (Iter.Value->GameplayTagContainer.HasTag(AreaDecoratorSPtr->GetCurrentInteraction_Area()))
			{
				const auto FloorLocation = Iter.Value->GetActorLocation();

				auto Marks = MakeShared<TSet<APersonMark*>>();
	
				for (int32 Index = 0; Index < Num; Index++)
				{
					const auto Pt = FMath::RandPointInBox(
					                                      FBox(
					                                           FVector(-(Deepth * 10), -(Deepth * 10), -Offset),
					                                           FVector(0, 0, -Offset)
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
						NewMarkPtr->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
						NewMarkPtr->Update(Pt);
						
						NewMarkPtr->Marks = Marks;

						Marks->Add(NewMarkPtr);
						
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

void ASceneElement_RadarSweep::UpdateMeshEffect()
{
	// for (auto& Iter : MeshAry)
	// {
	// 	Iter.Value += Speed;
	// }
}
