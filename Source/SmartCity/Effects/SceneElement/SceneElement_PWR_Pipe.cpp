#include "SceneElement_PWR_Pipe.h"

#include "Algorithm.h"
#include "Engine/OverlapResult.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetStringLibrary.h"
#include "Components/BoxComponent.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "DatasmithAssetUserData.h"
#include "FeatureWheel.h"
#include "GameplayTagsLibrary.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "SceneElement_DeviceBase.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"

ASceneElement_PWR_Pipe::ASceneElement_PWR_Pipe(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	CollisionComponentHelper->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASceneElement_PWR_Pipe::BeginPlay()
{
	Super::BeginPlay();
}

FBox ASceneElement_PWR_Pipe::GetComponentsBoundingBox(
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

void ASceneElement_PWR_Pipe::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);

}

void ASceneElement_PWR_Pipe::Merge(
	const TSoftObjectPtr<AActor>& ActorRef,
	const TPair<FName, FString>& InUserData
	, const TMap<FName, FString>& NewUserData
	)
{
	Super::Merge(ActorRef, UserData, NewUserData);

	UserData = InUserData;

	if (ActorRef.ToSoftObjectPath().IsValid())
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorRef.Get());
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

			auto Transform =
				STPtr->GetStaticMeshComponent()->
				       GetComponentTransform();
			
			auto NewComponentPtr = Cast<UStaticMeshComponent>(
			                                                  AddComponentByClass(
				                                                   UStaticMeshComponent::StaticClass(),
				                                                   true,
				                                                   Transform,
				                                                   false
				                                                  )
			                                                 );

			NewComponentPtr->AddAssetUserData(AUDPtr);

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
		}

		ActorRef->Destroy();
	}
}

void ASceneElement_PWR_Pipe::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	if (OriginalMaterials.IsEmpty())
	{
		for (auto Iter : StaticMeshComponentsAry)
		{
			if (Iter)
			{
				FMaterialsCache MaterialsCache;
				for (int32 Index = 0; Index < Iter->GetNumMaterials(); Index++)
				{
					MaterialsCache.MaterialsCacheAry.Add(Iter->GetMaterial(Index));
				}
				OriginalMaterials.Add(Iter, MaterialsCache);
			}
		}
	}

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
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_EnergyManagement);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			EntryShoweviceEffect();
			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR)
		)
		{
			EntryShoweviceEffect();
			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_View)
		)
		{
			EntryViewDevice();
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
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			EntryShowDevice();
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_Focus.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
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

FString ASceneElement_PWR_Pipe::GetID() const
{
	if (
		(UserData.Key == TEXT("Element*照明回路编号")) ||
		(UserData.Key == TEXT("Element*空调和新风回路编号"))
	)
	{
		return UserData.Value;
	}
	if (
		(UserData.Key == TEXT("Element*管线类型编号"))
	)
	{
		return UserData.Value;
	}

	return TEXT("");
}

void ASceneElement_PWR_Pipe::EntryFocusDevice()
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

void ASceneElement_PWR_Pipe::EntryViewDevice()
{
	Super::EntryViewDevice();
	SetActorHiddenInGame(false);
	RevertOnriginalMat();
}

void ASceneElement_PWR_Pipe::EntryShowDevice()
{
	Super::EntryShowDevice();

	SetActorHiddenInGame(false);

	for (auto Iter : StaticMeshComponentsAry)
	{
		if (Iter)
		{
			if (OriginalMaterials.Contains(Iter))
			{
				auto& Ref = OriginalMaterials[Iter];
				if (Ref.MaterialsCacheAry.Num() >= Iter->GetNumMaterials())
				{
					for (int32 Index = 0; Index < Iter->GetNumMaterials(); Index++)
					{
						Iter->SetMaterial(Index, Ref.MaterialsCacheAry[Index]);
					}
				}
			}
		}
	}
}

void ASceneElement_PWR_Pipe::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();
	if (
		(UserData.Key == TEXT("Element*照明回路编号")) ||
		(UserData.Key == TEXT("Element*管线类型编号")) ||
		(UserData.Key == TEXT("Element*空调和新风回路编号"))
	)
	{
		SetActorHiddenInGame(false);
		CacheOriginalMat(StaticMeshComponentsAry);

		auto EnergyMaterialInst = UAssetRefMap::GetInstance()->EnergyPipeMaterialInst.LoadSynchronous();

		auto MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(EnergyMaterialInst, this);

		EnergyValue = FMath::RandRange(0.f, 1.f);
		
		if (ExtensionParamMap.Contains(TEXT("Intensity")))
		{
			EnergyValue = UKismetStringLibrary::Conv_StringToInt(ExtensionParamMap[TEXT("Intensity")]);
		}
		
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
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}

void ASceneElement_PWR_Pipe::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(true);

	RevertOnriginalMat();
}
