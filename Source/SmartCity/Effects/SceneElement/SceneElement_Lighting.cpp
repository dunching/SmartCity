#include "SceneElement_Lighting.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "Engine/StaticMeshActor.h"

#include "GameplayTagsLibrary.h"
#include "RouteMarker.h"
#include "SmartCitySuiteTags.h"

ASceneElement_Lighting::ASceneElement_Lighting(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
}

void ASceneElement_Lighting::ReplaceImp(
	AActor* ActorPtr
	)
{
}

void ASceneElement_Lighting::Merge(
	const TSoftObjectPtr<AActor>& ActorRef
	)
{
	Super::Merge(ActorRef);
}

void ASceneElement_Lighting::MergeWithNear(
	const TSoftObjectPtr<AActor>& ActorRef
	)
{
	Super::MergeWithNear(ActorRef);

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

			const auto Num = NewComponentPtr->GetNumMaterials();
			for (int32 Index = 0; Index < Num; Index++)
			{
				auto MaterialPtr = UMaterialInstanceDynamic::Create(EmissiveMaterialInstance.LoadSynchronous(), this);
				NewComponentPtr->SetMaterial(Index, MaterialPtr);
			}
			
			StaticMeshComponentsAry.Add(NewComponentPtr);
		}

		ActorRef->Destroy();
	}
}

void ASceneElement_Lighting::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(true);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			SetEmissiveValue(1);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() == EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			SetEmissiveValue(0);

			return;
		}
	}
	
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_Focus.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
			EmptyContainer.Num())
		{
			// RouteMarkerPtr = CreateWidget<URouteMarker>(
			// 											GEngine->GetFirstLocalPlayerController(GetWorld()),
			// 											UAssetRefMap::GetInstance()->SpaceRouteMarkerClass
			// 										   );
			// if (RouteMarkerPtr)
			// {
			// 	RouteMarkerPtr->TextStr = TEXT("照明");
			// 	RouteMarkerPtr->TargetActor = this;
			// 	RouteMarkerPtr->AddToViewport();
			// }
			// if (DeviceType.MatchesTag(
			// 						  USmartCitySuiteTags::SceneElement_FanCoil.GetTag()
			// 						 ))
			// {
			// }
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

void ASceneElement_Lighting::SetEmissiveValue(
	int32 Value
	)
{
	for (auto MeshIter : StaticMeshComponentsAry)
	{
		const auto Num = MeshIter->GetNumMaterials();
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
