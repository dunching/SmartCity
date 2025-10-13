#include "SceneElement_SunShade.h"

#include "ActorSequenceComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Blueprint/UserWidget.h"

#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "RouteMarker.h"
#include "AssetRefMap.h"
#include "FloorHelper.h"
#include "SmartCitySuiteTags.h"
#include "Components/BoxComponent.h"

ASceneElement_SunShade::ASceneElement_SunShade(
	const FObjectInitializer& ObjectInitializer
	) :
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
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);

	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto ParentPtr = ActorPtr->GetAttachParentActor();

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

		if (!FloorPtr)
		{
			return;
		}

		const auto FloorCenter = FloorPtr->BoxComponentPtr->CalcBounds(FloorPtr->BoxComponentPtr->GetComponentToWorld())
		                                 .GetBox().GetCenter();
		const auto Dir = FloorCenter - GetActorLocation();

		const auto Dot = FVector::DotProduct(Dir, GetActorRightVector());

		// DrawDebugSphere(GetWorld(), FloorCenter, 20, 20, FColor::Red, true);
		// DrawDebugLine(
		//               GetWorld(),
		//               GetActorLocation(),
		//               GetActorLocation() + (GetActorRightVector() * 100),
		//               FColor::Red,
		//               true
		//              );

		if (Dot > 0)
		{
			SetActorRotation(GetActorRotation() + FRotator(0, 180, 0));
		}
		else
		{
		}

		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			ChestMeshComponent->SetRelativeTransform(STPtr->GetStaticMeshComponent()->GetRelativeTransform());

			FanMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());

			FVector Min;
			FVector Max;
			FanMeshComponent->GetLocalBounds(Min, Max);

			FBox Box(Min, Max);

			FanAncherMeshComponent->SetRelativeLocation(FVector(0, 0, Box.GetSize().Z));
			FanMeshComponent->SetRelativeLocation(FVector(0, 0, -Box.GetSize().Z));
		}
	}
}

void ASceneElement_SunShade::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	// Super::SwitchInteractionType(ConditionalSet);

	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			UpdateAngle(0);

			RemoveRouteMarker();

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_SunShadow);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			UpdateAngle(0);

			// RouteMarkerPtr = CreateWidget<URouteMarker>(
			//                                             GEngine->GetFirstLocalPlayerController(GetWorld()),
			//                                             UAssetRefMap::GetInstance()->RouteMarkerClass
			//                                            );
			// if (RouteMarkerPtr)
			// {
			// 	RouteMarkerPtr->TextStr = TEXT("遮阳");
			// 	RouteMarkerPtr->TargetActor = this;
			// 	RouteMarkerPtr->AddToViewport();
			// }

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

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			UpdateAngle(-90);

			RemoveRouteMarker();

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

		//
		SetActorHiddenInGame(true);

		UpdateAngle(0);

		RemoveRouteMarker();

		return;
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

	FanAncherMeshComponent->SetRelativeRotation(FRotator(0, 0, CurrentAngle));
}

void ASceneElement_SunShade::RemoveRouteMarker()
{
	if (RouteMarkerPtr)
	{
		RouteMarkerPtr->RemoveFromParent();
	}
	RouteMarkerPtr = nullptr;
}
