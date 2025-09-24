#include "SceneElement_Monitor.h"

#include "Engine/StaticMeshActor.h"
#include "ActorSequenceComponent.h"
#include "DatasmithAssetUserData.h"

#include "CollisionDataStruct.h"
#include "SmartCitySuiteTags.h"

ASceneElement_Monitor::ASceneElement_Monitor(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComponent->SetCollisionObjectType(Device_Object);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void ASceneElement_Monitor::ReplaceImp(
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

void ASceneElement_Monitor::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);
	
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
	
	if (ProcessJiaCengLogic(ConditionalSet))
	{
		SetActorHiddenInGame(true);
		return;
	}
}