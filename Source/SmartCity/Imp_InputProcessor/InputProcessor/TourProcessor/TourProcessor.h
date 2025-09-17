#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "InputProcessor.h"
#include "GenerateTypes.h"

class ATrackVehicleBase;
class ATourPawn;
class AHorseCharacter;
class ABuildingBase;

struct FControlParam;

namespace TourProcessor
{
	/**
	 * 楼层炸开
	 */
	class SMARTCITY_API FTourProcessor : public FInputProcessor
	{
	private:
		GENERATIONCLASSINFO(FTourProcessor, FInputProcessor);

	public:
		using FOwnerPawnType = ATourPawn;

		FTourProcessor(FOwnerPawnType* CharacterPtr);

		void UpdateCameraSetting(float InViewPitchMin,float InViewPitchMax);
	
	protected:

		bool UpdateCameraArmLen(const FControlParam&ControlParam,float Value);

		bool UpdateCamera(const FControlParam&ControlParam);
	};

}
