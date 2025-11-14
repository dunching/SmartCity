#include "SceneElement_RadarSweep.h"

#include "Components/BoxComponent.h"
#include "Marks/PersonMark.h"

#include "AssetRefMap.h"
#include "FloorHelper.h"
#include "GameplayTagsLibrary.h"
#include "SmartCitySuiteTags.h"
#include "PersonMark.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionWorldSystem.h"
#include "TemplateHelper.h"
#include "SceneInteractionDecorator_Area.h"

ASceneElement_RadarSweep::ASceneElement_RadarSweep(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
