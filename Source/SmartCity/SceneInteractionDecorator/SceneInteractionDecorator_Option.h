// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"
#include "SceneInteractionDecorator.h"

#include "Tools.h"

class AFloorHelper;
class USceneInteractionWorldSystem;
class ASceneElement_PWR_Pipe;
class APersonMark;
class UGT_SwitchSceneElementState;
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

	enum class EInteractionType:uint8
	{
		kDevice,
		kSpace,
	};

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

	struct FConfig
	{

		/**
		 * 墙体透明度 0 完全透明（隐藏） 100 完全不透明
		 */
		int32 WallTranlucent = 100;

		/**
		 * 墙体透明度 0 完全透明（隐藏） 100 完全不透明
		 */
		int32 PillarTranlucent = 100;

		/**
		 * 楼梯透明度 0 完全透明（隐藏） 100 完全不透明
		 */
		int32 StairsTranlucent = 100;

		/**
		 * 幕墙墙体透明度 0 完全透明（隐藏） 100 完全不透明
		 */
		bool bShowCurtainWall = true;

		/**
		 * 是否显示家具
		 */
		bool bShowFurniture = true;

		bool bUseCustomPitchLimit = false;

		float ViewPitchMin = -45;

		float ViewPitchMax = -45;

		EInteractionType InteractionType = EInteractionType::kSpace;

		FGameplayTag CurrentWeather;

		int32 CurrentHour;
		
	};

	FConfig Config;

	void Update(const FConfig &Config);
	
	FConfig GetCurrentConfig()const;
	
private:
};

#pragma endregion
