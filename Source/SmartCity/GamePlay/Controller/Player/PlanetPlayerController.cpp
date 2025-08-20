#include "PlanetPlayerController.h"

#include "PlayerGameplayTasks.h"
#include "PixelStreamingInputComponent.h"
#include "WebChannelWorldSystem.h"

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

	UWebChannelWorldSystem::GetInstance()->BindEvent();
}
