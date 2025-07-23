// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>
#include <map>
#include <set>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "GameplayTasksComponent.h"
#include "GenerateTypes.h"

#include "PlayerGameplayTasks.generated.h"

class AViewerPawn;
class USceneInteractionWorldSystem;
class ADatasmithSceneActor;

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
	void Adjust(
		float Percent
		) const;

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

/**
 * 定位某个设备
 */
UCLASS()
class SMARTCITY_API UGT_CameraTransformLocaterByID : public UGT_CameraTransform
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool
		)>;

	virtual void Activate() override;

	FGuid ID;
};

#pragma endregion

#pragma region 批量任务处理

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_BatchBase : public UGameplayTask
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool
		)>;

	UGT_BatchBase(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void TickTask(
		float DeltaTime
		) override;

protected:
	virtual bool ProcessTask();

	double ScopeTiempo = 1.f;

	bool bUseScope = true;

	int32 PerTickProcessNum = 100;

private:
	int32 CurrentTickProcessNum = 0;
};

#pragma endregion

#pragma region 数据处理

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_InitializeSceneActors : public UGT_BatchBase
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

	FOnEnd OnEnd;

protected:
	virtual bool ProcessTask() override;

private:

	int32 SceneActorMapIndex = 0;
	
	TArray<FSceneActorMap> SceneActorMap;

	int32 DataSmithSceneActorsSetIndex = 0;
	
	TArray<TSoftObjectPtr<ADatasmithSceneActor>> DataSmithSceneActorsSet;

	int32 RelatedActorsIndex = 0;
	
	TArray<TPair<FName, TSoftObjectPtr<AActor>>> RelatedActors;
};

#pragma endregion

#pragma region 场景对象切换

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_SceneObjSwitch : public UGT_BatchBase
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&

		
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

	TSet<FSceneActorConditional,TSceneActorConditionalKeyFuncs> FilterTags;

	/**
	 * 是否清楚之前的
	 */
	bool bClearPrevious = false;
	
	FOnEnd OnEnd;

protected:
	virtual bool ProcessTask() override;

private:
	
	/**
	 * 建筑物
	 * 用于计算保卫狂
	 */
	TSet<AActor*> Result;

	/**
	 * 等待显示
	 */
	int32 FilterIndex = 0;
	
	std::map<AActor*,int32> FilterCount;
	

	int32 HideDataSmithSceneActorsSetIndex = 0;
	
	TArray<TSoftObjectPtr<ADatasmithSceneActor>> HideDataSmithSceneActorsSet;

	
	int32 DataSmithSceneActorsSetIndex = 0;
	
	TArray<TSoftObjectPtr<ADatasmithSceneActor>> DataSmithSceneActorsSet;

	
	int32 RelatedActorsIndex = 0;
	
};

#pragma endregion
