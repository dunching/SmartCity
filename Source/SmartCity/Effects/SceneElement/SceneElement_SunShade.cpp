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
	BaseComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMeshComponent"));
	BaseComponent->SetupAttachment(RootComponent);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SceneComponent->SetupAttachment(BaseComponent);

	FanAncherMeshComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FanAncherMeshComponent"));
	FanAncherMeshComponent->SetupAttachment(SceneComponent);

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

		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			const auto Transient = STPtr->GetStaticMeshComponent()->GetRelativeTransform();
			// BaseComponent->SetRelativeTransform(Transient);

			FanMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());

			FVector Min;
			FVector Max;
			FanMeshComponent->GetLocalBounds(Min, Max);

			FBox Box(Min, Max);

			FanAncherMeshComponent->SetRelativeLocation(FVector(0, 0, Box.GetSize().Z));
			FanMeshComponent->SetRelativeLocation(FVector(0, 0, -Box.GetSize().Z));
			
			const auto FloorCenter = FloorPtr->BoxComponentPtr->CalcBounds(FloorPtr->BoxComponentPtr->GetComponentToWorld())
											 .GetBox().GetCenter();
			const auto Dir = FloorCenter - GetActorLocation();

			const auto Dot = FVector::DotProduct(Dir, GetActorRightVector());

			if (Dot > 0)
			{
				// DrawDebugSphere(GetWorld(), FloorCenter, 20, 20, FColor::Red, true);
				// DrawDebugLine(
				//               GetWorld(),
				//               GetActorLocation(),
				//               GetActorLocation() + (GetActorRightVector() * 100),
				//               FColor::Red,
				//               true
				//              );

				SceneComponent->SetRelativeRotation(FRotator(0, 180, 0));
			}
			else
			{
				// DrawDebugSphere(GetWorld(), FloorCenter, 20, 20, FColor::Yellow, true);
				// DrawDebugLine(
				//               GetWorld(),
				//               GetActorLocation(),
				//               GetActorLocation() + (GetActorRightVector() * 100),
				//               FColor::Yellow,
				//               true
				//              );

			}

		}
	}
}

void ASceneElement_SunShade::SwitchInteractionType(
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

void ASceneElement_SunShade::EntryFocusDevice()
{
	Super::EntryFocusDevice();

	SetActorHiddenInGame(false);

	auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
	if (PrimitiveComponentPtr)
	{
		PrimitiveComponentPtr->SetRenderCustomDepth(true);
		PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	}
}

void ASceneElement_SunShade::EntryViewDevice()
{
	Super::EntryViewDevice();
}

void ASceneElement_SunShade::EntryShowevice()
{
	Super::EntryShowevice();

	SetActorHiddenInGame(false);
	
	UpdateAngle(0);

}

void ASceneElement_SunShade::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();

	SetActorHiddenInGame(false);

	UpdateAngle(-90);

	RemoveRouteMarker();
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

void ASceneElement_SunShade::QuitAllState()
{
	Super::QuitAllState();

	auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
	if (PrimitiveComponentPtr)
	{
		PrimitiveComponentPtr->SetRenderCustomDepth(false);
	}
	
	//
	SetActorHiddenInGame(true);

	UpdateAngle(0);

	RemoveRouteMarker();
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
