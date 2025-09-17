#include "SceneElement_Furniture.h"

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

ASceneElement_Furniture::ASceneElement_Furniture(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASceneElement_Furniture::BeginPlay()
{
	Super::BeginPlay();

	
}

void ASceneElement_Furniture::ReplaceImp(
	AActor* ActorPtr
	)
{
	Super::ReplaceImp(ActorPtr);

	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			StaticMeshComponent->SetWorldTransform(STPtr->GetStaticMeshComponent()->GetComponentTransform());
			StaticMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());
			
			for (int32 Index = 0; Index < STPtr->GetStaticMeshComponent()->GetNumMaterials(); Index++)
			{
				StaticMeshComponent->SetMaterial(Index, STPtr->GetStaticMeshComponent()->GetMaterial(Index));
			}
		}

		TArray<UStaticMeshComponent*> Components;
		GetComponents<UStaticMeshComponent>(Components);
		
		for (auto Iter : Components)
		{
			if (Iter)
			{
				FMaterialAry MaterialAry;
				auto Mats = Iter->GetMaterials();
				for (auto MatIter : Mats)
				{
					MaterialAry.MaterialsAry.Add(MatIter);
				}

				MaterialMap.Add(Iter, MaterialAry);
			}
		}
	}
}

void ASceneElement_Furniture::SwitchInteractionType(
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
				SwitchState(DecoratorSPtr->bShowFurniture ? EState::kOriginal : EState::kHiden);

				return;
			}

			SetActorHiddenInGame(false);

			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}

		SetActorHiddenInGame(true);

		return;
	}
}

void ASceneElement_Furniture::SwitchState(
	EState State
	)
{
	switch (State)
	{
	case EState::kOriginal:
		{
			SetActorHiddenInGame(false);

			TArray<UStaticMeshComponent*> Components;
			GetComponents<UStaticMeshComponent>(Components);

			auto WallTranslucentMatInst = UAssetRefMap::GetInstance()->WallTranslucentMatInst.LoadSynchronous();
			for (auto Iter : Components)
			{
				if (!Iter)
				{
					continue;
				}
				auto MatAry = MaterialMap.Find(Iter);
				if (!MatAry)
				{
					continue;
				}

				const auto MatNum = Iter->GetMaterials().Num();
				const auto OriMatNum = MatAry->MaterialsAry.Num();
				for (int32 Index = 0; Index < MatNum && Index < OriMatNum; Index++)
				{
					Iter->SetMaterial(Index, MatAry->MaterialsAry[Index]);
				}
			}
		}
		break;
	case EState::kHiden:
		{
			SetActorHiddenInGame(true);
		}
		break;
	}
}
