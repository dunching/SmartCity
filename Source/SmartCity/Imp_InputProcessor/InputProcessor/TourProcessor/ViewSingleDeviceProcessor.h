#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "InputProcessor.h"
#include "GenerateTypes.h"
#include "TourProcessor.h"

class ASceneElement_DeviceBase;
class ATrackVehicleBase;
class ATourPawn;
class AHorseCharacter;
class ABuildingBase;

namespace TourProcessor
{
	/**
	 * 楼层炸开
	 */
	class SMARTCITY_API FViewSingleDeviceProcessor : public FTourProcessor
	{
	private:
		GENERATIONCLASSINFO(FViewSingleDeviceProcessor, FTourProcessor);

	public:
		using FOwnerPawnType = ATourPawn;

		FViewSingleDeviceProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction() override;

		virtual bool InputKey(
			const FInputKeyEventArgs& EventArgs
		) override;

		virtual bool InputAxis(
			const FInputKeyEventArgs& EventArgs 
			) override;

		TWeakObjectPtr<ASceneElement_DeviceBase>TargetDevicePtr = nullptr;

	private:

		bool bHasRoted = false;
		
		bool bStartRot = false;
		
		bool bHasMoved = false;
		
		bool bStartMove = false;
	};
}
