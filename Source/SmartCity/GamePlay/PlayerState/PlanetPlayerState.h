// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "TemplateHelper.h"

#include "GameFramework/PlayerState.h"

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "PlanetPlayerState.generated.h"

class UPlanetAbilitySystemComponent;
class UProxySycHelperComponent;
class UAudioComponent;
struct FCharacterProxy;
struct FSceneProxyContainer;

/*
 *	玩家数据
 */
UCLASS()
class SMARTCITY_API APlanetPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	using FOnRegionChanged =
	TCallbackHandleContainer<void(const FGameplayTag&)>;

	APlanetPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FOnRegionChanged OnRegionChanged;
	
	/*
	 *	角色进入副本之前在开放世界的位置信息
	 */
	FTransform OpenWorldTransform = FTransform::Identity;
	
protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

	void InitialData();

private:

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent>AudioComponentPtr = nullptr;
	
	FString PlayerName;

};
