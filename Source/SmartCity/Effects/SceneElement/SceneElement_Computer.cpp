#include "SceneElement_Computer.h"

#include "Engine/StaticMeshActor.h"
#include "ActorSequenceComponent.h"
#include "DatasmithAssetUserData.h"
#include "Components/BoxComponent.h"

#include "CollisionDataStruct.h"
#include "FloorHelper.h"
#include "MessageBody.h"
#include "SmartCitySuiteTags.h"
#include "WebChannelWorldSystem.h"
#include "ComputerMark.h"
#include "Components/TextBlock.h"

void UComputerMarkGroupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetAlignmentInViewport(FVector2D(0.5f, 1.f));
}

void UComputerMarkGroupWidget::SetText(
	const FString& InText
	)
{
	NameText->SetText(FText::FromString(InText));
}

FVector UComputerMarkGroupWidget::GetHoverPosition()
{
	return TargetPt;
}

ASceneElement_Computer::ASceneElement_Computer(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	// CollisionComponentHelper->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASceneElement_Computer::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);

	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
	}
}

void ASceneElement_Computer::Merge(
	const TSoftObjectPtr<AActor>& ActorRef,
	const TPair<FName, FString>& InUserData,
	const TMap<FName, FString>& NewUserData
	)
{
	Super::Merge(ActorRef, InUserData, NewUserData);

	CurrentUserData = InUserData;

	if (ActorRef.ToSoftObjectPath().IsValid())
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorRef.Get());
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

			auto Transform =
				STPtr->GetStaticMeshComponent()->
				       GetComponentTransform();

			auto NewComponentPtr = Cast<UStaticMeshComponent>(
			                                                  AddComponentByClass(
				                                                   UStaticMeshComponent::StaticClass(),
				                                                   true,
				                                                   Transform,
				                                                   false
				                                                  )
			                                                 );

			NewComponentPtr->AddAssetUserData(AUDPtr);

			NewComponentPtr->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());
			NewComponentPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

			NewComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			NewComponentPtr->SetCollisionObjectType(Device_Object);
			NewComponentPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

			NewComponentPtr->SetCollisionResponseToChannel(ExternalWall_Object, ECollisionResponse::ECR_Overlap);
			NewComponentPtr->SetCollisionResponseToChannel(Floor_Object, ECollisionResponse::ECR_Overlap);
			NewComponentPtr->SetCollisionResponseToChannel(Space_Object, ECollisionResponse::ECR_Overlap);

			NewComponentPtr->SetRenderCustomDepth(false);

			StaticMeshComponentsAry.Add(NewComponentPtr);

			UpdateCollisionBox(StaticMeshComponentsAry);
		}

		ActorRef->Destroy();
	}
}

void ASceneElement_Computer::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall) ||
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_Periphery)
		)
		{
			if (WidgetPtr)
			{
				WidgetPtr->RemoveFromParent();
			}
			WidgetPtr = nullptr;

			QuitAllState();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_SpecialArea) 
		)
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
		)
		{
			if (WidgetPtr)
			{
				WidgetPtr->RemoveFromParent();
			}
			WidgetPtr = nullptr;

			EntryShowDevice();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_BatchControl)
		)
		{
			if (WidgetPtr)
			{
				WidgetPtr->RemoveFromParent();
			}
			WidgetPtr = nullptr;

			EntryShowDevice();

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
			if (WidgetPtr)
			{
				WidgetPtr->RemoveFromParent();
			}
			WidgetPtr = nullptr;

			EntryShowDevice();

			return;
		}
	}
	{
		if ((ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
		     ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)))
		{
			if (WidgetPtr)
			{
				WidgetPtr->RemoveFromParent();
			}
			WidgetPtr = nullptr;

			EntryShowDevice();

			return;
		}
	}

	if (ProcessJiaCengLogic(ConditionalSet))
	{
		if (WidgetPtr)
		{
			WidgetPtr->RemoveFromParent();
		}
		WidgetPtr = nullptr;

		QuitAllState();
		return;
	}

	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}
		if (WidgetPtr)
		{
			WidgetPtr->RemoveFromParent();
		}
		WidgetPtr = nullptr;

		QuitAllState();

		return;
	}
}

