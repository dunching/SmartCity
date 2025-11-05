#include "SceneElement_InfraredDetector.h"

#include "Components/BoxComponent.h"
#include "Marks/PersonMark.h"
#include "Engine/StaticMeshActor.h"

#include "AssetRefMap.h"
#include "FloorHelper.h"
#include "GameplayTagsLibrary.h"
#include "MessageBody.h"
#include "SmartCitySuiteTags.h"
#include "PersonMark.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionWorldSystem.h"
#include "TemplateHelper.h"
#include "WebChannelWorldSystem.h"

ASceneElement_InfraredDetector::ASceneElement_InfraredDetector(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RelativeTransformComponent);

	AnchorComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AnchorComponent"));
	AnchorComponent->SetupAttachment(StaticMeshComponent);

	SweepEffectStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(
		 TEXT("SweepEffectStaticMeshComponent")
		);
	SweepEffectStaticMeshComponent->SetupAttachment(AnchorComponent);
	SweepEffectStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			
	SweepEffectStaticMeshComponent->SetCastShadow(false);
	SweepEffectStaticMeshComponent->SetReceivesDecals(false);
}

void ASceneElement_InfraredDetector::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);

	FVector Min;
	FVector Max;
	SweepEffectStaticMeshComponent->GetLocalBounds(Min, Max);

	const auto Scale = Deepth / (Max.Z - Min.Z);
	AnchorComponent->SetRelativeScale3D(FVector(1, 1, Scale));
}

FBox ASceneElement_InfraredDetector::GetComponentsBoundingBox(
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
		                                      if (InPrimComp == SweepEffectStaticMeshComponent)
		                                      {
			                                      return;
		                                      }

		                                      if (InPrimComp == AnchorComponent)
		                                      {
			                                      return;
		                                      }

		                                      // Only use collidable components to find collision bounding box.
		                                      if (InPrimComp->IsRegistered() && (
			                                          bNonColliding || InPrimComp->IsCollisionEnabled()))
		                                      {
			                                      Box += InPrimComp->Bounds.GetBox();
		                                      }
	                                      }
	                                     );

	return Box;
}

void ASceneElement_InfraredDetector::ReplaceImp(
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
			StaticMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());
		}
	}
}

void ASceneElement_InfraredDetector::SwitchInteractionType(
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
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Interaction) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception)
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
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_Focus)
		)
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

void ASceneElement_InfraredDetector::EntryFocusDevice()
{
	Super::EntryFocusDevice();

	SetActorHiddenInGame(false);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(true);
		StaticMeshComponent->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	}
}

void ASceneElement_InfraredDetector::EntryViewDevice()
{
	Super::EntryViewDevice();
}

void ASceneElement_InfraredDetector::EntryShowDevice()
{
	Super::EntryShowDevice();


	SetActorHiddenInGame(false);


	SweepEffectStaticMeshComponent->SetHiddenInGame(true);
}

void ASceneElement_InfraredDetector::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();
	SetActorHiddenInGame(false);

	SweepEffectStaticMeshComponent->SetHiddenInGame(false);
}

void ASceneElement_InfraredDetector::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(true);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(false);
	}
}
