#include "SceneElement_HVAC.h"

#include "Engine/StaticMeshActor.h"
#include "ActorSequenceComponent.h"
#include "NiagaraComponent.h"
#include "DatasmithAssetUserData.h"

#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "IPSSI.h"
#include "MessageBody.h"
#include "SmartCitySuiteTags.h"
#include "ViewSingleDeviceProcessor.h"
#include "TourPawn.h"
#include "WebChannelWorldSystem.h"

ASceneElement_HVAC::ASceneElement_HVAC(
	const FObjectInitializer& ObjectInitializer
	) :
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
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			QuitAllState();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_HVAC)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			EntryShowevice();
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_View.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			EntryViewDevice();
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_Focus.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			EntryFocusDevice();
			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}
		QuitAllState();

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

void ASceneElement_HVAC::EntryFocusDevice()
{
	Super::EntryFocusDevice();

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
}

void ASceneElement_HVAC::QuitFocusDevice()
{
	Super::QuitFocusDevice();
}

void ASceneElement_HVAC::EntryViewDevice()
{
	Super::EntryViewDevice();

	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<TourProcessor::FViewSingleDeviceProcessor>(
		 [this](
		 auto NewProcessor
		 )
		 {
			 NewProcessor->TargetDevicePtr = this;
		 }
		);
}

void ASceneElement_HVAC::QuitViewDevice()
{
	Super::QuitViewDevice();
}

void ASceneElement_HVAC::EntryShowevice()
{
	Super::EntryShowevice();

	SetActorHiddenInGame(false);

	NiagaraComponentPtr->SetActive(false);
}

void ASceneElement_HVAC::QuitShowDevice()
{
	Super::QuitShowDevice();
}

void ASceneElement_HVAC::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();

	SetActorHiddenInGame(false);

	NiagaraComponentPtr->SetActive(true);
}

void ASceneElement_HVAC::QuitShowDeviceEffect()
{
	Super::QuitShowDeviceEffect();

	NiagaraComponentPtr->SetActive(false);
}

void ASceneElement_HVAC::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(true);

	auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
	if (PrimitiveComponentPtr)
	{
		PrimitiveComponentPtr->SetRenderCustomDepth(false);
	}
}
