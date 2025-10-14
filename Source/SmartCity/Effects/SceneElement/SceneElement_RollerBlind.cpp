#include "SceneElement_RollerBlind.h"

#include "ActorSequenceComponent.h"
#include "ActorSequencePlayer.h"

#include "AssetRefMap.h"
#include "MessageBody.h"
#include "SmartCitySuiteTags.h"
#include "WebChannelWorldSystem.h"
#include "Kismet/KismetMathLibrary.h"

ASceneElement_RollerBlind::ASceneElement_RollerBlind(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void ASceneElement_RollerBlind::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneElement_RollerBlind::EndPlay(
	const EEndPlayReason::Type EndPlayReason
	)
{
	Super::EndPlay(EndPlayReason);
}

void ASceneElement_RollerBlind::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	 Super::SwitchInteractionType(ConditionalSet);

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall)
		)
		{
			QuitAllState();
			
			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_SunShadow)
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
			EntryShowevice();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode)
		)
		{
			QuitAllState();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor)
			)
		{
			EntryShowevice();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_Focus)
			)
		{
			EntryShowevice();

			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}

		//
		QuitAllState();

		return;
	}
}

void ASceneElement_RollerBlind::EntryFocusDevice()
{
	Super::EntryFocusDevice();

	TArray<UStaticMeshComponent*> OutComponents;
	GetComponents<UStaticMeshComponent>(OutComponents);

	for (auto Iter : OutComponents)
	{
		if (Iter)
		{
			Iter->SetRenderCustomDepth(true);
			Iter->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
		}
	}

	auto MessageBodySPtr = MakeShared<FMessageBody_ViewDevice>();

	MessageBodySPtr->DeviceID = SceneElementID;
	MessageBodySPtr->Type = DeviceTypeStr;

	UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);
}

void ASceneElement_RollerBlind::EntryViewDevice()
{
	Super::EntryViewDevice();
	SetActorHiddenInGame(false);

}

void ASceneElement_RollerBlind::EntryShowevice()
{
	Super::EntryShowevice();
	SetActorHiddenInGame(false);

	PlayAnimation(0);
			
}

void ASceneElement_RollerBlind::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();
	SetActorHiddenInGame(false);

	PlayAnimation(UKismetMathLibrary::RandomFloatInRange(0,1));
			
}

void ASceneElement_RollerBlind::QuitAllState()
{
	Super::QuitAllState();
	
	SetActorHiddenInGame(true);

	TArray<UStaticMeshComponent*> OutComponents;
	GetComponents<UStaticMeshComponent>(OutComponents);

	for (auto Iter : OutComponents)
	{
		if (Iter)
		{
			Iter->SetRenderCustomDepth(false);
		}
	}

	
	PlayAnimation(0);
			
}

void ASceneElement_RollerBlind::PlayAnimation(
	float Percent
	)
{
	if (MySequenceComponent->GetSequencePlayer() != nullptr)
	{
		const auto Second = MySequenceComponent->GetSequencePlayer()->GetDuration().AsSeconds();
		MySequenceComponent->GetSequencePlayer()->SetTimeRange(0, Percent * Duration);
		MySequenceComponent->GetSequencePlayer()->Play();
	}
}
