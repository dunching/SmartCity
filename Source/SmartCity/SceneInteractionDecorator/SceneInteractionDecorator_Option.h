// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"
#include "SceneInteractionDecorator.h"
#include "GameOptions.h"

#include "Tools.h"

class AFloorHelper;
class USceneInteractionWorldSystem;
class ASceneElement_PWR_Pipe;
class APersonMark;
class UGT_SwitchSceneElement_Generic;
class ASceneElement_DeviceBase;
class AFireMark;
class FArea_Decorator;

#pragma region 操作方式

/**
 * 
 */
class SMARTCITY_API FInteraction_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
						FInteraction_Decorator,
						FDecoratorBase
					   );

	FInteraction_Decorator();

	virtual void Entry() override;

	void SwitchIteractionType(
		EInteractionType NewInteractionType
		);

	EInteractionType GetInteractionType() const;

	FGameplayTag GetCurrentWeather() const;

	void SetCurrentWeather(
		const FGameplayTag& WeatherTag
		);

	int32 GetCurrentHour() const;

	void SetCurrentHour(
		int32 Hour
		);

	void UpdateViewConfig(const FViewConfig &Config);
	
	FViewConfig GetViewConfig()const;
	
	void UpdateControlConfig(const FControlConfig &Config);
	
	FControlConfig GetConfigControlConfig()const;
	
private:
	FViewConfig ViewConfig;

	FControlConfig ControllConfig;

};

#pragma endregion
