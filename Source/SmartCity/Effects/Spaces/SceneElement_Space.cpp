#include "SceneElement_Space.h"

#include "Algorithm.h"
#include "Engine/OverlapResult.h"
#include "Engine/StaticMeshActor.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "DatasmithAssetUserData.h"
#include "FeatureWheel.h"
#include "GameplayTagsLibrary.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "MessageBody.h"
#include "RouteMarker.h"
#include "SceneElement_DeviceBase.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "WebChannelWorldSystem.h"

ASceneElement_Space::ASceneElement_Space(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void ASceneElement_Space::BeginPlay()
{
	Super::BeginPlay();

	
}

void ASceneElement_Space::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
}

void ASceneElement_Space::Merge(
	const TSoftObjectPtr<AActor>& ActorRef,
	const TPair<FName, FString>& InUserData
	)
{
	Super::Merge(ActorRef, InUserData);

	if (ActorRef.ToSoftObjectPath().IsValid())
	{
		AActor* ParentPtr = ActorRef->GetAttachParentActor();
		if (ParentPtr && !GetAttachParentActor())
		{
			AttachToActor(ParentPtr, FAttachmentTransformRules::KeepRelativeTransform);
			SetActorRelativeTransform(FTransform::Identity);
		}

		auto STPtr = Cast<AStaticMeshActor>(ActorRef.Get());
		if (STPtr)
		{
			auto NewComponentPtr = Cast<UStaticMeshComponent>(
			                                                  AddComponentByClass(
				                                                   UStaticMeshComponent::StaticClass(),
				                                                   true,
				                                                   STPtr->GetStaticMeshComponent()->
				                                                          GetComponentTransform(),
				                                                   false
				                                                  )
			                                                 );

			NewComponentPtr->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());
			NewComponentPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

			NewComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			NewComponentPtr->SetCollisionObjectType(Space_Object);
			NewComponentPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

			NewComponentPtr->SetCollisionResponseToChannel(ExternalWall_Object, ECollisionResponse::ECR_Overlap);
			NewComponentPtr->SetCollisionResponseToChannel(Floor_Object, ECollisionResponse::ECR_Overlap);
			NewComponentPtr->SetCollisionResponseToChannel(Device_Object, ECollisionResponse::ECR_Overlap);

			NewComponentPtr->SetRenderCustomDepth(false);

			auto SpaceMaterialInstance = UAssetRefMap::GetInstance()->SpaceMaterialInstance;

			auto MatInst  = UMaterialInstanceDynamic::Create(SpaceMaterialInstance.LoadSynchronous(), this);
			
			for (int32 Index = 0; Index < NewComponentPtr->GetNumMaterials(); Index++)
			{
				NewComponentPtr->SetMaterial(Index, MatInst);
			}

			NewComponentPtr->SetCastShadow(false);
			NewComponentPtr->bVisibleInReflectionCaptures = false;
			NewComponentPtr->bVisibleInRealTimeSkyCaptures = false;
			NewComponentPtr->bVisibleInRayTracing = false;
			NewComponentPtr->bReceivesDecals = false;
			NewComponentPtr->bUseAsOccluder = false;

			StaticMeshComponentsAry.Add(NewComponentPtr);

			TArray<UStaticMeshComponent*> Components;
			STPtr->GetComponents<UStaticMeshComponent>(Components);
			for (auto SecondIter : Components)
			{
				auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
				if (InterfacePtr)
				{
					auto AUDPtr = Cast<UDatasmithAssetUserData>(
					                                            InterfacePtr->GetAssetUserDataOfClass(
						                                             UDatasmithAssetUserData::StaticClass()
						                                            )
					                                           );
					if (!AUDPtr)
					{
						continue;
					}
					for (const auto& ThirdIter : AUDPtr->MetaData)
					{
						if (ThirdIter.Key == DataSmith_Key)
						{
							Category = ThirdIter.Value;
							break;
						}
					}
					break;
				}
			}
		}

		ActorRef->Destroy();
	}
}

void ASceneElement_Space::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(true);

			if (RouteMarkerPtr)
			{
				RouteMarkerPtr->RemoveFromParent();
				RouteMarkerPtr = nullptr;
			}

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_Focus);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);
			
			SwitchColor(FColor::Red);
			
			auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetRenderCustomDepth(true);
				PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
			}

			TArray<FOverlapResult> OutOverlap;

			FComponentQueryParams Params;

			// Params.bTraceComplex = true;

#if !(UE_BUILD_TEST || UE_BUILD_SHIPPING)
			Params.bDebugQuery = true;
