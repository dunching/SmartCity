#include "Building_CurtainWall.h"

#include "Components/RectLightComponent.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "DatasmithSceneActor.h"
#include "Dynamic_SkyBase.h"
#include "FloorHelper.h"
#include "SceneElementCategory.h"
#include "SceneElement_RollerBlind.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionDecorator_Area.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "WeatherSystem.h"
#include "Components/BoxComponent.h"
#include "Engine/OverlapResult.h"

ABuilding_CurtainWall::ABuilding_CurtainWall(
	const FObjectInitializer& ObjectInitializer
	):Super(ObjectInitializer)
{

	CollisionComponentHelper = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponentHelper"));
	CollisionComponentHelper->SetupAttachment(RootComponent);

	CollisionComponentHelper->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponentHelper->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComponentHelper->SetCollisionResponseToChannel(Space_Object, ECollisionResponse::ECR_Overlap);
	CollisionComponentHelper->SetCollisionResponseToChannel(Device_Object, ECollisionResponse::ECR_Overlap);
	CollisionComponentHelper->SetCollisionObjectType(Device_Object);
}

void ABuilding_CurtainWall::BeginPlay()
{
	Super::BeginPlay();

	auto Handle = UWeatherSystem::GetInstance()->GetDynamicSky()->OnHourChanged.AddCallback(
		 std::bind(
		           &ThisClass::OnHourChanged,
		           this,
		           std::placeholders::_1
		          )
		);
	Handle->bIsAutoUnregister = false;
}

void ABuilding_CurtainWall::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);

	if (ActorPtr && ActorPtr->IsA(AActor::StaticClass()))
	{
		TArray<UStaticMeshComponent*> Components;
		ActorPtr->GetComponents<UStaticMeshComponent>(Components);
		for (auto Iter : Components)
		{
			if (Iter)
			{
				auto NewComponentPtr = Cast<UStaticMeshComponent>(
				                                                  AddComponentByClass(
					                                                   UStaticMeshComponent::StaticClass(),
					                                                   true,
					                                                   Iter->
					                                                   GetComponentTransform(),
					                                                   false
					                                                  )
				                                                 );
				NewComponentPtr->SetStaticMesh(Iter->GetStaticMesh());
				NewComponentPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

				NewComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				NewComponentPtr->SetRenderCustomDepth(false);

				auto Mats = Iter->GetMaterials();
				for (int32 Index = 0; Index < Mats.Num(); Index++)
				{
					NewComponentPtr->SetMaterial(Index, Mats[Index]);
				}

				StaticMeshComponentsAry.Add(NewComponentPtr);
			}
		}
		UpdateCollisionBox({StaticMeshComponent});

		// 附加到AS
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
			if (!FloorPtr->SceneElementCategoryMap.IsEmpty())
			{
				for (const auto& Iter : FloorPtr->SceneElementCategoryMap)
				{
					if (Iter.Key.MatchesTag(USmartCitySuiteTags::SceneElement_Category_AS))
					{
						AttachToActor(Iter.Value, FAttachmentTransformRules::KeepWorldTransform);
						return;
					}
				}
			}
		}
	}
}

void ABuilding_CurtainWall::SwitchInteractionType(
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
			SwitchState(EState::kOriginal);

			return;
		}
	}
	{
		//  只要是楼层就显示
		if ((ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)))
		{
			// 确认当前的模式
			auto DecoratorSPtr =
				DynamicCastSharedPtr<FInteraction_Decorator>(
				                                             USceneInteractionWorldSystem::GetInstance()->
				                                             GetDecorator(
				                                                          USmartCitySuiteTags::Interaction_Interaction
				                                                         )
				                                            );
			if (DecoratorSPtr)
			{
				const auto ViewConfig = DecoratorSPtr->GetViewConfig();
				if (ViewConfig.CurtainWallTranlucent <= 0)
				{
					SwitchState(EState::kHiden);
				}
				else if (ViewConfig.CurtainWallTranlucent >= 100)
				{
					SwitchState(EState::kOriginal);
				}
				else
				{
			SetActorHiddenInGame(false);

					TArray<UStaticMeshComponent*> Components;
					GetComponents<UStaticMeshComponent>(Components);

					SetTranslucentImp(
					                  Components,
					                  ViewConfig.CurtainWallTranlucent,
					                  UAssetRefMap::GetInstance()->CurtainWallTranslucentMatInst.LoadSynchronous()
					                 );
				}

				return;
			}

			SwitchState(EState::kOriginal);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SwitchState(EState::kHiden);

			return;
		}
	}
}

void ABuilding_CurtainWall::OnHourChanged(
	int32 Hour
	)
{
	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );

	if (!AreaDecoratorSPtr)
	{
		for (auto Iter : RectLightComponentAry)
		{
			Iter->SetHiddenInGame(true);
		}
		return;
	}

	if (!AreaDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Area_ExternalWall))
	{
		for (auto Iter : RectLightComponentAry)
		{
			Iter->SetHiddenInGame(true);
		}
		return;
	}

	if (Hour > 18 || Hour < 8)
	{
		for (auto Iter : RectLightComponentAry)
		{
			Iter->SetHiddenInGame(false);
		}
		return;
	}
	for (auto Iter : RectLightComponentAry)
	{
		Iter->SetHiddenInGame(true);
	}
}

