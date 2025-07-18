#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "InputProcessor.h"
#include "GenerateTypes.h"

class ATrackVehicleBase;
class ATourPawn;
class AHorseCharacter;
class ABuildingBase;

namespace TourProcessor
{
	class SMARTCITY_API FTourProcessor : public FInputProcessor
	{
	private:
		GENERATIONCLASSINFO(FTourProcessor, FInputProcessor);

	public:
		using FOwnerPawnType = ATourPawn;

		FTourProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction() override;

		virtual bool InputKey(
			const FInputKeyEventArgs& EventArgs
		) override;

		virtual bool InputAxis(
			const FInputKeyEventArgs& EventArgs 
			) ;

	private:
		bool bStartRot = false;
		
		bool bStartMove = false;
	};
}
