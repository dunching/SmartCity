#include "SceneElement_Space.h"

#include "Algorithm.h"
#include "Engine/OverlapResult.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "DatasmithAssetUserData.h"
#include "Dynamic_SkyBase.h"
#include "FeatureWheel.h"
#include "FloorHelper.h"
#include "GameplayTagsLibrary.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "MessageBody.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "RouteMarker.h"
#include "SceneElement_DeviceBase.h"
#include "SceneInteractionDecorator_Area.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "WeatherSystem.h"
#include "WebChannelWorldSystem.h"
#include "Components/PointLightComponent.h"

ASceneElement_Space::ASceneElement_Space(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void ASceneElement_Space::BeginPlay()
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

FBox ASceneElement_Space::GetComponentsBoundingBox(
	bool bNonColliding,
	bool bIncludeFromChildActors
	) const
{
	FBox Box(ForceInit);

	for (auto Iter : CollisionComponentsAry)
	{
			Box += Iter->Bounds.GetBox();
	}

	return Box;
}

void ASceneElement_Space::InitialSceneElement()
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

void ASceneElement_Space::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
}

void ASceneElement_Space::Merge(
	const TSoftObjectPtr<AActor>& ActorRef,
	const TPair<FName, FString>& InUserData,
	const TMap<FName, FString>& NewUserData
	)
{
	Super::Merge(ActorRef, InUserData, NewUserData);

	if (ActorRef.ToSoftObjectPath().IsValid())
	{
		AActor* ParentPtr = ActorRef->GetAttachParentActor();
		if (ParentPtr && !GetAttachParentActor())
		{
			AttachToActor(ParentPtr, FAttachmentTransformRules::KeepRelativeTransform);
			SetActorRelativeTransform(FTransform::Identity);
		}

		auto STPtr = Cast<AStaticMeshActor>(ActorRef.Get());
		if (STPtr)
		{
			TArray<UStaticMeshComponent*> Components;
			STPtr->GetComponents<UStaticMeshComponent>(Components);
			for (auto SecondIter : Components)
			{
				auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
				if (InterfacePtr)
				{
					auto AUDPtr = Cast<UDatasmithAssetUserData>(
					                                            InterfacePtr->GetAssetUserDataOfClass(
						                                             UDatasmithAssetUserData::StaticClass()
						                                            )
					                                           );
					if (!AUDPtr)
					{
						continue;
					}
					for (const auto& ThirdIter : AUDPtr->MetaData)
					{
						if (ThirdIter.Key == DataSmith_Key)
						{
							Category = ThirdIter.Value;
							break;
						}
					}
					break;
				}
			}

			auto NewComponentPtr = Cast<UStaticMeshComponent>(
			                                                  AddComponentByClass(
				                                                   UStaticMeshComponent::StaticClass(),
				                                                   true,
				                                                   STPtr->GetStaticMeshComponent()->
				                                                          GetComponentTransform(),
				                                                   false
				                                                  )
			                                                 );

			NewComponentPtr->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());
			NewComponentPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

			NewComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			NewComponentPtr->SetRenderCustomDepth(false);

			auto SpaceMaterialInstance = UAssetRefMap::GetInstance()->SpaceMaterialInstance;

			auto MatInst = UMaterialInstanceDynamic::Create(SpaceMaterialInstance.LoadSynchronous(), this);

			for (int32 Index = 0; Index < NewComponentPtr->GetNumMaterials(); Index++)
			{
				NewComponentPtr->SetMaterial(Index, MatInst);
			}

			NewComponentPtr->SetCastShadow(false);
			NewComponentPtr->bVisibleInReflectionCaptures = false;
			NewComponentPtr->bVisibleInRealTimeSkyCaptures = false;
			NewComponentPtr->bVisibleInRayTracing = false;
			NewComponentPtr->bReceivesDecals = false;
			NewComponentPtr->bUseAsOccluder = false;

			StaticMeshComponentsAry.Add(NewComponentPtr);

			auto CollisionComponentPtr = Cast<UBoxComponent>(
			                                                 AddComponentByClass(
				                                                  UBoxComponent::StaticClass(),
				                                                  true,
				                                                  STPtr->GetStaticMeshComponent()->
				                                                         GetComponentTransform(),
				                                                  false
				                                                 )
			                                                );

			CollisionComponentPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

			CollisionComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			CollisionComponentPtr->SetCollisionObjectType(Space_Object);
			CollisionComponentPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

			CollisionComponentPtr->SetCollisionResponseToChannel(ExternalWall_Object, ECollisionResponse::ECR_Overlap);
			CollisionComponentPtr->SetCollisionResponseToChannel(Floor_Object, ECollisionResponse::ECR_Overlap);
			CollisionComponentPtr->SetCollisionResponseToChannel(Device_Object, ECollisionResponse::ECR_Overlap);

			FBox Box(ForceInit);
			STPtr->GetStaticMeshComponent()->GetLocalBounds(Box.Min, Box.Max);

			CollisionComponentPtr->SetRelativeLocation(Box.GetCenter());

			CollisionComponentPtr->SetBoxExtent(Box.GetExtent());

			CollisionComponentPtr->SetRenderCustomDepth(false);

			CollisionComponentsAry.Add(CollisionComponentPtr);

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

			if (!FloorPtr->FloorTag.MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor_Roof))
			{
				auto LightComponentPtr = Cast<UPointLightComponent>(
																	AddComponentByClass(
																		 UPointLightComponent::StaticClass(),
																		 true,
																		 STPtr->GetStaticMeshComponent()->
																				GetComponentTransform(),
																		 false
																		)
																   );

				LightComponentPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

				LightComponentPtr->SetRelativeLocation(Box.GetCenter());

				LightComponentPtr->bUseInverseSquaredFalloff = false;
				LightComponentPtr->IntensityUnits = ELightUnits::Lumens;
				const auto Size = Box.GetSize().Size();
				LightComponentPtr->SourceRadius = Size;
				LightComponentPtr->SoftSourceRadius = Size;
				LightComponentPtr->SourceLength = Size;
				LightComponentPtr->Intensity = 8;

				LightComponentPtr->SetRelativeLocation(Box.GetCenter());

				RectLightComponentAry.Add(LightComponentPtr);
			}
		}

		ActorRef->Destroy();
	}
}

