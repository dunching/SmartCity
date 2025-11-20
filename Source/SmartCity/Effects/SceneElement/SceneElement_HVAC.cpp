#include "SceneElement_HVAC.h"

#include "Engine/StaticMeshActor.h"
#include "ActorSequenceComponent.h"
#include "AssetRefMap.h"
#include "NiagaraComponent.h"
#include "DatasmithAssetUserData.h"
#include "Kismet/KismetStringLibrary.h"

#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "IPSSI.h"
#include "MessageBody.h"
#include "SceneElement_PWR_Pipe.h"
#include "SceneInteractionDecorator_Mode.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "ViewSingleDeviceProcessor.h"
#include "TourPawn.h"
#include "WebChannelWorldSystem.h"

ASceneElement_HVAC::ASceneElement_HVAC(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMeshComponent->SetupAttachment(RelativeTransformComponent);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComponent->SetCollisionObjectType(Device_Object);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	NiagaraComponentPtr = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponentPtr->SetupAttachment(RelativeTransformComponent);

	NiagaraComponentPtr->SetAutoActivate(false);
}

void ASceneElement_HVAC::BeginPlay()
{
	Super::BeginPlay();


	NiagaraComponentPtr->SetBoolParameter(TEXT("DrawBounds"), false);
}

void ASceneElement_HVAC::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	if (ProcessJiaCengLogic(ConditionalSet))
	{
		QuitAllState();

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
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_HVAC)
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

			NiagaraComponentPtr->SetActive(false);

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

			CacheOriginalMat({StaticMeshComponent});
			if (StaticMeshComponent)
			{
				for (int32 Index = 0; Index < StaticMeshComponent->GetNumMaterials(); Index++)
				{
					StaticMeshComponent->SetMaterial(Index, MaterialInstanceDynamic);
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
			EntryShowDevice();

			return;
		}
	}
	{
		if ((ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)))
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

void ASceneElement_HVAC::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);

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

		auto ID = AUDPtr->MetaData.Find(TEXT("Element*设备回路编号"));
		if (ID)
		{
			PWR_ID = *ID;
		}

		StaticMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());

		for (int32 Index = 0; Index < STPtr->GetStaticMeshComponent()->GetNumMaterials(); Index++)
		{
			StaticMeshComponent->SetMaterial(Index, STPtr->GetStaticMeshComponent()->GetMaterial(Index));
		}

		UpdateCollisionBox({StaticMeshComponent});
	}
}

void ASceneElement_HVAC::EntryFocusDevice()
{
	Super::EntryFocusDevice();

	SetActorHiddenInGame(false);

	RevertOnriginalMat();

	auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
	if (PrimitiveComponentPtr)
	{
		PrimitiveComponentPtr->SetRenderCustomDepth(true);
		PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	}

	auto MessageBodySPtr = MakeShared<FMessageBody_ViewDevice>();

	MessageBodySPtr->DeviceID = SceneElementID;
	MessageBodySPtr->Type = DeviceTypeStr;

	UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);
}

void ASceneElement_HVAC::QuitFocusDevice()
{
	Super::QuitFocusDevice();
}

void ASceneElement_HVAC::EntryViewDevice()
{
	Super::EntryViewDevice();

	RevertOnriginalMat();
}

void ASceneElement_HVAC::QuitViewDevice()
{
	Super::QuitViewDevice();
}

void ASceneElement_HVAC::EntryShowDevice()
{
	Super::EntryShowDevice();

	SetActorHiddenInGame(false);

	RevertOnriginalMat();

	NiagaraComponentPtr->SetActive(false);
}

void ASceneElement_HVAC::QuitShowDevice()
{
	Super::QuitShowDevice();
}

void ASceneElement_HVAC::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();

	SetActorHiddenInGame(false);

	NiagaraComponentPtr->SetActive(true);

	if (ExtensionParamMap.Contains(TEXT("开关")))
	{
		const auto Value = static_cast<bool>(UKismetStringLibrary::Conv_StringToInt(ExtensionParamMap[TEXT("开关")]));

		NiagaraComponentPtr->SetActive(Value);
		if (Value)
		{
			if (ExtensionParamMap.Contains(TEXT("Looping")))
			{
			}

			if (ExtensionParamMap.Contains(TEXT("Pattern")))
			{
			}

			if (ExtensionParamMap.Contains(TEXT("风速")))
			{
				const auto WindSpeed = UKismetStringLibrary::Conv_StringToInt(ExtensionParamMap[TEXT("风速")]);
				switch (WindSpeed)
				{
				case 0:
					{
						NiagaraComponentPtr->SetFloatParameter(TEXT("Emessive"), 25);
					}
					break;
				case 1:
					{
						NiagaraComponentPtr->SetFloatParameter(TEXT("Emessive"), 50);
					}
					break;
				case 2:
					{
						NiagaraComponentPtr->SetFloatParameter(TEXT("Emessive"), 75);
					}
					break;
				case 3:
					{
						NiagaraComponentPtr->SetFloatParameter(TEXT("Emessive"), 100);
					}
					break;
				default:
					{
						NiagaraComponentPtr->SetFloatParameter(TEXT("Emessive"), 25);
					}
				}

				return;
			}

			if (ExtensionParamMap.Contains(TEXT("Temperature")))
			{
				NiagaraComponentPtr->SetColorParameter(TEXT("Color_A"), FColor::Blue);
			}
		}

		return;
	}

	EntryShowDevice();
}

void ASceneElement_HVAC::QuitShowDeviceEffect()
{
	Super::QuitShowDeviceEffect();

	NiagaraComponentPtr->SetActive(false);
}

void ASceneElement_HVAC::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(true);

	RevertOnriginalMat();

	auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
	if (PrimitiveComponentPtr)
	{
		PrimitiveComponentPtr->SetRenderCustomDepth(false);
	}
}
