#include "Building_Pillar.h"

#include "AssetRefMap.h"
#include "Engine/StaticMeshActor.h"

#include "CollisionDataStruct.h"
#include "SmartCitySuiteTags.h"

ABuilding_Pillar::ABuilding_Pillar(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComponent->SetCollisionObjectType(Wall_Object);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void ABuilding_Pillar::ReplaceImp(
	AActor* ActorPtr
	)
{
	Super::ReplaceImp(ActorPtr);

	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			// StaticMeshComponent->SetRelativeTransform(STPtr->GetStaticMeshComponent()->GetRelativeTransform());

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
				Iter->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				Iter->SetCollisionObjectType(Wall_Object);
				Iter->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

				Iter->SetCollisionResponseToChannel(ExternalWall_Object, ECollisionResponse::ECR_Overlap);
				Iter->SetCollisionResponseToChannel(Floor_Object, ECollisionResponse::ECR_Overlap);
				Iter->SetCollisionResponseToChannel(Space_Object, ECollisionResponse::ECR_Overlap);

				Iter->SetRenderCustomDepth(false);

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

void ABuilding_Pillar::SwitchInteractionType(
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

void ABuilding_Pillar::SwitchState(
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
