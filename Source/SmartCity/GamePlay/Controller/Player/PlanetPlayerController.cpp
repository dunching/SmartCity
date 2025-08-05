#include "PlanetPlayerController.h"

#include "InputProcessorSubSystem_Imp.h"
#include "PlayerGameplayTasks.h"
#include "ViewBuildingProcessor.h"
#include "ViewerPawn.h"
#include "TourPawn.h"
#include "PixelStreamingInputComponent.h"

APlanetPlayerController::APlanetPlayerController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	GameplayTasksComponentPtr = CreateDefaultSubobject<UPlayerControllerGameplayTasksComponent>(
		UPlayerControllerGameplayTasksComponent::ComponentName
	);

	PixelStreamingInputPtr = CreateDefaultSubobject<UPixelStreamingInput>(
		TEXT("PixelStreamingInput")
	);
}

void APlanetPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	bool bIsNewPawn = (InPawn != GetPawn());

	if (bIsNewPawn)
	{
	}
}
