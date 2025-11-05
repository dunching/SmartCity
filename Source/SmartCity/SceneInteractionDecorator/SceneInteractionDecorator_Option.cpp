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
#include "ViewTowerProcessor.h"
#include "ViewSingleDeviceProcessor.h"
#include "ViewSingleFloorViewEnergyProcessor.h"
#include "ViewSplitFloorProcessor.h"
#include "SceneElement_AccessControl.h"
#include "SceneInteractionDecorator_Area.h"

void FInteraction_Decorator::SwitchIteractionType(
	EInteractionType NewInteractionType
	)
{
	ControllConfig.InteractionType = NewInteractionType;
}

EInteractionType FInteraction_Decorator::GetInteractionType() const
{
	return ControllConfig.InteractionType;
}

FGameplayTag FInteraction_Decorator::GetCurrentWeather() const
{
	return ControllConfig.CurrentWeather;
}

void FInteraction_Decorator::SetCurrentWeather(
	const FGameplayTag& WeatherTag
	)
{
	ControllConfig.CurrentWeather = WeatherTag;

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );

	if (AreaDecoratorSPtr && AreaDecoratorSPtr->GetBranchDecoratorType().MatchesTag(
		     USmartCitySuiteTags::Interaction_Area_Floor
		    ))
	{
		return;
	}

	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather(ControllConfig.CurrentWeather);
}

int32 FInteraction_Decorator::GetCurrentHour() const
{
	return ControllConfig.CurrentHour;
}

void FInteraction_Decorator::SetCurrentHour(
	int32 Hour
	)
{
	ControllConfig.CurrentHour = Hour;

	FDateTime Time(1, 1, 1, Hour);

	UWeatherSystem::GetInstance()->AdjustTime(Time);
}

void FInteraction_Decorator::UpdateViewConfig(
	const FViewConfig& InConfig,
	bool bIsTempora
	)
{
	if (bIsTempora)
	{
		TemporaViewConfigAry = {InConfig};

		auto MessageSPtr = MakeShared<
			FMessageBody_ViewConfigChanged>();

		MessageSPtr->ViewConfig = InConfig;

		UWebChannelWorldSystem::GetInstance()->
			SendMessage(MessageSPtr);
	}
	else
	{
		ViewConfig = {InConfig};
	}
}

void FInteraction_Decorator::ClearTemporaViewConfig()
{
	TemporaViewConfigAry.Empty();
}

FViewConfig FInteraction_Decorator::GetViewConfig() const
{
	if (TemporaViewConfigAry.IsEmpty())
	{
	}
	else
	{
		return TemporaViewConfigAry[0];
	}

	if (ViewConfig.IsEmpty())
	{
	}
	else
	{
		return ViewConfig[0];
	}

	return FViewConfig();
}

bool FInteraction_Decorator::HasViewConfigChanged() const
{
	return ViewConfig.Num() > 0;
}

void FInteraction_Decorator::UpdateControlConfig(
	const FControlConfig& InConfig
	)
{
	ControllConfig = InConfig;
}

FControlConfig FInteraction_Decorator::GetConfigControlConfig() const
{
	return ControllConfig;
}
