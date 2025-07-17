// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GameMode_Main.h"

#include "DrawDebugHelpers.h"

#include "WeatherSystem.h"

AGameMode_Main::AGameMode_Main() :
	Super()
{
}

void AGameMode_Main::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AGameMode_Main::BeginPlay()
{
	Super::BeginPlay();

	UWeatherSystem::GetInstance()->RegisterCallback();
	UWeatherSystem::GetInstance()->ResetTime();

	TArray<AActor*>ResultAry;

}

void AGameMode_Main::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto Iter : CharacterProxyMap)
	{
		if (Iter.Value)
		{
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AGameMode_Main::BeginDestroy()
{
	Super::BeginDestroy();
}
