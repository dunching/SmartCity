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
	/**
	 * 查看单楼层
	 */
	class SMARTCITY_API FViewSingleFloorProcessor : public FInputProcessor
	{
	private:
		GENERATIONCLASSINFO(FViewSingleFloorProcessor, FInputProcessor);

	public:
		using FOwnerPawnType = ATourPawn;

		FViewSingleFloorProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction() override;

		virtual bool InputKey(
			const FInputKeyEventArgs& EventArgs
		) override;

		virtual bool InputAxis(
			const FInputKeyEventArgs& EventArgs 
			) override;
		
		FGameplayTag Interaction_Area;
	private:

		bool bHasRoted = false;
		
		bool bStartRot = false;
		
		bool bHasMoved = false;
		
		bool bStartMove = false;
	};
}