void ASceneElement_Space::SwitchInteractionType(
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
			OnExternalWall();
			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_View)
		)
		{
			EntryViewDevice(ConditionalSet);
			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_Focus)
		)
		{
			EntryFocusDevice(ConditionalSet);
			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_BatchControl)
		)
		{
			EntryShowEffect(ConditionalSet);

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Interaction) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
		)
		{
			EntryShowEffect(ConditionalSet);
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
			EntryShowEffect(ConditionalSet);

			return;
		}
	}
	{
		if ((ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)))
		{
			QuitAllState();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Interaction)
		)
		{
			QuitAllState();

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

TSharedPtr<FJsonValue> ASceneElement_Space::GetSceneElementData() const
{
	auto Result = Super::GetSceneElementData();

	auto RootJsonObj = Result->AsObject();

	RootJsonObj->SetStringField(
	                            TEXT("DataSmith_Key"),
	                            DataSmith_Key
	                           );

	RootJsonObj->SetStringField(
	                            TEXT("Category"),
	                            Category
	                           );

	return Result;
}

void ASceneElement_Space::SetFeatures(
	const TArray<FString>& InFeaturesAry
	)
{
	FeaturesAry = InFeaturesAry;
}

TSet<ASceneElement_DeviceBase*> ASceneElement_Space::GetAllDevices() const
{
	TSet<ASceneElement_DeviceBase*> Result;

	TArray<FOverlapResult> OutOverlap;

	FComponentQueryParams Params;

	// Params.bTraceComplex = true;

#if !(UE_BUILD_TEST || UE_BUILD_SHIPPING)
	Params.bDebugQuery = true;
#endif

	FCollisionObjectQueryParams ObjectQueryParams;

	ObjectQueryParams.AddObjectTypesToQuery(Device_Object);

	for (auto MeshIter : CollisionComponentsAry)
	{
		const auto Collision = MeshIter->GetCollisionEnabled();
		MeshIter->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetWorld()->ComponentOverlapMulti(
		                                  OutOverlap,
		                                  MeshIter,
		                                  // FVector::ZeroVector,
		                                  // FRotator::ZeroRotator,
		                                  MeshIter->GetComponentLocation(),
		                                  MeshIter->GetComponentRotation(),
		                                  Params,
		                                  ObjectQueryParams
		                                 );
		MeshIter->SetCollisionEnabled(Collision);

		for (const auto& Iter : OutOverlap)
		{
			if (Iter.GetActor() && !Iter.GetActor()->IsHidden())
			{
				auto DevicePtr = Cast<ASceneElement_DeviceBase>(Iter.GetActor());
				if (DevicePtr)
				{
					Result.Add(DevicePtr);
				}
			}
		}
	}

	return Result;
}

void ASceneElement_Space::EntryViewDevice(
	const FSceneElementConditional& ConditionalSet
	)
{
	SetActorHiddenInGame(false);

	for (auto Iter : FeatureWheelAry)
	{
		if (Iter)
		{
			Iter->RemoveFromParent();
		}
	}

	FeatureWheelAry.Empty();

	for (auto PrimitiveComponentPtr : StaticMeshComponentsAry)
	{
		if (PrimitiveComponentPtr)
		{
			PrimitiveComponentPtr->SetHiddenInGame(false);
			PrimitiveComponentPtr->SetRenderInMainPass(false);
			PrimitiveComponentPtr->SetRenderCustomDepth(true);
			PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
		}
	}
	
	for (auto Iter : CollisionComponentsAry)
	{
		Iter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASceneElement_Space::EntryFocusDevice(
	const FSceneElementConditional& ConditionalSet
	)
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
		switch (DecoratorSPtr->GetInteractionType())
		{
		case EInteractionType::kDevice:
			{
				SetActorHiddenInGame(true);

				for (auto Iter : FeatureWheelAry)
				{
					if (Iter)
					{
						Iter->RemoveFromParent();
					}
				}
				FeatureWheelAry.Empty();
			}
			break;
		case EInteractionType::kSpace:
			{
				SetActorHiddenInGame(true);

				SwitchColor(FColor::Red);

				auto FeatureWheelPtr = CreateWidget<UFeatureWheel>(
				                                                   GetWorld(),
				                                                   UAssetRefMap::GetInstance()->FeatureWheelClass
				                                                  );
				if (FeatureWheelPtr)
				{
					auto TargetPt = UKismetAlgorithm::GetActorBox(
					                                              {this}
					                                             );

					TArray<FFeaturesItem> Features;
					for (const auto& Iter : FeaturesAry)
					{
						Features.Add({Iter, nullptr});
					}

					FeatureWheelPtr->TargetPt = TargetPt.GetCenter() + FVector(0,0,TargetPt.GetExtent().Z);
					FeatureWheelPtr->InitalFeaturesItem(Category, Features);

					FeatureWheelPtr->AddToViewport();

					FeatureWheelAry.Add(FeatureWheelPtr);
				}

				for (auto PrimitiveComponentPtr : StaticMeshComponentsAry)
				{
					if (PrimitiveComponentPtr)
					{
						PrimitiveComponentPtr->SetHiddenInGame(false);
						// PrimitiveComponentPtr->SetRenderInMainPass(true);
						PrimitiveComponentPtr->SetRenderCustomDepth(true);
						PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
					}
				}
	
				for (auto Iter : CollisionComponentsAry)
				{
					Iter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}
			}
			break;
		}
	}
}

void ASceneElement_Space::EntryShow(
	const FSceneElementConditional& ConditionalSet
	)
{
	SetActorHiddenInGame(true);

	for (auto PrimitiveComponentPtr : StaticMeshComponentsAry)
	{
		if (PrimitiveComponentPtr)
		{
			PrimitiveComponentPtr->SetHiddenInGame(false);
			PrimitiveComponentPtr->SetRenderInMainPass(false);
			PrimitiveComponentPtr->SetRenderCustomDepth(false);
		}
	}
	
	for (auto Iter : CollisionComponentsAry)
	{
		Iter->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void ASceneElement_Space::EntryShowEffect(
	const FSceneElementConditional& ConditionalSet
	)
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
		switch (DecoratorSPtr->GetInteractionType())
		{
		case EInteractionType::kDevice:
			{
				SetActorHiddenInGame(true);

				for (auto Iter : FeatureWheelAry)
				{
					if (Iter)
					{
						Iter->RemoveFromParent();
					}
				}
				FeatureWheelAry.Empty();
			}
			break;
		case EInteractionType::kSpace:
			{
				SetActorHiddenInGame(false);

				SwitchColor(FColor::White);

				auto FeatureWheelPtr = CreateWidget<UFeatureWheel>(
				                                                   GetWorld(),
				                                                   UAssetRefMap::GetInstance()->FeatureWheelClass
				                                                  );
				if (FeatureWheelPtr)
				{
					auto TargetPt = UKismetAlgorithm::GetActorBox(
					                                              {this}
					                                             );

					TArray<FFeaturesItem> Features;
					for (const auto& Iter : FeaturesAry)
					{
						Features.Add({Iter, nullptr});
					}

					FeatureWheelPtr->TargetPt = TargetPt.GetCenter() + FVector(0, 0, TargetPt.GetExtent().Z);
					FeatureWheelPtr->InitalFeaturesItem(Category, Features);

					FOnButtonClickedEvent OnClickedDelegate;

					OnClickedDelegate.AddDynamic(this, &ThisClass::OnClickedTag);

					FeatureWheelPtr->SetOncliced(OnClickedDelegate);

					FeatureWheelPtr->AddToViewport();

					FeatureWheelAry.Add(FeatureWheelPtr);
				}

				for (auto PrimitiveComponentPtr : StaticMeshComponentsAry)
				{
					if (PrimitiveComponentPtr)
					{
						PrimitiveComponentPtr->SetHiddenInGame(false);
						PrimitiveComponentPtr->SetRenderInMainPass(false);
						PrimitiveComponentPtr->SetRenderCustomDepth(false);
					}
				}
	
				for (auto Iter : CollisionComponentsAry)
				{
					Iter->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				}
			}
			break;
		}
	}
}

void ASceneElement_Space::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(false);

	USceneInteractionWorldSystem::GetInstance()->ClearFocus();

	for (auto Iter : FeatureWheelAry)
	{
		if (Iter)
		{
			Iter->RemoveFromParent();
		}
	}
	FeatureWheelAry.Empty();

	for (auto PrimitiveComponentPtr : StaticMeshComponentsAry)
	{
		if (PrimitiveComponentPtr)
		{
			PrimitiveComponentPtr->SetHiddenInGame(true);
			PrimitiveComponentPtr->SetRenderCustomDepth(false);
		}
	}
	
	for (auto Iter : CollisionComponentsAry)
	{
		Iter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASceneElement_Space::SwitchColor(
	const FColor& Color
	)
{
}

void ASceneElement_Space::OnClickedTag()
{
	USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(
	                                                                   USmartCitySuiteTags::Interaction_Area_Space,
	                                                                   [this](
	                                                                   const TSharedPtr<FDecoratorBase>&
	                                                                   AreaDecoratorSPtr
	                                                                   )
	                                                                   {
		                                                                   auto SpaceAreaDecoratorSPtr =
			                                                                   DynamicCastSharedPtr<
				                                                                   FViewSpace_Decorator>(
				                                                                    AreaDecoratorSPtr
				                                                                   );
		                                                                   if (SpaceAreaDecoratorSPtr)
		                                                                   {
			                                                                   SpaceAreaDecoratorSPtr->Floor =
				                                                                   BelongFloor->FloorTag;
			                                                                   SpaceAreaDecoratorSPtr->SceneElementPtr =
				                                                                   this;
		                                                                   }
	                                                                   }
	                                                                  );
}

void ASceneElement_Space::OnHourChanged(
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

	if (
		AreaDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Area_ExternalWall) ||
		AreaDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Area_Periphery) 
		)
	{
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
			DrawDebugSphere(GetWorld(), Iter->GetComponentLocation(), 20, 20, FColor::Red, false, 10);
		}
		return;
	}
	
	for (auto Iter : RectLightComponentAry)
	{
		Iter->SetHiddenInGame(true);
	}
}

void ASceneElement_Space::OnExternalWall()
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
		DrawDebugSphere(GetWorld(), Iter->GetComponentLocation(), 20, 20, FColor::Red, false, 10);
	}
}
