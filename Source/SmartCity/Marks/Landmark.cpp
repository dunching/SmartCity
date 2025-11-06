#include "Landmark.h"

#include "NiagaraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

ALandmark::ALandmark(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ArmMeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmMeshPtr"));
	ArmMeshPtr->SetupAttachment(RootComponent);

	BorderScenePtr = CreateDefaultSubobject<USceneComponent>(TEXT("BorderScene"));
	BorderScenePtr->SetupAttachment(ArmMeshPtr);

	BorderAddScenePtr = CreateDefaultSubobject<USceneComponent>(TEXT("BorderAddScenePtr"));
	BorderAddScenePtr->SetupAttachment(BorderScenePtr);

	FloorDecriptionText = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	FloorDecriptionText->SetupAttachment(BorderAddScenePtr);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 30;
}

void ALandmark::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);
}

void ALandmark::BeginPlay()
{
	Super::BeginPlay();

	auto UIPtr = Cast<ULandmarkHelper>(FloorDecriptionText->GetUserWidgetObject());
	if (UIPtr && UIPtr->FloorIndexText)
	{
		UIPtr->FloorIndexText->SetText(FText::FromString(LandmarkName));
	}
}

void ALandmark::SetText(
	const FString& InText
	)
{
	auto UIPtr = Cast<ULandmarkHelper>(FloorDecriptionText->GetUserWidgetObject());
	if (UIPtr)
	{
		UIPtr->FloorIndexText->SetText(FText::FromString(InText));
	}
}
