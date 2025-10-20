#include "SceneElement_RadarMode.h"

#include "Marks/PersonMark.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"

#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
#include "FloorHelper.h"
#include "MessageBody.h"
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
	StaticMeshComponent->SetupAttachment(RootComponent);
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
		const auto FloorBox = BelongFloor->BoxComponentPtr->CalcBounds(BelongFloor->BoxComponentPtr->GetComponentToWorld());
		const auto Offset = FloorBox.GetBox().GetExtent().Z;
		for (int i = 0; i < 5; ++i)
		{
		
			const auto Pt = FMath::RandPointInBox(
												  FBox(
													   FVector((Deepth * 2), -(Deepth * 8), 0),
													   FVector((Deepth * 8),-(Deepth * 2),  0)
													  )
												 );
			Pts.Add(Pt);
		}
		
		// UpdatePositions( Pts);
	
	}
	
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall)
		)
		{
			QuitAllState();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
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
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
		)
		{
			EntryShowevice();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode)
		)
		{
			QuitAllState();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor)
		)
		{
			EntryShowevice();

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
			EntryShowevice();

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

	SweepActor->SetActorHiddenInGame(true);
}

void ASceneElement_RadarMode::EntryShowevice()
{
	Super::EntryShowevice();

	SetActorHiddenInGame(false);

	SweepActor->SetActorHiddenInGame(true);
}

void ASceneElement_RadarMode::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();

	SetActorHiddenInGame(false);

	SweepActor->SetActorHiddenInGame(false);
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
		if (Iter)
		{
			Iter->Destroy();
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

			SweepActor = GetWorld()->SpawnActor<AStaticMeshActor>(GetActorLocation(), GetActorRotation());
			SweepActor->SetMobility(EComponentMobility::Movable);
			SweepActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SweepActor->GetStaticMeshComponent()->SetStaticMesh(SweepMesh.LoadSynchronous());
			const auto MatsNum = SweepActor->GetStaticMeshComponent()->GetNumMaterials();
			for (int32 Index = 0; Index < MatsNum; Index++)
			{
				SweepActor->GetStaticMeshComponent()->SetMaterial(Index, SweepMatInst.LoadSynchronous());
			}

			const auto Scale = Deepth / 10;
			SweepActor->GetStaticMeshComponent()->SetRelativeScale3D(FVector(Scale, Scale, 1));
		}
	}
}

void ASceneElement_RadarMode::UpdatePositions(
	const TArray<FVector>& Pts
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
			if (Iter.Value->GameplayTagContainer.HasTag(AreaDecoratorSPtr->GetCurrentInteraction_Area()))
			{
				const auto FloorLocation = Iter.Value->GetActorLocation();

				auto Marks = MakeShared<TSet<APersonMark*>>();

				for (int32 Index = 0; Index < Pts.Num(); Index++)
				{
					auto Pt = Pts[Index];

					Pt.Z = -Offset + 50;
					
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

				for (int32 Index = GeneratedMarkers.Num() - 1; Index >= Pts.Num(); Index--)
				{
					GeneratedMarkers[Index]->Destroy();
					GeneratedMarkers.RemoveAt(Index);
				}
				
				return;
			}
		}
	}
}
