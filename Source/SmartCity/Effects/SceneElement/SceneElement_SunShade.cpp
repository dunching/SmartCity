#include "SceneElement_SunShade.h"

#include "ActorSequenceComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Blueprint/UserWidget.h"

#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "RouteMarker.h"
#include "AssetRefMap.h"

ASceneElement_SunShade::ASceneElement_SunShade(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	ChestMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMeshComponent"));
	ChestMeshComponent->SetupAttachment(RootComponent);

	ChestMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ChestMeshComponent->SetCollisionObjectType(Device_Object);
	ChestMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	FanAncherMeshComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FanAncherMeshComponent"));
	FanAncherMeshComponent->SetupAttachment(RootComponent);

	FanMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FanMeshComponent"));
	FanMeshComponent->SetupAttachment(FanAncherMeshComponent);

	FanMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FanMeshComponent->SetCollisionObjectType(Device_Object);
	FanMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void ASceneElement_SunShade::ReplaceImp(
	AActor* ActorPtr
	)
{
	Super::ReplaceImp(ActorPtr);

	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			FanMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());

			FVector Min;
			FVector Max;
			FanMeshComponent->GetLocalBounds(Min, Max);

			FBox Box(Min, Max);

			FanAncherMeshComponent->SetRelativeLocation(FVector(0, 0, Box.GetExtent().Z));
			FanMeshComponent->SetRelativeLocation(FVector(0, 0, -Box.GetExtent().Z));
		}
	}
}

void ASceneElement_SunShade::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
			SetActorHiddenInGame(true);

			RemoveRouteMarker();

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_ExternalWall);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(false);

			RemoveRouteMarker();

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);
		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Mode_SunShade);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(false);

			RouteMarkerPtr = CreateWidget<URouteMarker>(
															 GEngine->GetFirstLocalPlayerController(GetWorld()),
															 UAssetRefMap::GetInstance()->RouteMarkerClass
															);
			if (RouteMarkerPtr)
			{
				RouteMarkerPtr->TextStr = TEXT("遮阳");
				RouteMarkerPtr->TargetActor = this;
				RouteMarkerPtr->AddToViewport();
			}

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(false);

			RemoveRouteMarker();

			return;
		}
	}
}

void ASceneElement_SunShade::UpdateAngle(
	int32 Angle
	)
{
	if (Angle != TargetAngle)
	{
		TargetAngle = Angle;

		GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::UpdateAngleImp, Frequency, true);
	}
}

void ASceneElement_SunShade::UpdateAngleImp()
{
	if (CurrentAngle > TargetAngle)
	{
		CurrentAngle -= Frequency * Speed;
		if (CurrentAngle < TargetAngle)
		{
			CurrentAngle = TargetAngle;
			
			GetWorldTimerManager().ClearTimer(TimerHandle);
		}
	}
	else if (CurrentAngle == TargetAngle)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
	}
	else
	{
		CurrentAngle += Frequency * Speed;
		if (CurrentAngle > TargetAngle)
		{
			CurrentAngle = TargetAngle;
			
			GetWorldTimerManager().ClearTimer(TimerHandle);
		}
	}

	FanAncherMeshComponent->SetRelativeRotation(FRotator(CurrentAngle, 0, 0));
}

void ASceneElement_SunShade::RemoveRouteMarker()
{
	if (RouteMarkerPtr)
	{
		RouteMarkerPtr->RemoveFromParent();
	}
	RouteMarkerPtr = nullptr;
}
