#include "SceneElement_Monitor.h"

#include "Engine/StaticMeshActor.h"
#include "ActorSequenceComponent.h"
#include "DatasmithAssetUserData.h"
#include "Components/BoxComponent.h"

#include "CollisionDataStruct.h"
#include "MessageBody.h"
#include "SmartCitySuiteTags.h"
#include "WebChannelWorldSystem.h"
#include "Components/WidgetSwitcher.h"

ASceneElement_Monitor::ASceneElement_Monitor(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMeshComponent->SetupAttachment(RelativeTransformComponent);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComponent->SetCollisionObjectType(Device_Object);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void ASceneElement_Monitor::ReplaceImp(
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

			StaticMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());

			for (int32 Index = 0; Index < STPtr->GetStaticMeshComponent()->GetNumMaterials(); Index++)
			{
				StaticMeshComponent->SetMaterial(Index, STPtr->GetStaticMeshComponent()->GetMaterial(Index));
			}

			UpdateCollisionBox({StaticMeshComponent});
			
			const auto Box = CollisionComponentHelper->GetScaledBoxExtent();
			CollisionComponentHelper->SetBoxExtent(Box  + FVector(0));
		}
	}
}

void ASceneElement_Monitor::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	if (ProcessJiaCengLogic(ConditionalSet))
	{
		SetActorHiddenInGame(true);
		return;
	}

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall) ||
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_Periphery)
		)
		{
			QuitAllState();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_SafeManagement)
		)
		{
			EntryShoweviceEffect();

			{
				if (
					ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Selectd)
					)
				{
					EntryFocusDevice();

					if (MonitorHoverWidgetPtr)
					{
						MonitorHoverWidgetPtr->SetIsSelected(true);
			
						return;
					}
				}
			}

			if (MonitorHoverWidgetPtr)
			{
				MonitorHoverWidgetPtr->SetIsSelected(false);
			}
			
			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode)
		)
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if ((ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
		     ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)))
		{
			EntryShowDevice();

			return;
		}
	}

	if (ProcessJiaCengLogic(ConditionalSet))
	{
		QuitAllState();
		return;
	}

	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}
		QuitAllState();

		return;
	}
}

void ASceneElement_Monitor::EntryFocusDevice()
{
	Super::EntryFocusDevice();

	SetActorHiddenInGame(false);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(false);
//		StaticMeshComponent->SetRenderCustomDepth(true);
//		StaticMeshComponent->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	}

	auto MessageBodySPtr = MakeShared<FMessageBody_ViewDevice>();

	MessageBodySPtr->DeviceID = SceneElementID;
	MessageBodySPtr->Type = DeviceTypeStr;

	UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);
}

void ASceneElement_Monitor::EntryViewDevice()
{
	Super::EntryViewDevice();
	
	SetActorHiddenInGame(false);

	if (MonitorHoverWidgetPtr)
	{
		MonitorHoverWidgetPtr->RemoveFromParent();
	}
	MonitorHoverWidgetPtr = nullptr;
}

void ASceneElement_Monitor::EntryShowDevice()
{
	Super::EntryShowDevice();

	SetActorHiddenInGame(false);

	if (MonitorHoverWidgetPtr)
	{
		MonitorHoverWidgetPtr->RemoveFromParent();
	}
	MonitorHoverWidgetPtr = nullptr;
}

void ASceneElement_Monitor::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();
	
	SetActorHiddenInGame(false);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(false);
	}

	if (!MonitorHoverWidgetPtr)
	{
		MonitorHoverWidgetPtr = CreateWidget<UMonitorHoverWidget>(GetWorld(), MonitorHoverWidgetClass);
		MonitorHoverWidgetPtr->SceneElement_MonitorPtr = this;
		MonitorHoverWidgetPtr->Button->OnClicked.AddDynamic(this, &ThisClass::OnClicked);

		MonitorHoverWidgetPtr->AddToViewport();
	}
}

void ASceneElement_Monitor::QuitAllState()
{
	Super::QuitAllState();
	SetActorHiddenInGame(true);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(false);
	}

	if (MonitorHoverWidgetPtr)
	{
		MonitorHoverWidgetPtr->RemoveFromParent();
	}
	MonitorHoverWidgetPtr = nullptr;
}

void ASceneElement_Monitor::OnClicked()
{										
	auto MessageBodySPtr = MakeShared<FMessageBody_ClickedMonitor>();

	MessageBodySPtr->DeviceID = SceneElementID;
	MessageBodySPtr->Type = DeviceTypeStr;

	UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);									
}

void UMonitorHoverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetAlignmentInViewport(FVector2D(.5f, 1.f));

	SetIsSelected(false);
}

FVector UMonitorHoverWidget::GetHoverPosition()
{
	const auto BoxPt = SceneElement_MonitorPtr->CollisionComponentHelper->GetComponentLocation();

	const auto Bounds = SceneElement_MonitorPtr->CollisionComponentHelper->GetScaledBoxExtent();

	return BoxPt + FVector(0,0,Bounds.Z);
}

void UMonitorHoverWidget::SetIsSelected(
	bool bIsSelected
	)
{
	Switcher->SetActiveWidgetIndex(bIsSelected ? 0 : 1);
}

void UMonitorHoverWidget::SetOncliced(
	const FOnButtonClickedEvent& OnClicked
	)
{
}
