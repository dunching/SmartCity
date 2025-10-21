#include "SceneInteractionDecorator_Option.h"

#include "Kismet/GameplayStatics.h"
#include "Net/WebChannelWorldSystem.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "GameOptions.h"
#include "LogWriter.h"
#include "MessageBody.h"
#include "SceneInteractionWorldSystem.h"
#include "Algorithm.h"
#include "DatasmithSceneActor.h"
#include "Dynamic_WeatherBase.h"
#include "FloorHelper.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "SceneElement_PWR_Pipe.h"
#include "TemplateHelper.h"
#include "FloorHelperBase.h"
#include "IPSSI.h"
#include "NavagationPaths.h"
#include "PersonMark.h"
#include "SceneElement_Space.h"
#include "SmartCitySuiteTags.h"
#include "ViewSingleFloorProcessor.h"
#include "WeatherSystem.h"
#include "TourPawn.h"
#include "ViewBuildingProcessor.h"
#include "ViewSingleDeviceProcessor.h"
#include "ViewSingleFloorViewEnergyProcessor.h"
#include "ViewSplitFloorProcessor.h"
#include "SceneElement_AccessControl.h"

void FInteraction_Decorator::SwitchIteractionType(
	EInteractionType NewInteractionType
	)
{
	Config.InteractionType = NewInteractionType;
}

FInteraction_Decorator::EInteractionType FInteraction_Decorator::GetInteractionType() const
{
	return Config.InteractionType;
}

FGameplayTag FInteraction_Decorator::GetCurrentWeather() const
{
	return Config.CurrentWeather;
}

void FInteraction_Decorator::SetCurrentWeather(
	const FGameplayTag& WeatherTag
	)
{
	Config.CurrentWeather = WeatherTag;

	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather(Config.CurrentWeather);
}

int32 FInteraction_Decorator::GetCurrentHour() const
{
	return Config.CurrentHour;
}

void FInteraction_Decorator::SetCurrentHour(
	int32 Hour
	)
{
	Config.CurrentHour = Hour;

	FDateTime Time(1, 1, 1, Hour);

	UWeatherSystem::GetInstance()->AdjustTime(Time);
}

void FInteraction_Decorator::Update(
	const FConfig& InConfig
	)
{
	Config = InConfig;
}

FInteraction_Decorator::FConfig FInteraction_Decorator::GetCurrentConfig() const
{
	return Config;
}
