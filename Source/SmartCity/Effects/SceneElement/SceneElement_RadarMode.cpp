#include "SceneElement_RadarMode.h"

#include "Marks/PersonMark.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"

#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
#include "FloorHelper.h"
#include "MessageBody.h"
#include "SceneElement_RadarSweep.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionDecorator_Area.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "WebChannelWorldSystem.h"

ASceneElement_RadarMode::ASceneElement_RadarMode(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RelativeTransformComponent);

	StaticMeshComponent->SetRelativeRotation(FRotator(0, 90, 0));
}

void ASceneElement_RadarMode::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);
}

void ASceneElement_RadarMode::BeginPlay()
{
	Super::BeginPlay();

	// const auto Interval = Deepth / MeshNum;
	// for (int32 Index = 0; Index < MeshNum; Index++)
	// {
	// 	FTransform Transform;
	//
	// 	const auto Distance = Index * Interval;
	// 	FVector Location(Distance);
	// 	Transform.SetTranslation(Location);
	//
	// 	auto NewComponentPtr = Cast<UStaticMeshComponent>(
	// 	                                                  AddComponentByClass(
	// 	                                                                      UStaticMeshComponent::StaticClass(),
	// 	                                                                      true,
	// 	                                                                      Transform,
	// 	                                                                      false
	// 	                                                                     )
	// 	                                                 );
	//
	// 	NewComponentPtr->SetStaticMesh(Mesh.LoadSynchronous());
	// 	NewComponentPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	//
	// 	NewComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//
	// 	NewComponentPtr->SetRenderCustomDepth(false);
	//
	// 	MeshAry.Add({NewComponentPtr, Distance});
	// }
}

void ASceneElement_RadarMode::Tick(
	float DeltaTime
	)
{
	Super::Tick(DeltaTime);
}

FBox ASceneElement_RadarMode::GetComponentsBoundingBox(
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

void ASceneElement_RadarMode::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	TArray<FVector> Pts;

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );
	if (AreaDecoratorSPtr)
	{
		const auto FloorBox = BelongFloor->BoxComponentPtr->CalcBounds(
		                                                               BelongFloor->BoxComponentPtr->
		                                                               GetComponentToWorld()
		                                                              );
		const auto Offset = FloorBox.GetBox().GetExtent().Z;
		for (int i = 0; i < 5; ++i)
		{
			const auto Pt = FMath::RandPointInBox(
			                                      FBox(
			                                           FVector((Deepth * 2), -(Deepth * 8), 0),
			                                           FVector((Deepth * 8), -(Deepth * 2), 0)
			                                          )
			                                     );
			Pts.Add(Pt);
		}

		// UpdatePositions( Pts);
	}

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall) ||
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_Periphery)
		)
		{
			QuitAllState();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_BatchControl)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Interaction)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
		)
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode)
		)
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if ((ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)))
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_View)
		)
		{
			EntryViewDevice();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_Focus)
		)
		{
			EntryShowDevice();

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

void ASceneElement_RadarMode::EntryFocusDevice()
{
	Super::EntryFocusDevice();

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(true);
		StaticMeshComponent->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	}

	auto MessageBodySPtr = MakeShared<FMessageBody_ViewDevice>();

	MessageBodySPtr->DeviceID = SceneElementID;

	UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);
}

void ASceneElement_RadarMode::EntryViewDevice()
{
	Super::EntryViewDevice();

	SetActorHiddenInGame(false);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(true);
		StaticMeshComponent->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	}

	SweepActor->SetActorHiddenInGame(false);

	SweepActor->StaticMeshComponent->SetRenderCustomDepth(true);
	SweepActor->StaticMeshComponent->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
}

void ASceneElement_RadarMode::EntryShowDevice()
{
	Super::EntryShowDevice();

	SetActorHiddenInGame(false);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(false);
	}

	SweepActor->SetActorHiddenInGame(true);
	SweepActor->StaticMeshComponent->SetRenderCustomDepth(false);

	for (auto Iter : GeneratedMarkers)
	{
		if (Iter.Value)
		{
			Iter.Value->Destroy();
		}
	}
	GeneratedMarkers.Empty();
}

void ASceneElement_RadarMode::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();

	SetActorHiddenInGame(false);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(false);
	}

	SweepActor->SetActorHiddenInGame(false);
	SweepActor->StaticMeshComponent->SetRenderCustomDepth(false);
}

void ASceneElement_RadarMode::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(true);

	SweepActor->SetActorHiddenInGame(true);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(false);
	}

	for (auto Iter : GeneratedMarkers)
	{
		if (Iter.Value)
		{
			Iter.Value->Destroy();
		}
	}
	GeneratedMarkers.Empty();
}