void ASceneElement_Computer::EntryFocusDevice()
{
	Super::EntryFocusDevice();

	SetActorHiddenInGame(false);

	auto MessageBodySPtr = MakeShared<FMessageBody_ViewDevice>();

	MessageBodySPtr->DeviceID = SceneElementID;
	MessageBodySPtr->Type = DeviceTypeStr;

	UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);
}

void ASceneElement_Computer::EntryViewDevice()
{
	Super::EntryViewDevice();

	SetActorHiddenInGame(false);
}

void ASceneElement_Computer::EntryShowDevice()
{
	Super::EntryShowDevice();

	SetActorHiddenInGame(false);
}

void ASceneElement_Computer::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();

	SetActorHiddenInGame(false);
}

void ASceneElement_Computer::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(true);
}

TPair<FTransform, float> ASceneElement_Computer::GetViewSeat() const
{
	const auto FloorBoxPt = BelongFloor->BoxComponentPtr->GetComponentLocation();

	const auto BoxPt = CollisionComponentHelper->GetComponentLocation();

	const auto Bounds = CollisionComponentHelper->GetScaledBoxExtent();

	const auto Pt1 =
		BoxPt + FVector(
		                0,
		                Bounds.Y,
		                0
		               );
	const auto Pt2 =
		BoxPt - FVector(
		                0,
		                Bounds.Y,
		                0
		               );

	DrawDebugSphere(GetWorld(), Pt1, 20, 20, FColor::Red, false, 10);
	DrawDebugSphere(GetWorld(), Pt2, 20, 20, FColor::Yellow, false, 10);

	TPair<FTransform, float> Result;
	Result.Value = 130;

	if (FVector::Distance(FloorBoxPt, Pt1) > FVector::Distance(FloorBoxPt, Pt2))
	{
		Result.Key.SetLocation(Pt1);
		Result.Key.SetRotation((CollisionComponentHelper->GetComponentRotation() + FRotator(0, -90, 0)).Quaternion());
	}
	else
	{
		Result.Key.SetLocation(Pt2);
		Result.Key.SetRotation((CollisionComponentHelper->GetComponentRotation() + FRotator(0, 90, 0)).Quaternion());
	}

	return Result;
}

void ASceneElement_Computer::DisplayGroupWidget()
{
	auto FloorPtr = Cast<AFloorHelper_Computer>(BelongFloor);
	if (FloorPtr)
	{
		FString Str;
		ON_SCOPE_EXIT

		{
			WidgetPtr = CreateWidget<UComputerMarkGroupWidget>(
			                                                   GetWorld(),
			                                                   ComputerMarkClass
			                                                  );


			WidgetPtr->SetText(Str);


			const auto FloorBoxPt = BelongFloor->BoxComponentPtr->GetComponentLocation();

			const auto BoxPt = CollisionComponentHelper->GetComponentLocation();

			const auto Bounds = CollisionComponentHelper->GetScaledBoxExtent();

			const auto Pt1 =
				BoxPt +
					FVector(
				                0,
				                Bounds.Y,
				                0
				                ) +
				                	FVector(
								 0,
								 0,
								 Bounds.Z
								);
			
			const auto Pt2 =
				BoxPt - FVector(
				                0,
				                Bounds.Y,
				                0
				                ) + FVector(
								 0,
								 0,
								 Bounds.Z
								);

			if (FVector::Distance(FloorBoxPt, Pt1) > FVector::Distance(FloorBoxPt, Pt2))
			{
				WidgetPtr->TargetPt = Pt1;
			}
			else
			{
				WidgetPtr->TargetPt = Pt2;
			}

			WidgetPtr->AddToViewport();
		};

		for (const auto& Iter : FloorPtr->ComputerNameMap)
		{
			for (const auto& SecondIter : Iter.Value.Names)
			{
				if (SecondIter == CurrentUserData.Value)
				{
					Str = Iter.Key;
					return;
				}
			}
		}
	}
}
