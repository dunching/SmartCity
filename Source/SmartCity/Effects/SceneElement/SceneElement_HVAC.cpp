#include "SceneElement_HVAC.h"

#include "Engine/StaticMeshActor.h"
#include "ActorSequenceComponent.h"
#include "NiagaraComponent.h"

#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"

ASceneElement_HVAC::ASceneElement_HVAC(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComponent->SetCollisionObjectType(Device_Object);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	NiagaraComponentPtr = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponentPtr->SetupAttachment(RootComponent);
	
	NiagaraComponentPtr->SetAutoActivate(false);
}

void ASceneElement_HVAC::BeginPlay()
{
	Super::BeginPlay();

	
	NiagaraComponentPtr->SetBoolParameter(TEXT("DrawBounds"), false);
}

void ASceneElement_HVAC::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);
	
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
			SetActorHiddenInGame(true);
		
			NiagaraComponentPtr->SetActive(false);
			
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_ExternalWall);
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(true);
		
			NiagaraComponentPtr->SetActive(false);
			
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);
		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Mode_PWR_HVAC);
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(false);

			NiagaraComponentPtr->SetActive(true);
		
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(false);

			NiagaraComponentPtr->SetActive(false);
		
			return;
		}
	}
}

void ASceneElement_HVAC::ReplaceImp(
	AActor* ActorPtr
	)
{
	Super::ReplaceImp(ActorPtr);
}
