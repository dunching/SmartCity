#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "InputProcessor.h"
#include "GenerateTypes.h"
#include "TourProcessor.h"
#include "GameOptions.h"

class ATrackVehicleBase;
class ATourPawn;
class AHorseCharacter;
class ABuildingBase;

namespace TourProcessor
{
	/**
	 * 楼层炸开
	 */
	class SMARTCITY_API FViewSpecialAreaProcessor : public FTourProcessor
	{
	private:
		GENERATIONCLASSINFO(FViewSpecialAreaProcessor, FTourProcessor);

	public:
		using FOwnerPawnType = ATourPawn;

		FViewSpecialAreaProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction() override;

		virtual bool InputKey(
			const FInputKeyEventArgs& EventArgs
		) override;

		virtual bool InputAxis(
			const FInputKeyEventArgs& EventArgs 
			) override;

		FControlParam ControlParam;
	
	private:

		bool bHasRoted = false;
		
		bool bStartRot = false;
		
		bool bHasMoved = false;
		
		bool bStartMove = false;
	};
}
