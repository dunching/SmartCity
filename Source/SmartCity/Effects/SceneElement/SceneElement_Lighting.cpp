#include "SceneElement_Lighting.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "DatasmithAssetUserData.h"
#include "Engine/StaticMeshActor.h"
#include "Components/LocalLightComponent.h"
#include "Engine/RectLight.h"

#include "TourPawn.h"
#include "IPSSI.h"
#include "MessageBody.h"
#include "RouteMarker.h"
#include "SceneElement_PWR_Pipe.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "ViewSingleDeviceProcessor.h"
#include "WebChannelWorldSystem.h"

ASceneElement_Lighting::ASceneElement_Lighting(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void ASceneElement_Lighting::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	UserData = InUserData;

	if (ActorPtr)
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			auto Components
			= STPtr->GetComponents();
			for (auto SecondIter : Components)
			{
				auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
				if (!InterfacePtr)
				{
					continue;
				}
				auto AUDPtr = Cast<UDatasmithAssetUserData>(
															InterfacePtr->GetAssetUserDataOfClass(
																 UDatasmithAssetUserData::StaticClass()
																)
														   );
				if (!AUDPtr)
				{
					continue;
				}
				
				CheckIsJiaCeng(AUDPtr);

				auto ID = AUDPtr->MetaData.Find(TEXT("Element*设备回路编号"));
				if (!ID)
				{
					continue;
				}

				PWR_ID = *ID;
			}
			
			TArray<ULocalLightComponent*> LocalLightComponents;
			GetComponents<ULocalLightComponent>(LocalLightComponents);

			for (auto Iter : LocalLightComponents)
			{
				if (Iter)
				{
					auto Transform = STPtr->GetStaticMeshComponent()->
					                        GetComponentTransform();
					Iter->SetWorldLocation(Transform.GetLocation());
					Iter->SetRelativeRotation(Transform.GetRotation().Rotator() + FRotator(-90, -90, 0));
					Iter->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
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
			NewComponentPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

			NewComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			NewComponentPtr->SetCollisionObjectType(Device_Object);
			NewComponentPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

			NewComponentPtr->SetCollisionResponseToChannel(ExternalWall_Object, ECollisionResponse::ECR_Overlap);
			NewComponentPtr->SetCollisionResponseToChannel(Floor_Object, ECollisionResponse::ECR_Overlap);
			NewComponentPtr->SetCollisionResponseToChannel(Space_Object, ECollisionResponse::ECR_Overlap);

			NewComponentPtr->SetRenderCustomDepth(false);

			StaticMeshComponentsAry.Add(NewComponentPtr);

			RecordOnriginalMat();
		}

		SwitchLight(0);
	}
}

void ASceneElement_Lighting::Merge(
	const TSoftObjectPtr<AActor>& ActorRef,
	const TPair<FName, FString>& InUserData
	)
{
	Super::Merge(ActorRef, InUserData);
}

void ASceneElement_Lighting::SwitchInteractionType(
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

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(true);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			SetEmissiveValue(1);
			SwitchLight(5);

			RevertOnriginalMat();
			
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
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_EnergyManagement);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			SetEmissiveValue(0);
			SwitchLight(0);

			auto DecoratorSPtr =
				DynamicCastSharedPtr<FEnergyMode_Decorator>(
															USceneInteractionWorldSystem::GetInstance()->
															GetDecorator(
																		 USmartCitySuiteTags::Interaction_Mode
																		)
														   );
			if (!DecoratorSPtr)
			{
				return;
			}
			if (!DecoratorSPtr->IDMap.Contains(PWR_ID))
			{
				return;
			}
			
			auto EnergyMaterialInst = UAssetRefMap::GetInstance()->EnergyDeviceMaterialInst.LoadSynchronous();

			auto MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(EnergyMaterialInst, this);

			const auto EnergyValue = DecoratorSPtr->IDMap[PWR_ID]->EnergyValue;
			MaterialInstanceDynamic->SetScalarParameterValue(TEXT("EnergyValue"), EnergyValue);
			for (auto Iter : StaticMeshComponentsAry)
			{
				if (Iter)
				{
					for (int32 Index = 0; Index < Iter->GetNumMaterials(); Index++)
					{
						Iter->SetMaterial(Index, MaterialInstanceDynamic);
					}
				}
			}

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

			RevertOnriginalMat();
			
			SetEmissiveValue(0);
			SwitchLight(0);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_View.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<TourProcessor::FViewSingleDeviceProcessor>(
				 [this](
				 auto NewProcessor
				 )
				 {
					 NewProcessor->TargetDevicePtr = this;
				 }
				);

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

		if (RouteMarkerPtr)
		{
			RouteMarkerPtr->RemoveFromParent();
		}
		RouteMarkerPtr = nullptr;

		return;
	}
}

void ASceneElement_Lighting::SwitchLight(
	int32 Intensity
	)
{
	TArray<ULocalLightComponent*> LocalLightComponents;
	GetComponents<ULocalLightComponent>(LocalLightComponents);

	for (auto Iter : LocalLightComponents)
	{
		if (Iter)
		{
			Iter->Intensity = Intensity;
			if (Intensity <= 0)
			{
				Iter->SetHiddenInGame(true);
			}
			else
			{
				Iter->SetHiddenInGame(false);
			}
		}
	}
}

void ASceneElement_Lighting::SetEmissiveValue(
	int32 Value
	)
{
	for (auto MeshIter : StaticMeshComponentsAry)
	{
		const auto Num = MeshIter->GetNumMaterials();
		for (int32 Index = 0; Index < Num; Index++)
		{
			auto MaterialPtr = UMaterialInstanceDynamic::Create(EmissiveMaterialInstance.LoadSynchronous(), this);
			MeshIter->SetMaterial(Index, MaterialPtr);
		}

		for (int32 Index = 0; Index < Num; Index++)
		{
			auto MaterialPtr = Cast<UMaterialInstanceDynamic>(MeshIter->GetMaterial(Index));
			if (MaterialPtr)
			{
				MaterialPtr->SetScalarParameterValue(EmissiveValue, Value);
			}
		}
	}
}
