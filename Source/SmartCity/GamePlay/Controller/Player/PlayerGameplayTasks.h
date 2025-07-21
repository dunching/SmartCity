// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "GameplayTasksComponent.h"

#include "PlayerGameplayTasks.generated.h"

class AViewerPawn;

/*
 * PlayerController处理异步的组件
 */
UCLASS(BlueprintType,
	Blueprintable)
class SMARTCITY_API UPlayerControllerGameplayTasksComponent : public UGameplayTasksComponent
{
	GENERATED_BODY()

public:
	static FName ComponentName;

	template <typename GameplayTaskType>
	void StartGameplayTask(const std::function<void(GameplayTaskType*)>& Func);
};

template <typename GameplayTaskType>
void UPlayerControllerGameplayTasksComponent::StartGameplayTask(const std::function<void(GameplayTaskType*)>& Func)
{
	auto GameplayTaskPtr = UGameplayTask::NewTask<GameplayTaskType>(
		TScriptInterface<
			IGameplayTaskOwnerInterface>(
			this
		)
	);

	if (Func)
	{
		Func(GameplayTaskPtr);
	}

	GameplayTaskPtr->ReadyForActivation();
}

#pragma region 摄像机修改

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_CameraTransform : public UGameplayTask
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool)>;

	UGT_CameraTransform(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void Activate() override;

	virtual void TickTask(
		float DeltaTime
	) override;

	float Duration = 1.f;

	FVector TargetLocation;

	FRotator TargetRotation;

	float TargetTargetArmLength = 0.f;

protected:
private:
	float CurrentTime = 0.f;

	FVector OriginalLocation;

	FRotator OriginalRotation;

	float OriginalSpringArmLen = 0.f;
};

/**
 * 恢复到原始视角
 */
UCLASS()
class SMARTCITY_API UGT_ReplyCameraTransform : public UGT_CameraTransform
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool)>;

	UGT_ReplyCameraTransform(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void Activate() override;

	virtual void OnDestroy(
		bool bInOwnerFinished
	) override;

	FGameplayTag SeatTag;
};

/**
 * 移动到指定Transform
 */
UCLASS()
class SMARTCITY_API UGT_ModifyCameraTransform : public UGT_CameraTransform
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool)>;

	UGT_ModifyCameraTransform(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void Activate() override;

	virtual void OnDestroy(
		bool bInOwnerFinished
	) override;
};

#pragma endregion
