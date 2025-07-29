#include "SceneElement_Space.h"

#include "Engine/OverlapResult.h"
#include "Engine/StaticMeshActor.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "SceneInteractionWorldSystem.h"

ASceneElement_Space::ASceneElement_Space(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void ASceneElement_Space::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneElement_Space::ReplaceImp(
	AActor* ActorPtr
	)
{
	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			StaticMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());
		}

		auto SpaceMaterialInstance = UAssetRefMap::GetInstance()->SpaceMaterialInstance;

		TArray<UStaticMeshComponent*> Components;
		GetComponents<UStaticMeshComponent>(Components);
		for (auto Iter : Components)
		{
			if (Iter)
			{
				Iter->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				Iter->SetCollisionObjectType(Space_Object);
				Iter->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				
				Iter->SetCollisionResponseToChannel(ExternalWall_Object, ECollisionResponse::ECR_Overlap);
				Iter->SetCollisionResponseToChannel(Floor_Object, ECollisionResponse::ECR_Overlap);
				Iter->SetCollisionResponseToChannel(Device_Object, ECollisionResponse::ECR_Overlap);

				Iter->SetRenderCustomDepth(false);

				for (int32 Index = 0; Index < Iter->GetNumMaterials(); Index++)
				{
					Iter->SetMaterial(Index, SpaceMaterialInstance.LoadSynchronous());
				}

				Iter->SetCastShadow(false);
				Iter->bVisibleInReflectionCaptures = false;
				Iter->bVisibleInRealTimeSkyCaptures = false;
				Iter->bVisibleInRayTracing = false;
				Iter->bReceivesDecals = false;
				Iter->bUseAsOccluder = false;

				break;
			}
		}
	}
}

void ASceneElement_Space::SwitchFocusState(
	bool bIsFocus
	)
{
	Super::SwitchFocusState(bIsFocus);

	if (bIsFocus)
	{
		auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
		if (PrimitiveComponentPtr)
		{
			PrimitiveComponentPtr->SetRenderCustomDepth(true);
			PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
		}

		TArray<FOverlapResult> OutOverlap;

		FCollisionQueryParams ObjectQueryParams;

		// ObjectQueryParams.bTraceComplex = true;

		StaticMeshComponent->ComponentOverlapComponentWithResult(
		                                  StaticMeshComponent,
		                                  StaticMeshComponent->GetRelativeLocation(),
		                                  StaticMeshComponent->GetRelativeRotation(),
		                                  ObjectQueryParams,
		                                  OutOverlap
		                                 );

		StaticMeshComponent->ComponentOverlapComponentWithResult(
		StaticMeshComponent,
		StaticMeshComponent->GetComponentLocation(),
		StaticMeshComponent->GetComponentRotation(),
		                                  ObjectQueryParams,
		                                  OutOverlap
		                                 );

		auto InteractionModeDecoratorSPtr = USceneInteractionWorldSystem::GetInstance()->GetInteractionModeDecorator();
		for (const auto& Iter : OutOverlap)
		{
			
		}
	}
	else
	{
		auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
		if (PrimitiveComponentPtr)
		{
			PrimitiveComponentPtr->SetRenderCustomDepth(false);
		}
	}
}
