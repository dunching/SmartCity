#include "SceneElement_HVAC.h"

#include "Engine/StaticMeshActor.h"
#include "ActorSequenceComponent.h"
#include "NiagaraComponent.h"

#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "InputProcessorSubSystem_Imp.h"
#include "SmartCitySuiteTags.h"
#include "ViewSingleDeviceProcessor.h"
#include "TourPawn.h"

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
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag());
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(true);
		
			NiagaraComponentPtr->SetActive(false);
			
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_HVAC.GetTag());
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			NiagaraComponentPtr->SetActive(true);
		
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			NiagaraComponentPtr->SetActive(false);
		
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_View.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
			EmptyContainer.Num())
		{
			UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<TourProcessor::FViewSingleDeviceProcessor>(
				 [this](
				 auto NewProcessor
				 )
				 {
					 NewProcessor->TargetDevicePtr = this;
				 }
				);

			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}
		SetActorHiddenInGame(true);
		
		NiagaraComponentPtr->SetActive(false);
			
		return;
	}
}

void ASceneElement_HVAC::ReplaceImp(
	AActor* ActorPtr
	)
{
	Super::ReplaceImp(ActorPtr);
}
