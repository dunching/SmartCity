#include "SceneElement_Space.h"

#include "Engine/OverlapResult.h"
#include "Engine/StaticMeshActor.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
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
	}
}

void ASceneElement_Space::SwitchFocusState(
	bool bIsFocus
	)
{
	Super::SwitchFocusState(bIsFocus);

	if (bIsFocus)
	{
		auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
		if (PrimitiveComponentPtr)
		{
			PrimitiveComponentPtr->SetRenderCustomDepth(true);
			PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
		}

		TArray<FOverlapResult> OutOverlap;

		FComponentQueryParams Params;

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(Device_Object);

		GetWorld()->ComponentOverlapMulti(
		                                  OutOverlap,
		                                  StaticMeshComponent,
		                                  StaticMeshComponent->GetComponentLocation(),
		                                  StaticMeshComponent->GetComponentRotation(),
		                                  Params,
		                                  ObjectQueryParams
		                                 );

		auto InteractionModeDecoratorSPtr = USceneInteractionWorldSystem::GetInstance()->GetInteractionModeDecorator();
		if (InteractionModeDecoratorSPtr)
		{
			if (InteractionModeDecoratorSPtr->GetBranchDecoratorType() == UGameplayTagsLibrary::Interaction_Mode_QD)
			{
				 FString FeatureName;
				 TArray<FString> Features;
				for (const auto& Iter : OutOverlap)
				{
					if (Iter.GetActor())
					{
						if (Iter.GetActor()->IsA(ASceneElement_DeviceBase::StaticClass()))
						{
							auto SceneElementPtr = Cast<ASceneElement_DeviceBase>(Iter.GetActor());
							if (SceneElementPtr)
							{
								if (SceneElementPtr->DeviceType.MatchesTag(UGameplayTagsLibrary::SceneElement_FanCoil))
								{
									Features.Add(SceneElementPtr->GetName());
								}
							}
						}
					}
				}
				
				auto HUDPtr = Cast<AMainHUD>(GEngine->GetFirstLocalPlayerController(GetWorldImp())->GetHUD());
				if (HUDPtr)
				{
					HUDPtr->GetMainHUDLayout()->InitalFeaturesItem(FeatureName, Features);
				}
			}
			else if (InteractionModeDecoratorSPtr->GetBranchDecoratorType() ==
			         UGameplayTagsLibrary::Interaction_Mode_Lighting)
			{
			}
		}
	}
	else
	{
		auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
		if (PrimitiveComponentPtr)
		{
			PrimitiveComponentPtr->SetRenderCustomDepth(false);
		}
	}
}
