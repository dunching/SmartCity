// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PlanetPlayerState.h"

#include "Components/AudioComponent.h"

APlanetPlayerState::APlanetPlayerState(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;

	AudioComponentPtr = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}

void APlanetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	InitialData();
}

void APlanetPlayerState::Tick(
	float DeltaSeconds
	)
{
	Super::Tick(DeltaSeconds);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
	}
#endif
}

void APlanetPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APlanetPlayerState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void APlanetPlayerState::InitialData()
{
}