void ASceneElement_RadarMode::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);

	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			auto InterfacePtr = Cast<IInterface_AssetUserData>(STPtr->GetStaticMeshComponent());
			if (!InterfacePtr)
			{
				return;
			}
			auto AUDPtr = Cast<UDatasmithAssetUserData>(
			                                            InterfacePtr->GetAssetUserDataOfClass(
				                                             UDatasmithAssetUserData::StaticClass()
				                                            )
			                                           );

			CheckIsJiaCeng(AUDPtr);

			StaticMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());

			for (int32 Index = 0; Index < STPtr->GetStaticMeshComponent()->GetNumMaterials(); Index++)
			{
				StaticMeshComponent->SetMaterial(Index, STPtr->GetStaticMeshComponent()->GetMaterial(Index));
			}

			UpdateCollisionBox({StaticMeshComponent});

			SweepActor = GetWorld()->SpawnActor<ASceneElement_RadarSweep>(GetActorLocation(), GetActorRotation());
			SweepActor->StaticMeshComponent->SetStaticMesh(SweepMesh.LoadSynchronous());
			SweepActor->StaticMeshComponent->SetRelativeRotation(FRotator(0, 180, 0));
			const auto MatsNum = SweepActor->StaticMeshComponent->GetNumMaterials();
			for (int32 Index = 0; Index < MatsNum; Index++)
			{
				SweepActor->StaticMeshComponent->SetMaterial(Index, SweepMatInst.LoadSynchronous());
			}

			const auto Scale = Deepth / 10;
			SweepActor->StaticMeshComponent->SetRelativeScale3D(FVector(Scale, Scale, 1));

			SweepActor->StaticMeshComponent->SetCastShadow(false);
			SweepActor->StaticMeshComponent->SetReceivesDecals(false);
		}
	}
}

void ASceneElement_RadarMode::UpdateReletiveTransform(
	const FTransform& NewRelativeTransform
	)
{
	Super::UpdateReletiveTransform(NewRelativeTransform);

	SweepActor->SetActorLocation(RelativeTransformComponent->GetComponentLocation());

	auto Rot = RelativeTransformComponent->GetComponentRotation();

	Rot.Pitch = 0;
	Rot.Roll = 0;

	SweepActor->SetActorRotation(Rot);
}

void ASceneElement_RadarMode::UpdatePositions(
	const TMap<FString, FVector>& Pts
	)
{
	{
		if (
			CurrentConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			CurrentConditionalSet.ConditionalSet.HasTagExact(
			                                                 USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar
			                                                )
		)
		{
		}
		else if (
			CurrentConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			CurrentConditionalSet.ConditionalSet.HasTagExact(
			                                                 USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception
			                                                )
		)
		{
		}
		else if (
			CurrentConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_View) 
		)
		{
		}
		else
		{
			return;
		}
	}

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );
	if (AreaDecoratorSPtr)
	{
		const auto FloorBox = BelongFloor->BoxComponentPtr->CalcBounds(
		                                                               BelongFloor->BoxComponentPtr->
		                                                               GetComponentToWorld()
		                                                              );
		const auto Offset = FloorBox.GetBox().GetExtent().Z;

		for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			if (
			Iter.Value->GameplayTagContainer.HasTag(AreaDecoratorSPtr->GetCurrentInteraction_Area())||
			Iter.Value->GameplayTagContainer.HasTag(BelongFloor->FloorTag)
			)
			{
				const auto FloorLocation = Iter.Value->GetActorLocation();

				auto Marks = MakeShared<TSet<APersonMark*>>();


				for (const auto& PtIter : Pts)
				{
					auto Pt = PtIter.Value;

					Pt.Z = -Offset + 80;

					if (GeneratedMarkers.Contains(PtIter.Key))
					{
						GeneratedMarkers[PtIter.Key]->Update(Pt);

						GeneratedMarkers[PtIter.Key]->Marks = Marks;

						Marks->Add(GeneratedMarkers[PtIter.Key]);
					}
					else
					{
						auto NewMarkPtr = GetWorldImp()->SpawnActor<APersonMark>(
							 UAssetRefMap::GetInstance()->PersonMarkClass
							);
						NewMarkPtr->AttachToComponent(
						                              RelativeTransformComponent,
						                              FAttachmentTransformRules::KeepRelativeTransform
						                             );
						NewMarkPtr->Update(Pt);

						NewMarkPtr->Marks = Marks;

						Marks->Add(NewMarkPtr);

						GeneratedMarkers.Add(PtIter.Key, NewMarkPtr);
					}
				}

				for (auto PtIter = GeneratedMarkers.CreateIterator(); PtIter; ++PtIter)
				{
					if (Pts.Contains(PtIter->Key))
					{
					}
					else
					{
						PtIter->Value->Destroy();
						PtIter.RemoveCurrent();
					}
				}

				return;
			}
		}
	}
}
