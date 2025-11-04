// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>
#include "NativeGameplayTags.h"

#include "Dynamic_WeatherBase.generated.h"

UCLASS(Blueprintable)
class UDS_Weather_Settings : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
};

UCLASS()
class WEATHER_API ADynamic_WeatherBase : public AActor
{
	GENERATED_BODY()

public:
	ADynamic_WeatherBase(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	virtual void Tick(
		float DeltaSeconds
		) override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
		) override;

	virtual void ForceNetUpdate() override;

	void UpdateWeather(
		const FGameplayTag& WeatherType
		);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeatherCPP();

	virtual bool IsNetRelevantFor(
		const AActor* RealViewer,
		const AActor* ViewTarget,
		const FVector& SrcLocation
		) const override;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCloudCoverage(
		float CloudCoverage
		);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCloudCoverageMunualOverride(
		bool CloudCoverageMunualOverride
		);

	FGameplayTag GetCurrentWeather() const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeatherCPPImp(
		UPrimaryDataAsset* DS_Weather_SettingsPtr,
		float TransitionDelta
		);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, TSoftObjectPtr<UPrimaryDataAsset>> WeatherMap;

	FGameplayTag CurrentWeather;
};

namespace WeatherSettings
{
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Clear_Skies);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cloudy);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Foggy);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Overcast);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Partly_Cloudy);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Rain);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Rain_Light);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Rain_Thunderstorm);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sand_Dust_Calm);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sand_Dust_Storm);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Snow);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Snow_Blizzard);
	WEATHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Snow_Light);
}
