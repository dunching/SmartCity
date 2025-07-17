#include "PlanetPlayerController.h"

#include "InputProcessorSubSystem_Imp.h"
#include "TourProcessor.h"
#include "ViewerPawn.h"

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
