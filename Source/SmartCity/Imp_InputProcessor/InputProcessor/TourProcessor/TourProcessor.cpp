#include "TourProcessor.h"

#include "GameFramework/SpringArmComponent.h"

#include "GameOptions.h"
#include "PlanetPlayerCameraManager.h"
#include "TourPawn.h"

TourProcessor::FTourProcessor::FTourProcessor(
	FOwnerPawnType* CharacterPtr
	) :
	  Super(CharacterPtr)
{
}

bool TourProcessor::FTourProcessor::UpdateCameraArmLen(
	const FControlParam& ControlParam,
	float OffsetValue
	)
{
	auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	if (!OnwerActorPtr)
	{
		return false;
	}

	const auto ClampValue = FMath::Clamp(
	                                     OnwerActorPtr->SpringArmComponent->TargetArmLength - OffsetValue,
	                                     ControlParam.MinCameraSpringArm,
	                                     ControlParam.MaxCameraSpringArm
	                                    );

	OnwerActorPtr->SpringArmComponent->TargetArmLength = ClampValue;

	return true;
}

bool TourProcessor::FTourProcessor::UpdateCameraClampPitch(
	const FControlParam& ControlParam
	)
{
	auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	if (!OnwerActorPtr)
	{
		return false;
	}

	Cast<APlanetPlayerCameraManager>(
									 GEngine->GetFirstLocalPlayerController(GetWorldImp())->PlayerCameraManager
									)->UpdateCameraSetting(ControlParam.CameraPitchMinLimit,ControlParam.CameraPitchMaxLimit);

	Cast<APlanetPlayerCameraManager>(
									 GEngine->GetFirstLocalPlayerController(GetWorldImp())->PlayerCameraManager
									)->UpdateCameraSetting();

	return true;
}