#endif

			FCollisionObjectQueryParams ObjectQueryParams;
			ObjectQueryParams.AddObjectTypesToQuery(Device_Object);

			auto MessageBodySPtr = MakeShared<FMessageBody_SelectedSpace>();

			MessageBodySPtr->SpaceName = Category;
			for (auto MeshIter : StaticMeshComponentsAry)
			{
				GetWorld()->ComponentOverlapMulti(
				                                  OutOverlap,
				                                  MeshIter,
				                                  FVector::ZeroVector,
				                                  FRotator::ZeroRotator,
				                                  // StaticMeshComponent->GetComponentLocation(),
				                                  // StaticMeshComponent->GetComponentRotation(),
				                                  Params,
				                                  ObjectQueryParams
				                                 );

				TSet<ASceneElement_DeviceBase*> ActorsAry;
				for (const auto& Iter : OutOverlap)
				{
					if (Iter.GetActor() && !Iter.GetActor()->IsHidden())
					{
						ActorsAry.Add(Cast<ASceneElement_DeviceBase>(Iter.GetActor()));
					}
				}

				for (const auto& Iter : ActorsAry)
				{
					auto SceneElementPtr = Cast<ASceneElement_DeviceBase>(Iter);
					if (SceneElementPtr)
					{
						SceneElementPtr->SwitchInteractionType(ConditionalSet);
					}
					else
					{
					}
				}

				for (auto DeviceIter : ActorsAry)
				{
					if (DeviceIter)
					{
						MessageBodySPtr->DeviceIDAry.Add(DeviceIter->DeviceID);
					}
				}
			}

			UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);

			auto HUDPtr = Cast<AMainHUD>(
			                             GEngine->GetFirstLocalPlayerController(GetWorldImp())->
			                                      GetHUD()
			                            );
			if (HUDPtr)
			{
				HUDPtr->GetMainHUDLayout()->RemoveFeatures();

				TArray<FFeaturesItem> Features;
				for (const auto& Iter : FeaturesAry)
				{
					Features.Add({Iter, nullptr});
				}

				HUDPtr->GetMainHUDLayout()->InitalFeaturesItem(this, Category, Features);
			}
			if (RouteMarkerPtr)
			{
				RouteMarkerPtr->TextStr = Category;
				RouteMarkerPtr->TargetActor = this;
				RouteMarkerPtr->AddToViewport();
			}

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			// 确认当前的模式
			auto DecoratorSPtr =
				DynamicCastSharedPtr<FInteraction_Decorator>(
				                                             USceneInteractionWorldSystem::GetInstance()->
				                                             GetDecorator(
				                                                          USmartCitySuiteTags::Interaction_Interaction
				                                                         )
				                                            );
			if (DecoratorSPtr)
			{
				switch (DecoratorSPtr->GetInteractionType())
				{
				case FInteraction_Decorator::EInteractionType::kDevice:
					{
						SetActorHiddenInGame(true);

						if (RouteMarkerPtr)
						{
							RouteMarkerPtr->RemoveFromParent();
							RouteMarkerPtr = nullptr;
						}
					}
					break;
				case FInteraction_Decorator::EInteractionType::kSpace:
					{
						SetActorHiddenInGame(false);

						SwitchColor(FColor::White);
			
						auto HUDPtr = Cast<AMainHUD>(GEngine->GetFirstLocalPlayerController(GetWorldImp())->GetHUD());
						if (HUDPtr)
						{
							HUDPtr->GetMainHUDLayout()->RemoveFeatures();
						}

						auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
						if (PrimitiveComponentPtr)
						{
							PrimitiveComponentPtr->SetRenderCustomDepth(false);
						}

						if (RouteMarkerPtr)
						{
							RouteMarkerPtr->RemoveFromParent();
							RouteMarkerPtr = nullptr;
						}
					}
					break;
				}
			}

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			// 确认当前的模式
			auto DecoratorSPtr =
				DynamicCastSharedPtr<FInteraction_Decorator>(
				                                             USceneInteractionWorldSystem::GetInstance()->
				                                             GetDecorator(
				                                                          USmartCitySuiteTags::Interaction_Interaction
				                                                         )
				                                            );
			if (DecoratorSPtr)
			{
				switch (DecoratorSPtr->GetInteractionType())
				{
				case FInteraction_Decorator::EInteractionType::kDevice:
					{
						SetActorHiddenInGame(true);

						if (RouteMarkerPtr)
						{
							RouteMarkerPtr->RemoveFromParent();
							RouteMarkerPtr = nullptr;
						}
					}
					break;
				case FInteraction_Decorator::EInteractionType::kSpace:
					{
						SetActorHiddenInGame(false);

						SwitchColor(FColor::White);
			
						auto HUDPtr = Cast<AMainHUD>(GEngine->GetFirstLocalPlayerController(GetWorldImp())->GetHUD());
						if (HUDPtr)
						{
							HUDPtr->GetMainHUDLayout()->RemoveFeatures();
						}

						auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
						if (PrimitiveComponentPtr)
						{
							PrimitiveComponentPtr->SetRenderCustomDepth(false);
						}

						if (RouteMarkerPtr)
						{
							RouteMarkerPtr->RemoveFromParent();
							RouteMarkerPtr = nullptr;
						}
					}
					break;
				}
			}

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Interaction);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			// 确认当前的模式
			auto DecoratorSPtr =
				DynamicCastSharedPtr<FInteraction_Decorator>(
				                                             USceneInteractionWorldSystem::GetInstance()->
				                                             GetDecorator(
				                                                          USmartCitySuiteTags::Interaction_Interaction
				                                                         )
				                                            );
			if (DecoratorSPtr)
			{
				switch (DecoratorSPtr->GetInteractionType())
				{
				case FInteraction_Decorator::EInteractionType::kDevice:
					{
						SetActorHiddenInGame(true);

						if (RouteMarkerPtr)
						{
							RouteMarkerPtr->RemoveFromParent();
							RouteMarkerPtr = nullptr;
						}
					}
					break;
				case FInteraction_Decorator::EInteractionType::kSpace:
					{
						SetActorHiddenInGame(false);

						SwitchColor(FColor::White);
			
						auto HUDPtr = Cast<AMainHUD>(GEngine->GetFirstLocalPlayerController(GetWorldImp())->GetHUD());
						if (HUDPtr)
						{
							HUDPtr->GetMainHUDLayout()->RemoveFeatures();
						}

						auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
						if (PrimitiveComponentPtr)
						{
							PrimitiveComponentPtr->SetRenderCustomDepth(false);
						}

						if (RouteMarkerPtr)
						{
							RouteMarkerPtr->RemoveFromParent();
							RouteMarkerPtr = nullptr;
						}
					}
					break;
				}
			}

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Interaction);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			// 确认当前的模式
			auto DecoratorSPtr =
				DynamicCastSharedPtr<FInteraction_Decorator>(
				                                             USceneInteractionWorldSystem::GetInstance()->
				                                             GetDecorator(
				                                                          USmartCitySuiteTags::Interaction_Interaction
				                                                         )
				                                            );
			if (DecoratorSPtr)
			{
				switch (DecoratorSPtr->GetInteractionType())
				{
				case FInteraction_Decorator::EInteractionType::kDevice:
					{
						SetActorHiddenInGame(true);

						if (RouteMarkerPtr)
						{
							RouteMarkerPtr->RemoveFromParent();
							RouteMarkerPtr = nullptr;
						}
					}
					break;
				case FInteraction_Decorator::EInteractionType::kSpace:
					{
						SetActorHiddenInGame(false);

						SwitchColor(FColor::White);
			
						auto HUDPtr = Cast<AMainHUD>(GEngine->GetFirstLocalPlayerController(GetWorldImp())->GetHUD());
						if (HUDPtr)
						{
							HUDPtr->GetMainHUDLayout()->RemoveFeatures();
						}

						auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
						if (PrimitiveComponentPtr)
						{
							PrimitiveComponentPtr->SetRenderCustomDepth(false);
						}

						if (RouteMarkerPtr)
						{
							RouteMarkerPtr->RemoveFromParent();
							RouteMarkerPtr = nullptr;
						}
					}
					break;
				}
			}

			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}

		SetActorHiddenInGame(true);

		auto HUDPtr = Cast<AMainHUD>(GEngine->GetFirstLocalPlayerController(GetWorldImp())->GetHUD());
		if (HUDPtr)
		{
			HUDPtr->GetMainHUDLayout()->RemoveFeatures();
		}

		auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
		if (PrimitiveComponentPtr)
		{
			PrimitiveComponentPtr->SetRenderCustomDepth(false);
		}

		if (RouteMarkerPtr)
		{
			RouteMarkerPtr->RemoveFromParent();
			RouteMarkerPtr = nullptr;
		}

		return;
	}
}

void ASceneElement_Space::SetFeatures(
	const TArray<FString>& InFeaturesAry
	)
{
	FeaturesAry = InFeaturesAry;
}

void ASceneElement_Space::SwitchColor(
	const FColor& Color
	)
{
	for (auto MeshIter : StaticMeshComponentsAry)
	{
		const auto Mats = MeshIter->GetMaterials();
		for (auto MatIter : Mats)
		{
			auto MatInst =Cast<UMaterialInstanceDynamic>(MatIter);
			if (MatInst)
			{
				MatInst->SetVectorParameterValue(UAssetRefMap::GetInstance()->SpaceMaterialColorName, Color);
			}
		}
	}
}
