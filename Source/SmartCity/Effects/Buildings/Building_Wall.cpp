#include "Building_Wall.h"

#include "Engine/StaticMeshActor.h"

#include "CollisionDataStruct.h"
#include "SmartCitySuiteTags.h"

ABuilding_Wall::ABuilding_Wall(
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

void ABuilding_Wall::ReplaceImp(
	AActor* ActorPtr
	)
{
	Super::ReplaceImp(ActorPtr);

	TArray<AActor*> OutActors;
	ActorPtr->GetAttachedActors(OutActors);
	for (auto Iter : OutActors)
	{
		Iter->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}
	
	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			StaticMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());

			for (int32 Index = 0; Index < STPtr->GetStaticMeshComponent()->GetNumMaterials(); Index++)
			{
				StaticMeshComponent->SetMaterial(Index, STPtr->GetStaticMeshComponent()->GetMaterial(Index));
			}
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

void ABuilding_Wall::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);
	
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag());
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(true);

			return;
		}
	}
}