void ABuilding_CurtainWall::OnExternalWall()
{
	const auto Hour = UWeatherSystem::GetInstance()->GetDynamicSky()->GetCurrentHour();

	if (Hour > 18 || Hour < 8)
	{
		for (auto Iter : RectLightComponentAry)
		{
			Iter->SetHiddenInGame(false);
		}
		return;
	}
	for (auto Iter : RectLightComponentAry)
	{
		Iter->SetHiddenInGame(true);
	}
}

void ABuilding_CurtainWall::GenerateRollerBlind(AActor* ActorPtr)
{
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

		if (FloorPtr->FloorTag.MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor_F1))
		{
			return;
		}

		TArray<FOverlapResult> OutOverlap;

		FComponentQueryParams Params;

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(Device_Object);

		auto TempBox = FloorPtr->BoxComponentPtr->GetScaledBoxExtent();
		TempBox.X -= 200;
		TempBox.Y -= 200;
		
		GetWorld()->OverlapMultiByObjectType(
		                                     OutOverlap,
		                                     FloorPtr->BoxComponentPtr->GetComponentLocation(),
		                                     FloorPtr->BoxComponentPtr->GetComponentRotation().Quaternion(),
		                                     ObjectQueryParams,
		                                     FCollisionShape::MakeBox(TempBox),
		                                     Params
		                                    );

		// DrawDebugBox(
		// 	GetWorld(), FloorPtr->BoxComponentPtr->GetComponentLocation(),TempBox,
		// 	FloorPtr->BoxComponentPtr->GetComponentRotation().Quaternion(),
		// 	FColor::Red,
		// 	false,
		// 	50
		// 	);
		
		for (const auto& Iter : OutOverlap)
		{
			auto Building_CurtainWallPtr = Cast<ABuilding_CurtainWall>(Iter.GetActor());
			if (Building_CurtainWallPtr == this)
			{
				return;
			}
		}

		const auto FloorCenter = FloorPtr->BoxComponentPtr->CalcBounds(
		                                                               FloorPtr->BoxComponentPtr->
		                                                               GetComponentToWorld()
		                                                              )
		                                 .GetBox().GetCenter();
		const auto Dir = FloorCenter - GetActorLocation();

		const auto Dot = FVector::DotProduct(Dir, GetActorRightVector());

		FBox Box(ForceInit);
		for (auto Iter : StaticMeshComponentsAry)
		{
			Iter->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			FBox TemoBox(ForceInit);
			TemoBox.IsValid = true;
			Iter->GetLocalBounds(TemoBox.Min, TemoBox.Max);
			TemoBox = TemoBox.TransformBy(Iter->GetRelativeTransform());
			Box += TemoBox;
		}
		if (!UserData.Contains( TEXT("Datasmith_UniqueId")))
		{
			return;
		}
			
		auto SceneElement_RollerBlindPtr = GetWorld()->SpawnActor<ASceneElement_RollerBlind>(
			 UAssetRefMap::GetInstance()->SceneElement_RollerBlindClass
			);
		if (SceneElement_RollerBlindPtr)
		{
			SceneElement_RollerBlindPtr->SceneElementID = UserData[TEXT("Datasmith_UniqueId")] + TEXT("窗帘");
			SceneElement_RollerBlindPtr->DeviceTypeStr = TEXT("窗帘");
			SceneElement_RollerBlindPtr->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

			USceneInteractionWorldSystem::GetInstance()->SetSceneActor(SceneElement_RollerBlindPtr->SceneElementID, SceneElement_RollerBlindPtr);

			const auto Size = Box.GetSize();
			if (Dot > 0)
			{
				SceneElement_RollerBlindPtr->
					SetActorRelativeLocation(Box.GetCenter() - FVector(0, -20, Size.Z / 2));
			}
			else
			{
				SceneElement_RollerBlindPtr->SetActorRelativeLocation(Box.GetCenter() - FVector(0, 20, Size.Z / 2));
			}

			SceneElement_RollerBlindPtr->SetActorScale3D(
			                                             FVector(
			                                                     Size.X / SceneElement_RollerBlindPtr->DefaultSize.
			                                                     X,
			                                                     1,
			                                                     (Size.Z - 40) / SceneElement_RollerBlindPtr->
			                                                     DefaultSize.Z
			                                                    )
			                                            );
		}
	}
}


void ABuilding_CurtainWall::UpdateCollisionBox(
	const TArray<UStaticMeshComponent*>& SMCompsAry
	)
{
	FBox Box(ForceInit);
	for (auto Iter : SMCompsAry)
	{
		Iter->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		FBox TemoBox(ForceInit);
		TemoBox.IsValid = true;
		Iter->GetLocalBounds(TemoBox.Min, TemoBox.Max);
		TemoBox = TemoBox.TransformBy(Iter->GetRelativeTransform());
		
		Box += TemoBox;
	}

	CollisionComponentHelper->SetRelativeLocation(Box.GetCenter());

	CollisionComponentHelper->SetBoxExtent(Box.GetExtent());
}
