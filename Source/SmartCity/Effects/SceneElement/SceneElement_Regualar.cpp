#include "SceneElement_Regualar.h"

#include "ActorSequenceComponent.h"
#include "DatasmithAssetUserData.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "FloorHelper.h"
#include "GameplayTagsLibrary.h"
#include "MessageBody.h"
#include "RouteMarker.h"
#include "SmartCitySuiteTags.h"
#include "WebChannelWorldSystem.h"

ASceneElement_Regualar::ASceneElement_Regualar(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	
	CollisionComponentHelper = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponentHelper"));
	CollisionComponentHelper->SetupAttachment(RootComponent);
			
	CollisionComponentHelper->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponentHelper->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComponentHelper->SetCollisionResponseToChannel(Space_Object, ECollisionResponse::ECR_Overlap);
	CollisionComponentHelper->SetCollisionObjectType(Device_Object);
}

void ASceneElement_Regualar::BeginPlay()
{
	Super::BeginPlay();
}

FBox ASceneElement_Regualar::GetComponentsBoundingBox(
	bool bNonColliding,
	bool bIncludeFromChildActors
	) const
{
	FBox Box(ForceInit);

	ForEachComponent<UPrimitiveComponent>(
	                                      bIncludeFromChildActors,
	                                      [&](
	                                      const UPrimitiveComponent* InPrimComp
	                                      )
	                                      {
		                                      if (StaticMeshComponent->GetStaticMesh())
		                                      {
			                                      // Only use collidable components to find collision bounding box.
			                                      if (InPrimComp->IsRegistered() && (
				                                          bNonColliding || InPrimComp->IsCollisionEnabled()))
			                                      {
				                                      Box += InPrimComp->Bounds.GetBox();
			                                      }
		                                      }
	                                      }
	                                     );

	return Box;
}

void ASceneElement_Regualar::InitialSceneElement()
{
	Super::InitialSceneElement();

	if (BelongFloor)
	{
		return;
	}
	
	auto ParentPtr = GetAttachParentActor();
	AFloorHelper* FloorPtr = nullptr;
	for (; ParentPtr;)
	{
		ParentPtr = ParentPtr->GetAttachParentActor();
		FloorPtr = Cast<AFloorHelper>(ParentPtr);
		if (FloorPtr)
		{
			break;
		}
	}

	if (FloorPtr)
	{
		BelongFloor = FloorPtr;
	}
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

			StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			FBox Box(ForceInit);
			StaticMeshComponent->GetLocalBounds(Box.Min, Box.Max);
			
			CollisionComponentHelper->SetRelativeLocation(Box.GetCenter());

			CollisionComponentHelper->SetBoxExtent(Box.GetExtent());
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
	Super::SwitchInteractionType(ConditionalSet);

	if (ProcessJiaCengLogic(ConditionalSet))
	{
		SetActorHiddenInGame(true);
		return;
	}

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall)
		)
		{
			if (BelongFloor && BelongFloor->FloorTag.MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor_Roof))
			{
				SetActorHiddenInGame(false);

				auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
				if (PrimitiveComponentPtr)
				{
					PrimitiveComponentPtr->SetRenderCustomDepth(false);
				}

			}
			else
			{
				QuitAllState();
			}

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor)
		)
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
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
		)
		{
			SetActorHiddenInGame(false);

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_Focus)
		)
		{
			SetActorHiddenInGame(false);

			auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetRenderCustomDepth(true);
				PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
			}

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

void ASceneElement_Regualar::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(true);

	auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
	if (PrimitiveComponentPtr)
	{
		PrimitiveComponentPtr->SetRenderCustomDepth(false);
	}
}
