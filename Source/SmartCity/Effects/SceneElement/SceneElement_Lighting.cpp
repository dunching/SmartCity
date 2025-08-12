#include "SceneElement_Lighting.h"

#include "Engine/StaticMeshActor.h"

#include "GameplayTagsLibrary.h"

ASceneElement_Lighting::ASceneElement_Lighting(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
}

void ASceneElement_Lighting::ReplaceImp(
	AActor* ActorPtr
	)
{
	Super::ReplaceImp(ActorPtr);

	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			MeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());

			const auto Num = MeshComponent->GetNumMaterials();
			for (int32 Index = 0; Index < Num; Index++)
			{
				auto MaterialPtr = UMaterialInstanceDynamic::Create(EmissiveMaterialInstance.LoadSynchronous(), this);
				MeshComponent->SetMaterial(Index, MaterialPtr);
			}
		}
	}
}

void ASceneElement_Lighting::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_ExternalWall);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(true);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);
		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Mode_PWR_Lighting);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			SetEmissiveValue(1);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			SetEmissiveValue(0);

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

void ASceneElement_Lighting::SetEmissiveValue(
	int32 Value
	)
{
	const auto Num = MeshComponent->GetNumMaterials();
	for (int32 Index = 0; Index < Num; Index++)
	{
		auto MaterialPtr = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(Index));
		if (MaterialPtr)
		{
			MaterialPtr->SetScalarParameterValue(EmissiveValue, Value);
		}
	}
}
