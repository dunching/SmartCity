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

ASceneElement_Computer::ASceneElement_Computer(
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

void ASceneElement_Computer::ReplaceImp(
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
		}
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
			QuitAllState();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
		)
		{
			EntryShowDevice();

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

void ASceneElement_Computer::EntryFocusDevice()
{
	Super::EntryFocusDevice();

	SetActorHiddenInGame(false);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(true);
		StaticMeshComponent->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	}

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
	const auto BoxPt = BelongFloor->BoxComponentPtr->GetComponentLocation();

	const auto STCTransform = StaticMeshComponent->GetComponentTransform();

	FVector Min;
	FVector Max;
	StaticMeshComponent->GetLocalBounds(Min, Max);
	FBox Bounds(Min, Max);

	const auto Pt1 = STCTransform.TransformPosition(
	                                                Bounds.GetCenter() + FVector(
		                                                 0,
		                                                 Bounds.GetExtent().Y,
		                                                 0
		                                                )
	                                               );
	const auto Pt2 = STCTransform.TransformPosition(
	                                                Bounds.GetCenter() - FVector(
		                                                 0,
		                                                 Bounds.GetExtent().Y,
		                                                 0
		                                                )
	                                               );

	DrawDebugSphere(GetWorld(), Pt1, 20, 20, FColor::Red, false, 10);
	DrawDebugSphere(GetWorld(), Pt2, 20, 20, FColor::Yellow, false, 10);

	if (FVector::Distance(BoxPt, Pt1) > FVector::Distance(BoxPt, Pt2))
	{
		TPair<FTransform, float> Result;

		Result.Key.SetLocation(Pt1);
		Result.Key.SetRotation((StaticMeshComponent->GetComponentRotation() + FRotator(0, -90, 0)).Quaternion());
		Result.Value = 160;

		return Result;
	}
	else
	{
		TPair<FTransform, float> Result;

		Result.Key.SetLocation(Pt2);
		Result.Key.SetRotation((StaticMeshComponent->GetComponentRotation() + FRotator(0, 90, 0)).Quaternion());
		Result.Value = 60;

		return Result;
	}
}
