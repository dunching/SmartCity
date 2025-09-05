#include "Dynamic_WeatherBase.h"

#include <Kismet/GameplayStatics.h>

ADynamic_WeatherBase::ADynamic_WeatherBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	NetPriority = 3.0f;
	// SetNetUpdateFrequency(100.f);

	SetReplicatingMovement(true);

#if WITH_EDITOR
	bIsSpatiallyLoaded = false;
#endif // WITH_EDITOR

	WeatherMap.Add(WeatherSettings::Clear_Skies, nullptr);
	WeatherMap.Add(WeatherSettings::Cloudy, nullptr);
	WeatherMap.Add(WeatherSettings::Foggy, nullptr);
	WeatherMap.Add(WeatherSettings::Overcast, nullptr);
	WeatherMap.Add(WeatherSettings::Partly_Cloudy, nullptr);
	WeatherMap.Add(WeatherSettings::Rain, nullptr);
	WeatherMap.Add(WeatherSettings::Rain_Light, nullptr);
	WeatherMap.Add(WeatherSettings::Rain_Thunderstorm, nullptr);
	WeatherMap.Add(WeatherSettings::Sand_Dust_Calm, nullptr);
	WeatherMap.Add(WeatherSettings::Sand_Dust_Storm, nullptr);
	WeatherMap.Add(WeatherSettings::Snow, nullptr);
	WeatherMap.Add(WeatherSettings::Snow_Blizzard, nullptr);
	WeatherMap.Add(WeatherSettings::Snow_Light, nullptr);
}

void ADynamic_WeatherBase::BeginPlay()
{
	Super::BeginPlay();

	// ForceNetUpdate();
	// SetNetDormancy(ENetDormancy::DORM_DormantAll);
}

void ADynamic_WeatherBase::Tick(
	float DeltaSeconds
	)
{
	Super::Tick(DeltaSeconds);
}

void ADynamic_WeatherBase::EndPlay(
	const EEndPlayReason::Type EndPlayReason
	)
{
	Super::EndPlay(EndPlayReason);
}

inline void ADynamic_WeatherBase::ForceNetUpdate()
{
	Super::ForceNetUpdate();
}

void ADynamic_WeatherBase::UpdateWeather(
	const FGameplayTag& WeatherType
	)
{
	const auto Iter = WeatherMap.Find(WeatherType);
	if (Iter)
	{
		CurrentWeather = WeatherType;
		UpdateWeatherCPPImp(Iter->LoadSynchronous(), 3.f);
	}
}

bool ADynamic_WeatherBase::IsNetRelevantFor(
	const AActor* RealViewer,
	const AActor* ViewTarget,
	const FVector& SrcLocation
	) const
{
	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

FGameplayTag ADynamic_WeatherBase::GetCurrentWeather() const
{
	return CurrentWeather;
}

namespace WeatherSettings
{
	UE_DEFINE_GAMEPLAY_TAG(Clear_Skies, "Weather.Clear_Skies");
	UE_DEFINE_GAMEPLAY_TAG(Cloudy, "Weather.Cloudy");
	UE_DEFINE_GAMEPLAY_TAG(Foggy, "Weather.Foggy");
	UE_DEFINE_GAMEPLAY_TAG(Overcast, "Weather.Overcast");
	UE_DEFINE_GAMEPLAY_TAG(Partly_Cloudy, "Weather.Partly_Cloudy");
	UE_DEFINE_GAMEPLAY_TAG(Rain, "Weather.Rain");
	UE_DEFINE_GAMEPLAY_TAG(Rain_Light, "Weather.Rain_Light");
	UE_DEFINE_GAMEPLAY_TAG(Rain_Thunderstorm, "Weather.Rain_Thunderstorm");
	UE_DEFINE_GAMEPLAY_TAG(Sand_Dust_Calm, "Weather.Sand_Dust_Calm");
	UE_DEFINE_GAMEPLAY_TAG(Sand_Dust_Storm, "Weather.Sand_Dust_Storm");
	UE_DEFINE_GAMEPLAY_TAG(Snow, "Weather.Snow");
	UE_DEFINE_GAMEPLAY_TAG(Snow_Blizzard, "Weather.Snow_Blizzard");
	UE_DEFINE_GAMEPLAY_TAG(Snow_Light, "Weather.Snow_Light");
}

