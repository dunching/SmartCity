#include "Building_ItemBase.h"

#include "AssetRefMap.h"
#include "Engine/StaticMeshActor.h"

#include "CollisionDataStruct.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"

ABuilding_ItemBase::ABuilding_ItemBase(
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

void ABuilding_ItemBase::ReplaceImp(
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
	}
}

void ABuilding_ItemBase::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	// {
	// 	auto EmptyContainer = FGameplayTagContainer::EmptyContainer;
	//
	// 	EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag());
	//
	// 	if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
	// 	    EmptyContainer.Num())
	// 	{
	// 		SwitchState(EState::kOriginal);
	//
	// 		return;
	// 	}
	// }
	// {
	// 	auto EmptyContainer = FGameplayTagContainer::EmptyContainer;
	//
	// 	EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
	//
	// 	//  只要是楼层就显示
	// 	if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
	// 	// if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
	// 	//     EmptyContainer.Num())
	// 	{
	// 		// 确认当前的模式
	// 		auto DecoratorSPtr =
	// 			DynamicCastSharedPtr<FInteraction_Decorator>(
	// 			                                             USceneInteractionWorldSystem::GetInstance()->
	// 			                                             GetDecorator(
	// 			                                                          USmartCitySuiteTags::Interaction_Interaction
	// 			                                                         )
	// 			                                            );
	// 		if (DecoratorSPtr)
	// 		{
	// 			if (DecoratorSPtr->Config.WallTranlucent <= 0)
	// 			{
	// 				SwitchState(EState::kHiden);
	// 			}
	// 			else if (DecoratorSPtr->Config.WallTranlucent >= 100)
	// 			{
	// 				SwitchState(EState::kOriginal);
	// 			}
	// 			else
	// 			{
	// 				SetTranslucent(DecoratorSPtr->Config.WallTranlucent);
	// 			}
	//
	// 			return;
	// 		}
	//
	// 		SwitchState(EState::kOriginal);
	//
	// 		return;
	// 	}
	// }
	// {
	// 	auto EmptyContainer = FGameplayTagContainer::EmptyContainer;
	//
	// 	if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
	// 	    EmptyContainer.Num())
	// 	{
	// 		SwitchState(EState::kHiden);
	//
	// 		return;
	// 	}
	// }
}

void ABuilding_ItemBase::SwitchState(
	EState State
	)
{
	switch (State)
	{
	case EState::kOriginal:
		{
			SetActorHiddenInGame(false);

			RevertOnriginalMat();
		}
		break;
	case EState::kHiden:
		{
			SetActorHiddenInGame(true);
		}
		break;
	}
}

void ABuilding_ItemBase::SetTranslucent(
	int32 Value
	)
{
	SetActorHiddenInGame(false);

	TArray<UStaticMeshComponent*> Components;
	GetComponents<UStaticMeshComponent>(Components);
	
	SetTranslucentImp(Components, Value, UAssetRefMap::GetInstance()->WallTranslucentMatInst);
}
