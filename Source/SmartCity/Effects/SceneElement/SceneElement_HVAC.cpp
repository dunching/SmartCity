#include "SceneElement_HVAC.h"

#include "Engine/StaticMeshActor.h"
#include "ActorSequenceComponent.h"
#include "NiagaraComponent.h"
#include "DatasmithAssetUserData.h"

#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "InputProcessorSubSystem_Imp.h"
#include "MessageBody.h"
#include "SmartCitySuiteTags.h"
#include "ViewSingleDeviceProcessor.h"
#include "TourPawn.h"
#include "WebChannelWorldSystem.h"

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
	// Super::SwitchInteractionType(ConditionalSet);
	
	if (ProcessJiaCengLogic(ConditionalSet))
	{
		SetActorHiddenInGame(true);
		return;
	}

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
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
			EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

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
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_Focus.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
			EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetRenderCustomDepth(true);
				PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
			}

			auto MessageBodySPtr = MakeShared<FMessageBody_SelectedDevice>();

			MessageBodySPtr->DeviceID = DeviceID;

			UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);

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
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);
	
	auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
	if (STPtr)
	{
		auto InterfacePtr = Cast<IInterface_AssetUserData>(STPtr->GetStaticMeshComponent());
		if (!InterfacePtr)
		{
			return;
		}
		auto AUDPtr = Cast<UDatasmithAssetUserData>(
													InterfacePtr->GetAssetUserDataOfClass(
														 UDatasmithAssetUserData::StaticClass()
														)
												   );

		CheckIsJiaCeng(AUDPtr);
	}
}
