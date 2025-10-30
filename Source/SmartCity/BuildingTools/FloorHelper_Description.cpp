#include "FloorHelper_Description.h"

#include "FloorHelper.h"
#include "Components/TextBlock.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"

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

	FloorDecriptionText = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	FloorDecriptionText->SetupAttachment(BorderAddScenePtr);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 30;
}

void AFloorHelper_Description::UpdateFloorDescription(
	const FString& NewFloorDescription
	)
{
	auto UIPtr = Cast<UFloorDescriptionHelper>(FloorDecriptionText->GetUserWidgetObject());
	if ( UIPtr)
	{
		UIPtr->FloorDecriptionText->SetText(FText::FromString(NewFloorDescription));
	}
}

void AFloorHelper_Description::BeginPlay()
{
	Super::BeginPlay();

	BorderAddScenePtr->SetRelativeRotation(OffsetRot);

	if (FloorPtr)
	{
		auto UIPtr = Cast<UFloorDescriptionHelper>(FloorDecriptionText->GetUserWidgetObject());
		if ( UIPtr)
		{
			UIPtr->FloorIndexText->SetText(FText::FromString(FloorPtr->FloorIndexDescription));
			UIPtr->FloorDecriptionText->SetText(FText::FromString(FloorPtr->FloorDescription));
		}
		
		SetActorTransform(FloorPtr->FloorHelper_DescriptionAttachTransform->GetComponentTransform());
	}
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
	AFloorHelper* InFloorPtr
	)
{
	if (InFloorPtr)
	{
		FloorPtr = InFloorPtr;
	}
}
