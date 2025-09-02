#include "SceneElement_PWR_Pipe.h"

#include "Algorithm.h"
#include "Engine/OverlapResult.h"
#include "Engine/StaticMeshActor.h"

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
	):
	 Super(ObjectInitializer)
{
}

void ASceneElement_PWR_Pipe::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneElement_PWR_Pipe::ReplaceImp(
	AActor* ActorPtr
	)
{
}

void ASceneElement_PWR_Pipe::Merge(
	const TSoftObjectPtr<AActor>& ActorRef
	)
{
	Super::Merge(ActorRef);

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
			FMaterialsCache MaterialsCache;
			for (int32 Index = 0; Index < Iter->GetNumMaterials(); Index++)
			{
				MaterialsCache.MaterialsCacheAry.Add(Iter->GetMaterial(Index));
			}
			OriginalMaterials.Add(Iter, MaterialsCache);
		}
	}

	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall);
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(true);
			
			for (auto Iter : StaticMeshComponentsAry)
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
			
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Energy);
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);
			
			auto EnergyMaterialInst = UAssetRefMap::GetInstance()->EnergyMaterialInst.LoadSynchronous();

			auto MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(EnergyMaterialInst, this);

			EnergyValue = FMath::RandRange(0.f, 1.f);
			MaterialInstanceDynamic->SetScalarParameterValue(TEXT("EnergyValue"), EnergyValue);
			for (auto Iter : StaticMeshComponentsAry)
			{
				for (int32 Index = 0; Index < Iter->GetNumMaterials(); Index++)
				{
					Iter->SetMaterial(Index, MaterialInstanceDynamic);
				}
			}
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);
			
			for (auto Iter : StaticMeshComponentsAry)
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
			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}
		SetActorHiddenInGame(true);
		
		return;
	}
}
