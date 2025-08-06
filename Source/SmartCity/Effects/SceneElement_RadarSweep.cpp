#include "SceneElement_RadarSweep.h"

ASceneElement_RadarSweep::ASceneElement_RadarSweep(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	
	SweepEffectStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SweepEffectStaticMeshComponent"));
	SweepEffectStaticMeshComponent->SetupAttachment(RootComponent);
}

void ASceneElement_RadarSweep::SwitchInteractionType(
	EInteractionType InInteractionType
	)
{
	Super::SwitchInteractionType(InInteractionType);

	switch (CurrentInteractionType)
	{
	case EInteractionType::kRegular:
		{
			SetActorHiddenInGame(false);
		}
		break;
	case EInteractionType::kView:
		{
			SetActorHiddenInGame(false);
		}
		break;
	case EInteractionType::kFocus:
		{
			SetActorHiddenInGame(false);
		}
		break;
	case EInteractionType::kHide:
		{
			SetActorHiddenInGame(true);
		}
		break;
	case EInteractionType::kNone:
		{
			SetActorHiddenInGame(true);
		}
		break;
	}
}
