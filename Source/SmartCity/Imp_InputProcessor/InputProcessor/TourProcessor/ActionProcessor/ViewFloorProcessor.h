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
	class SMARTCITY_API FViewFloorProcessor : public FInputProcessor
	{
	private:
		GENERATIONCLASSINFO(FViewFloorProcessor, FInputProcessor);

	public:
		using FOwnerPawnType = ATourPawn;

		FViewFloorProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction() override;

		virtual bool InputKey(
			const FInputKeyEventArgs& EventArgs
		) override;

		virtual bool InputAxis(
			const FInputKeyEventArgs& EventArgs 
			) ;

	private:

		bool bHasRoted = false;
		
		bool bStartRot = false;
		
		bool bHasMoved = false;
		
		bool bStartMove = false;
	};
}
