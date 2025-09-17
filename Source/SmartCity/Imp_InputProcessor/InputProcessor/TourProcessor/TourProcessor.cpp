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

void TourProcessor::FTourProcessor::UpdateCameraSetting(
	float InViewPitchMin,
	float InViewPitchMax
	)
{
	Cast<APlanetPlayerCameraManager>(
									 GEngine->GetFirstLocalPlayerController(GetWorldImp())->PlayerCameraManager
									)->UpdateCameraSetting(InViewPitchMin, InViewPitchMax);

}

bool TourProcessor::FTourProcessor::UpdateCameraArmLen(
	const FControlParam& ControlParam,
	float Value
	)
{
	auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	if (!OnwerActorPtr)
	{
		return false;
	}

	auto GameOptionsPtr = UGameOptions::GetInstance();

	const auto ClampValue = FMath::Clamp(
	                                     OnwerActorPtr->SpringArmComponent->TargetArmLength - Value,
	                                     GameOptionsPtr->
	                                     ViewFloorEnergyControlParam.MinCameraSpringArm,
	                                     GameOptionsPtr->
	                                     ViewFloorEnergyControlParam.MaxCameraSpringArm
	                                    );

	OnwerActorPtr->SpringArmComponent->TargetArmLength = ClampValue;

	Cast<APlanetPlayerCameraManager>(
	                                 GEngine->GetFirstLocalPlayerController(GetWorldImp())->PlayerCameraManager
	                                )->UpdateCameraSetting();

	return true;
}

bool TourProcessor::FTourProcessor::UpdateCamera(
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
									)->UpdateCameraSetting();

	return true;
}
