// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "PixelStreamingPlayerId.h"
#include "Engine/World.h"

#include "SceneInteractionDecorator.h"

#include "WebChannelWorldSystem.generated.h"

class FDecoratorBase;
struct FMessageBody;

/*
 */
UCLASS()
class SMARTCITY_API UWebChannelWorldSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	
	static UWebChannelWorldSystem* GetInstance();

	void BindEvent();
	
	void InitializeDeserializeStrategies();
	
	void SendMessage(const TSharedPtr<FMessageBody>& Message);

private:

	UFUNCTION()
	void OnConnectedToSignallingServerNative();

	void NewConnectionNative(FString Str, FPixelStreamingPlayerId ID, bool bIsTrue);

	UFUNCTION()
	void OnAllConnectionsClosed(FString StreamerId);

	UFUNCTION()
	void OnInput(const FString& Descriptor);

	void MessageTickImp();
	
	TMap<FString,TSharedPtr<FMessageBody>>DeserializeStrategiesMap;

	FTimerHandle MessageTickTimerHandle;
};
