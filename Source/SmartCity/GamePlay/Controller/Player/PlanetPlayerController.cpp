#include "PlanetPlayerController.h"

#include "InputProcessorSubSystem_Imp.h"
#include "PlayerGameplayTasks.h"
#include "TourProcessor.h"
#include "ViewerPawn.h"
#include "TourPawn.h"

APlanetPlayerController::APlanetPlayerController(const FObjectInitializer& ObjectInitializer) :
	  Super(ObjectInitializer)
{
	GameplayTasksComponentPtr = CreateDefaultSubobject<UPlayerControllerGameplayTasksComponent>(
		 UPlayerControllerGameplayTasksComponent::ComponentName
		);
}

inline void APlanetPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	bool bIsNewPawn = (InPawn != GetPawn());

	if (bIsNewPawn)
	{
	}

	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<TourProcessor::FTourProcessor>(
		[this, InPawn](
		auto NewProcessor
	)
		{
		}
	);
}
