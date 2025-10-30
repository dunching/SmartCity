#include "SceneElement_Space.h"

#include "Algorithm.h"
#include "Engine/OverlapResult.h"
#include "Engine/StaticMeshActor.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "DatasmithAssetUserData.h"
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
#include "WebChannelWorldSystem.h"

ASceneElement_Space::ASceneElement_Space(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void ASceneElement_Space::BeginPlay()
{
	Super::BeginPlay();
}

FBox ASceneElement_Space::GetComponentsBoundingBox(
	bool bNonColliding,
	bool bIncludeFromChildActors
	) const
{
	FBox Box(ForceInit);

	for (auto Iter : StaticMeshComponentsAry)
	{
		if (Iter->IsRegistered() && (bNonColliding || Iter->IsCollisionEnabled()))
		{
			Box += Iter->Bounds.GetBox();
		}
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
	const TPair<FName, FString>& InUserData
	)
{
	Super::Merge(ActorRef, InUserData);

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

			NewComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			NewComponentPtr->SetCollisionObjectType(Space_Object);
			NewComponentPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

			NewComponentPtr->SetCollisionResponseToChannel(ExternalWall_Object, ECollisionResponse::ECR_Overlap);
			NewComponentPtr->SetCollisionResponseToChannel(Floor_Object, ECollisionResponse::ECR_Overlap);
			NewComponentPtr->SetCollisionResponseToChannel(Device_Object, ECollisionResponse::ECR_Overlap);

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
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall)
		)
		{
			QuitAllState();

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
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
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
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
		)
		{
			EntryShowEffect(ConditionalSet);

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor)
		)
		{
			QuitAllState();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
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

	for (auto MeshIter : StaticMeshComponentsAry)
	{
		GetWorld()->ComponentOverlapMulti(
		                                  OutOverlap,
		                                  MeshIter,
		                                  FVector::ZeroVector,
		                                  FRotator::ZeroRotator,
		                                  // StaticMeshComponent->GetComponentLocation(),
		                                  // StaticMeshComponent->GetComponentRotation(),
		                                  Params,
		                                  ObjectQueryParams
		                                 );

		for (const auto& Iter : OutOverlap)
		{
			if (Iter.GetActor() && !Iter.GetActor()->IsHidden())
			{
				Result.Add(Cast<ASceneElement_DeviceBase>(Iter.GetActor()));
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

					FeatureWheelPtr->TargetPt = TargetPt.GetCenter();
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

	SwitchColor(FColor::Red);

	for (auto PrimitiveComponentPtr : StaticMeshComponentsAry)
	{
		if (PrimitiveComponentPtr)
		{
			PrimitiveComponentPtr->SetHiddenInGame(true);
			// PrimitiveComponentPtr->SetRenderInMainPass(false);
			PrimitiveComponentPtr->SetRenderCustomDepth(false);
		}
	}

	auto MessageBodySPtr = MakeShared<FMessageBody_SelectedSpace>();

	MessageBodySPtr->SpaceName = Category;

	TSet<ASceneElement_DeviceBase*> ActorsAry = GetAllDevices();
	
	TSet<ASceneElementBase*>TempActorsAry;
	
	USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(TempActorsAry, ConditionalSet);

	for (auto DeviceIter : ActorsAry)
	{
		if (DeviceIter)
		{
			FMessageBody_SelectedSpace::FDeviceInfo DeviceInfo;

			DeviceInfo.DeviceID = DeviceIter->SceneElementID;
			DeviceInfo.Type = DeviceIter->DeviceTypeStr;

			MessageBodySPtr->DeviceIDAry.Add(DeviceInfo);
		}
	}

	UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);

	for (auto Iter : FeatureWheelAry)
	{
		if (Iter)
		{
			Iter->RemoveFromParent();
		}
	}
	FeatureWheelAry.Empty();
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
			}
			break;
		}
	}
}

void ASceneElement_Space::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(true);

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
}

void ASceneElement_Space::SwitchColor(
	const FColor& Color
	)
{
	for (auto MeshIter : StaticMeshComponentsAry)
	{
		const auto Mats = MeshIter->GetMaterials();
		for (auto MatIter : Mats)
		{
			auto MatInst = Cast<UMaterialInstanceDynamic>(MatIter);
			if (MatInst)
			{
				MatInst->SetVectorParameterValue(UAssetRefMap::GetInstance()->SpaceMaterialColorName, Color);
			}
		}
	}
}

void ASceneElement_Space::OnClickedTag()
{
	USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(
		 USmartCitySuiteTags::Interaction_Area_Space,
		 [this](const TSharedPtr<FDecoratorBase>& AreaDecoratorSPtr)
		 {
			 auto SpaceAreaDecoratorSPtr = DynamicCastSharedPtr<FViewSpace_Decorator>(AreaDecoratorSPtr);
		 	if (SpaceAreaDecoratorSPtr)
		 	{
		 		SpaceAreaDecoratorSPtr->Floor = BelongFloor->FloorTag;
		 		SpaceAreaDecoratorSPtr->SceneElementPtr = this;
		 	}
		 }
		);
}
