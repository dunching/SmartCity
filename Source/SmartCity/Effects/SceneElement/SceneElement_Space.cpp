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
#include "SceneElement_DeviceBase.h"
#include "SceneInteractionWorldSystem.h"

ASceneElement_Space::ASceneElement_Space(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void ASceneElement_Space::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneElement_Space::ReplaceImp(
	AActor* ActorPtr
	)
{
	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			StaticMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());
		}

		auto SpaceMaterialInstance = UAssetRefMap::GetInstance()->SpaceMaterialInstance;

		TArray<UStaticMeshComponent*> Components;
		GetComponents<UStaticMeshComponent>(Components);
		for (auto Iter : Components)
		{
			if (Iter)
			{
				Iter->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				Iter->SetCollisionObjectType(Space_Object);
				Iter->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

				Iter->SetCollisionResponseToChannel(ExternalWall_Object, ECollisionResponse::ECR_Overlap);
				Iter->SetCollisionResponseToChannel(Floor_Object, ECollisionResponse::ECR_Overlap);
				Iter->SetCollisionResponseToChannel(Device_Object, ECollisionResponse::ECR_Overlap);

				Iter->SetRenderCustomDepth(false);

				for (int32 Index = 0; Index < Iter->GetNumMaterials(); Index++)
				{
					Iter->SetMaterial(Index, SpaceMaterialInstance.LoadSynchronous());
				}

				Iter->SetCastShadow(false);
				Iter->bVisibleInReflectionCaptures = false;
				Iter->bVisibleInRealTimeSkyCaptures = false;
				Iter->bVisibleInRayTracing = false;
				Iter->bReceivesDecals = false;
				Iter->bUseAsOccluder = false;

				break;
			}
		}

		ActorPtr->GetComponents<UStaticMeshComponent>(Components);
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
}

void ASceneElement_Space::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
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

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_ExternalWall);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(true);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);
		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Mode_Focus);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(false);

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

			GetWorld()->ComponentOverlapMulti(
			                                  OutOverlap,
			                                  StaticMeshComponent,
			                                  FVector::ZeroVector,
			                                  FRotator::ZeroRotator,
			                                  // StaticMeshComponent->GetComponentLocation(),
			                                  // StaticMeshComponent->GetComponentRotation(),
			                                  Params,
			                                  ObjectQueryParams
			                                 );

			auto InteractionModeDecoratorSPtr = USceneInteractionWorldSystem::GetInstance()->
				GetInteractionModeDecorator();
			if (InteractionModeDecoratorSPtr)
			{
				if (InteractionModeDecoratorSPtr->GetBranchDecoratorType() ==
				    UGameplayTagsLibrary::Interaction_Mode_PWR)
				{
					FString FeatureName = Category;

					if (UAssetRefMap::GetInstance()->ModeDescription.Contains(
					                                                          InteractionModeDecoratorSPtr->
					                                                          GetBranchDecoratorType()
					                                                         ))
					{
						const auto Description = UAssetRefMap::GetInstance()->ModeDescription[
							InteractionModeDecoratorSPtr->
							GetBranchDecoratorType()];
						FeatureName.Append(TEXT(":"));
						FeatureName.Append(Description.Title);
					}

					TSet<AActor*> ActorsAry;
					for (const auto& Iter : OutOverlap)
					{
						if (Iter.GetActor())
						{
							ActorsAry.Add(Iter.GetActor());
						}
					}

					TArray<FFeaturesItem> Features;
					for (const auto& Iter : ActorsAry)
					{
						if (Iter)
						{
							if (Iter->IsA(ASceneElement_DeviceBase::StaticClass()))
							{
								auto SceneElementPtr = Cast<ASceneElement_DeviceBase>(Iter);
								if (SceneElementPtr)
								{
									if (SceneElementPtr->DeviceType.MatchesTag(
									                                           UGameplayTagsLibrary::SceneElement_FanCoil
									                                          ))
									{
										FFeaturesItem FeaturesItem;

										FeaturesItem.Text = SceneElementPtr->SceneElementName;
										FeaturesItem.SceneElementPtr = SceneElementPtr;

										Features.Add(FeaturesItem);
									}
								}
							}
						}
					}

					auto HUDPtr = Cast<AMainHUD>(GEngine->GetFirstLocalPlayerController(GetWorldImp())->GetHUD());
					if (HUDPtr)
					{
						HUDPtr->GetMainHUDLayout()->InitalFeaturesItem(this, FeatureName, Features);
					}
				}
				else if (InteractionModeDecoratorSPtr->GetBranchDecoratorType() ==
				         UGameplayTagsLibrary::Interaction_Mode_PWR_Lighting)
				{
				}
			}

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(false);

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
			return;
		}
	}
}
