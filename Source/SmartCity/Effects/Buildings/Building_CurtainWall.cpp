#include "Building_CurtainWall.h"

#include "AssetRefMap.h"
#include "Engine/StaticMeshActor.h"

#include "CollisionDataStruct.h"
#include "SmartCitySuiteTags.h"

ABuilding_CurtainWall::ABuilding_CurtainWall(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ABuilding_CurtainWall::ReplaceImp(
	AActor* ActorPtr
	)
{
	Super::ReplaceImp(ActorPtr);

	if (ActorPtr && ActorPtr->IsA(AActor::StaticClass()))
	{
		TArray<UStaticMeshComponent*> Components;
		ActorPtr->GetComponents<UStaticMeshComponent>(Components);
		for (auto Iter : Components)
		{
			if (Iter)
			{
				auto NewComponentPtr = Cast<UStaticMeshComponent>(
																  AddComponentByClass(
																	   UStaticMeshComponent::StaticClass(),
																	   true,
																	   Iter->
																			  GetComponentTransform(),
																	   false
																	  )
																 );
				NewComponentPtr->SetStaticMesh(Iter->GetStaticMesh());
				NewComponentPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

				NewComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				NewComponentPtr->SetCollisionObjectType(Wall_Object);
				NewComponentPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

				NewComponentPtr->SetCollisionResponseToChannel(ExternalWall_Object, ECollisionResponse::ECR_Overlap);
				NewComponentPtr->SetCollisionResponseToChannel(Floor_Object, ECollisionResponse::ECR_Overlap);
				NewComponentPtr->SetCollisionResponseToChannel(Space_Object, ECollisionResponse::ECR_Overlap);

				NewComponentPtr->SetRenderCustomDepth(false);

				StaticMeshComponentsAry.Add(NewComponentPtr);
				break;
			}
		}

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

void ABuilding_CurtainWall::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SwitchState(EState::kOriginal);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SwitchState(EState::kOriginal);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_EnergyManagement);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SwitchState(EState::kTranslucent);
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SwitchState(EState::kHiden);

			return;
		}
	}
}

void ABuilding_CurtainWall::SwitchState(
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
	case EState::kTranslucent:
		{
			SetActorHiddenInGame(false);

			TArray<UStaticMeshComponent*> Components;
			GetComponents<UStaticMeshComponent>(Components);

			auto WallTranslucentMatInst = UAssetRefMap::GetInstance()->WallTranslucentMatInst.LoadSynchronous();
			for (auto Iter : Components)
			{
				if (Iter)
				{
					const auto MatNum = Iter->GetMaterials().Num();
					for (int32 Index = 0; Index < MatNum; Index++)
					{
						Iter->SetMaterial(Index, WallTranslucentMatInst);
					}
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
