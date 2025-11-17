#include "SceneElement_Lighting.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "DatasmithAssetUserData.h"
#include "Dynamic_SkyBase.h"
#include "Engine/StaticMeshActor.h"
#include "Components/LocalLightComponent.h"
#include "Engine/RectLight.h"
#include "Kismet/KismetStringLibrary.h"

#include "TourPawn.h"
#include "IPSSI.h"
#include "MessageBody.h"
#include "RouteMarker.h"
#include "SceneElement_PWR_Pipe.h"
#include "SceneInteractionDecorator_Area.h"
#include "SceneInteractionDecorator_Mode.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "ViewSingleDeviceProcessor.h"
#include "WeatherSystem.h"
#include "WebChannelWorldSystem.h"

ASceneElement_Lighting::ASceneElement_Lighting(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void ASceneElement_Lighting::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneElement_Lighting::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);

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

			UpdateCollisionBox({NewComponentPtr});
		}

		// SetEmissiveValue(0, -1, FColor::White);
		SwitchLight(0, -1, FColor::White);
	}
}

void ASceneElement_Lighting::Merge(
	const TSoftObjectPtr<AActor>& ActorRef,
	const TPair<FName, FString>& InUserData,
	const TMap<FName, FString>& NewUserData
	)
{
	Super::Merge(ActorRef, InUserData, NewUserData);
}

void ASceneElement_Lighting::SwitchInteractionType(
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
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_EnergyManagement)
		)
		{
			SetActorHiddenInGame(false);

			// SetEmissiveValue(0, -1, FColor::White);
			SwitchLight(0, -1, FColor::White);

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

			CacheOriginalMat(StaticMeshComponentsAry);
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
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
		)
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
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
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_View)
		)
		{
			EntryViewDevice();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_Focus)
		)
		{
			EntryShowDevice();

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

void ASceneElement_Lighting::EntryFocusDevice()
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

void ASceneElement_Lighting::QuitFocusDevice()
{
	Super::QuitFocusDevice();
}

void ASceneElement_Lighting::EntryViewDevice()
{
	Super::EntryViewDevice();
}

void ASceneElement_Lighting::QuitViewDevice()
{
	Super::QuitViewDevice();
}

void ASceneElement_Lighting::EntryShowDevice()
{
	Super::EntryShowDevice();

	SetActorHiddenInGame(false);

	// SetEmissiveValue(0, -1, FColor::White);
	SwitchLight(0, -1, FColor::White);

	RevertOnriginalMat();
}

void ASceneElement_Lighting::QuitShowDevice()
{
	Super::QuitShowDevice();
}

void ASceneElement_Lighting::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();

	SetActorHiddenInGame(false);

	if (ExtensionParamMap.Contains(TEXT("开关")))
	{
		const auto Value = UKismetStringLibrary::Conv_StringToInt(ExtensionParamMap[TEXT("开关")]);
		if (Value)
		{
			if (ExtensionParamMap.Contains(TEXT("亮度")))
			{
				FLinearColor LightColor = FColor::White;
				bool OutIsValid = false;
				if (ExtensionParamMap.Contains(TEXT("LightColor")))
				{
					UKismetStringLibrary::Conv_StringToColor(
					                                         ExtensionParamMap[TEXT("LightColor")],
					                                         LightColor,
					                                         OutIsValid
					                                        );
				}

				int32 Temperature = -1;
				if (ExtensionParamMap.Contains(TEXT("色温")))
				{
					Temperature = UKismetStringLibrary::Conv_StringToInt(ExtensionParamMap[TEXT("色温")]);
				}

				const auto IntensityValue =
					UKismetStringLibrary::Conv_StringToInt(ExtensionParamMap[TEXT("亮度")]);
				SetEmissiveValue(IntensityValue, Temperature, LightColor);
				SwitchLight(IntensityValue, Temperature, LightColor);

				return;
			}
		}
		else
		{
			SetEmissiveValue(0, -1, FColor::White);
			SwitchLight(0, -1, FColor::White);
		}
		return;
	}

	SetEmissiveValue(0, -1, FColor::White);
	SwitchLight(0, -1, FColor::White);
}

void ASceneElement_Lighting::QuitShowDeviceEffect()
{
	Super::QuitShowDeviceEffect();
}

void ASceneElement_Lighting::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(true);

	RevertOnriginalMat();

	if (RouteMarkerPtr)
	{
		RouteMarkerPtr->RemoveFromParent();
	}
	RouteMarkerPtr = nullptr;
}

void ASceneElement_Lighting::SwitchLight(
	int32 Intensity,
	int32 Temperature,
	const FLinearColor& LightColor
	)
{
	TArray<ULocalLightComponent*> LocalLightComponents;
	GetComponents<ULocalLightComponent>(LocalLightComponents);

	for (auto Iter : LocalLightComponents)
	{
		if (Iter)
		{
			Iter->SetIntensity(FMath::Lerp(0, 10, Intensity / 100.f));
			// Iter->LightColor = LightColor.ToRGBE();
			if (Intensity <= 0)
			{
				Iter->SetHiddenInGame(true);
			}
			else
			{
				Iter->SetHiddenInGame(false);
			}

			if (Temperature <= 0)
			{
				Iter->SetUseTemperature(false);
			}
			else
			{
				Iter->SetTemperature(FMath::Lerp(1700, 12000, Temperature / 100.f));
				Iter->SetUseTemperature(true);
			}
		}
	}
}

void ASceneElement_Lighting::SetEmissiveValue(
	int32 Value,
	int32 Temperature,
	const FLinearColor& LightColor
	)
{
	if (Value <= 0)
	{
	RevertOnriginalMat();
	}
	else
	{

		CacheOriginalMat(StaticMeshComponentsAry);
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
					MaterialPtr->SetVectorParameterValue(Color, LightColor);
				}
			}
		}
	}
}
