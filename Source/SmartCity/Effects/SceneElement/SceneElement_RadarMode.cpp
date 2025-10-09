#include "SceneElement_RadarMode.h"

#include "Marks/PersonMark.h"
#include "Engine/StaticMeshActor.h"

#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
#include "SmartCitySuiteTags.h"

ASceneElement_RadarMode::ASceneElement_RadarMode(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void ASceneElement_RadarMode::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);
}

void ASceneElement_RadarMode::BeginPlay()
{
	Super::BeginPlay();

	// const auto Interval = Deepth / MeshNum;
	// for (int32 Index = 0; Index < MeshNum; Index++)
	// {
	// 	FTransform Transform;
	//
	// 	const auto Distance = Index * Interval;
	// 	FVector Location(Distance);
	// 	Transform.SetTranslation(Location);
	//
	// 	auto NewComponentPtr = Cast<UStaticMeshComponent>(
	// 	                                                  AddComponentByClass(
	// 	                                                                      UStaticMeshComponent::StaticClass(),
	// 	                                                                      true,
	// 	                                                                      Transform,
	// 	                                                                      false
	// 	                                                                     )
	// 	                                                 );
	//
	// 	NewComponentPtr->SetStaticMesh(Mesh.LoadSynchronous());
	// 	NewComponentPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	//
	// 	NewComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//
	// 	NewComponentPtr->SetRenderCustomDepth(false);
	//
	// 	MeshAry.Add({NewComponentPtr, Distance});
	// }
}

void ASceneElement_RadarMode::Tick(
	float DeltaTime
	)
{
	Super::Tick(DeltaTime);
}

void ASceneElement_RadarMode::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	// Super::SwitchInteractionType(ConditionalSet);

	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(true);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar.GetTag());

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

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Interaction.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

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

void ASceneElement_RadarMode::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);

	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
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

			StaticMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());

			for (int32 Index = 0; Index < STPtr->GetStaticMeshComponent()->GetNumMaterials(); Index++)
			{
				StaticMeshComponent->SetMaterial(Index, STPtr->GetStaticMeshComponent()->GetMaterial(Index));
			}
		}
	}
}
