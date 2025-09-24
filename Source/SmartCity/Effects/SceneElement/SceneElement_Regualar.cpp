#include "SceneElement_Regualar.h"

#include "ActorSequenceComponent.h"
#include "DatasmithAssetUserData.h"
#include "Engine/StaticMeshActor.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "MessageBody.h"
#include "RouteMarker.h"
#include "SmartCitySuiteTags.h"
#include "WebChannelWorldSystem.h"

ASceneElement_Regualar::ASceneElement_Regualar(
	const FObjectInitializer& ObjectInitializer
	):
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void ASceneElement_Regualar::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneElement_Regualar::ReplaceImp(
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
		}
	}
}

void ASceneElement_Regualar::BeginInteraction()
{
	Super::BeginInteraction();

	bIsOpened = true;
}

void ASceneElement_Regualar::EndInteraction()
{
	Super::EndInteraction();

	bIsOpened = false;
}

TMap<FString, FString> ASceneElement_Regualar::GetStateDescription() const
{
	TMap<FString, FString> Result;

	if (bIsOpened)
	{
		Result.Add(TEXT(""), TEXT("开启"));
	}
	else
	{
		Result.Add(TEXT(""), TEXT("关闭"));
	}

	return Result;
}

void ASceneElement_Regualar::SwitchInteractionType(
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
			SetActorHiddenInGame(false);

			auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetRenderCustomDepth(false);
			}

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetRenderCustomDepth(false);
			}

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

		return;
	}
}
