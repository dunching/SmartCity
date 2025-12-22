#include "ComputerMark.h"

#include "NiagaraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

AComputerMark::AComputerMark(
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

void AComputerMark::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);
}

void AComputerMark::BeginPlay()
{
	Super::BeginPlay();

	auto UIPtr = Cast<UComputerMarkWDGHelper>(FloorDecriptionText->GetUserWidgetObject());
	if (UIPtr && UIPtr->FloorIndexText)
	{
		UIPtr->FloorIndexText->SetText(FText::FromString(Name));
	}
}

void AComputerMark::SetText(
	const FString& InText
	)
{
	auto UIPtr = Cast<UComputerMarkWDGHelper>(FloorDecriptionText->GetUserWidgetObject());
	if (UIPtr)
	{
		UIPtr->FloorIndexText->SetText(FText::FromString(InText));
	}
}