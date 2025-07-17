// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Components/StateTreeComponent.h"

#include "GenerateTypes.h"

#include "PlayerComponent.generated.h"

struct FInputActionValue;

class USceneComponent;
class UInputActions;
class AHumanCharacter_Player;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class SMARTCITY_API UPlayerComponent :
	public UActorComponent
{
	GENERATED_BODY()

public:
	using FOwnerType = APawn;

	static FName ComponentName;

	virtual void PossessedBy(
		APlayerController* NewController
		);

	void SetupPlayerInputComponent(
		UInputComponent* PlayerInputComponent
		);

	virtual void MoveForward(
		const FInputActionValue& InputActionValue
		);

	virtual void MoveRight(
		const FInputActionValue& InputActionValue
		);

	virtual void AddPitchInput(
		const FInputActionValue& InputActionValue
		);

	virtual void AddYawInput(
		const FInputActionValue& InputActionValue
		);

	UPROPERTY()
	UInputActions* InputActionsPtr = nullptr;

};
