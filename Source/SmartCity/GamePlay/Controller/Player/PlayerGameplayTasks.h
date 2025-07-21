// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "GameplayTasksComponent.h"

#include "PlayerGameplayTasks.generated.h"

class AViewerPawn;
class USceneInteractionWorldSystem;

/*
 * PlayerController处理异步的组件
 */
UCLASS(
	BlueprintType,
	Blueprintable
)
class SMARTCITY_API UPlayerControllerGameplayTasksComponent : public UGameplayTasksComponent
{
	GENERATED_BODY()

public:
	static FName ComponentName;

	template <typename GameplayTaskType>
	void StartGameplayTask(
		const std::function<void(
			GameplayTaskType*
			)>& Func
		);
};

template <typename GameplayTaskType>
void UPlayerControllerGameplayTasksComponent::StartGameplayTask(
	const std::function<void(
		GameplayTaskType*
		)>& Func
	)
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
		bool
		)>;

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
		bool
		)>;

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
		bool
		)>;

	virtual void Activate() override;

	virtual void OnDestroy(
		bool bInOwnerFinished
		) override;
};

#pragma endregion

#pragma region 数据处理

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_InitializeSceneActors : public UGameplayTask
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool
		)>;

	UGT_InitializeSceneActors(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void Activate() override;

	virtual void TickTask(
		float DeltaTime
		) override;

	virtual void OnDestroy(
		bool bInOwnerFinished
		) override;

	USceneInteractionWorldSystem* SceneInteractionWorldSystemPtr = nullptr;

private:
	TArray<AActor*> ResultAry;

	int32 Index = 0;
};

#pragma endregion

#pragma region 场景对象切换

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_SceneObjSwitch : public UGameplayTask
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool
		)>;

	UGT_SceneObjSwitch(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void Activate() override;

	virtual void TickTask(
		float DeltaTime
		) override;

	virtual void OnDestroy(
		bool bInOwnerFinished
		) override;

	USceneInteractionWorldSystem* SceneInteractionWorldSystemPtr = nullptr;

	FGameplayTag Interaction_Area;
private:
	TArray<AActor*> ResultAry;

	int32 Index = 0;
};

#pragma endregion
