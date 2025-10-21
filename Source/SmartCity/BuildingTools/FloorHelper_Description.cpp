#include "FloorHelper_Description.h"

#include "FloorHelper.h"
#include "Components/TextRenderComponent.h"

AFloorHelper_Description::AFloorHelper_Description(
	const FObjectInitializer& ObjectInitializer
	)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ArmMeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmMeshPtr"));
	ArmMeshPtr->SetupAttachment(RootComponent);

	BorderScenePtr = CreateDefaultSubobject<USceneComponent>(TEXT("BorderScene"));
	BorderScenePtr->SetupAttachment(ArmMeshPtr);

	BorderAddScenePtr = CreateDefaultSubobject<USceneComponent>(TEXT("BorderAddScenePtr"));
	BorderAddScenePtr->SetupAttachment(BorderScenePtr);

	LeftBorderMeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftBorderMeshPtr"));
	LeftBorderMeshPtr->SetupAttachment(BorderAddScenePtr);

	MidBorderMeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MidBorderMeshPtr"));
	MidBorderMeshPtr->SetupAttachment(BorderAddScenePtr);

	RightBorderMeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightBorderMeshPtr"));
	RightBorderMeshPtr->SetupAttachment(BorderAddScenePtr);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 30;
}

void AFloorHelper_Description::Tick(
	float DeltaTime
	)
{
	Super::Tick(DeltaTime);

	const auto Pt = GEngine->GetFirstLocalPlayerController(GetWorld())->PlayerCameraManager->GetCameraLocation();
	auto Dir = (Pt - BorderScenePtr->GetComponentLocation()).GetSafeNormal();

	BorderScenePtr->SetWorldRotation(Dir.Rotation());
}

void AFloorHelper_Description::SetFloor(
	AFloorHelper* FloorPtr
	)
{
	if (FloorPtr)
	{
		FloorIndexText->SetText(FText::FromString(FloorPtr->FloorIndexDescription));
		FloorDecriptionText->SetText(FText::FromString(FloorPtr->FloorDescription));
	}
}
