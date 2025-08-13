// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>
#include <map>
#include <set>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "GameplayTasksComponent.h"
#include "GenerateTypes.h"
#include "SceneInteractionWorldSystem.h"

#include "PlayerGameplayTasks.generated.h"

class AViewerPawn;
class USceneInteractionWorldSystem;
class ADatasmithSceneActor;
class AReplaceActorBase;

class UGameplayTaskBase;

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

	virtual void OnGameplayTaskDeactivated(
		UGameplayTask& Task
		) override;

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

UCLASS()
class SMARTCITY_API UGameplayTaskBase : public UGameplayTask
{
	GENERATED_BODY()

public:
	class UPlayerControllerGameplayTasksComponent;

	using FOnTaskComplete = TMulticastDelegate<void(
		bool
		)>;

	virtual void OnDestroy(
		bool bInOwnerFinished
		) override;

	UPROPERTY()
	TObjectPtr<UGameplayTaskBase> NextTaskPtr;

protected:
	FOnTaskComplete OnTaskComplete;
};


/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_CameraTransform : public UGameplayTaskBase
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

	TObjectPtr<AActor> TargetDevicePtr = nullptr;
	
};

/**
 * 定位某个区域
 */
UCLASS()
class SMARTCITY_API UGT_CameraTransformLocaterBySpace : public UGT_CameraTransform
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool
		)>;

	virtual void Activate() override;

	TWeakObjectPtr<AActor> SpaceActorPtr;
};

#pragma endregion

#pragma region 批量任务处理

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_BatchBase : public UGameplayTaskBase
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
	virtual bool ProcessTask(
		float DeltaTime
		);

	double ScopeTime = .1f;

	enum class EUseScopeType
	{
		kTime,
		kCount,
		kNone,
	};

	EUseScopeType UseScopeType = EUseScopeType::kTime;

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
	virtual bool ProcessTask(
		float DeltaTime
		) override;

	/**
	 * 
	 * @return true：未处理完成，false：处理完成
	 */
	bool ProcessTask_RecordFloor();

	/**
	 * 
	 * @return true：未处理完成，false：处理完成
	 */
	bool ProcessTask_NeedReplaceByRef();

	/**
	 * 
	 * @return true：未处理完成，false：处理完成
	 */
	bool ProcessTask_StructItemSet();

	bool ProcessTask_InnerStructItemSet();

	bool ProcessTask_SoftDecorationItemSet();

	bool ProcessTask_ReplaceSoftDecorationItemSet();

	bool ProcessTask_SpaceItemSet();

private:
	bool ReplacedActor(
		AActor* ActorPtr
		);

	/**
	 * 
	 * @param Index tu
	 * @param ItemSet 
	 * @return true：未处理完成，false：处理完成
	 */
	bool NormalAdjust(
		int32& Index,
		TArray<TSoftObjectPtr<ADatasmithSceneActor>>& ItemSet
		);

	bool NormalAdjust(
		int32& Index,
		TArray<TSoftObjectPtr<AReplaceActorBase>>& ItemSet
		);

	void ApplyData(
		int32 Index
		);

	void ApplyRelatedActors(
		const TSoftObjectPtr<ADatasmithSceneActor>& ItemSet
		);

	void ApplyRelatedActors(
		const TSoftObjectPtr<AReplaceActorBase>& ItemSet
		);

	int32 SceneActorMapIndex = 0;

	TArray<FSceneElementMap> SceneActorMap;


	enum class EStep
	{
		kRecordFloor,
		kNeedReplaceByRef,
		kStructItemSet,
		kInnerStructItemSet,
		kSoftDecorationItemSet,
		kSpaceItemSet,
	};

	EStep Step = EStep::kRecordFloor;


	int32 StructItemSetIndex = 0;

	TArray<TSoftObjectPtr<ADatasmithSceneActor>> StructItemSet;

	int32 InnerStructItemSetIndex = 0;

	TArray<TSoftObjectPtr<ADatasmithSceneActor>> InnerStructItemSet;

	int32 SoftDecorationItemSetIndex = 0;

	TArray<TSoftObjectPtr<ADatasmithSceneActor>> SoftDecorationItemSet;

	int32 SpaceItemSetIndex = 0;

	TArray<TSoftObjectPtr<ADatasmithSceneActor>> SpaceItemSet;


	int32 RelatedActorsIndex = 0;

	TArray<TObjectPtr<AActor>> RelatedActors;


	TMap<int32, ASceneElementBase*> MergeActorsMap;
};

#pragma endregion

#pragma region 场景对象切换

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_SwitchSceneElementState : public UGT_BatchBase
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&
		
		)>;

	UGT_SwitchSceneElementState(
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

	FSceneElementConditional FilterTags;

	/**
	 * 是否清除之前的
	 */
	bool bClearPrevious = false;

	FOnEnd OnEnd;

protected:
	virtual bool ProcessTask(
		float DeltaTime
		) override;

private:
	bool ProcessTask_Display();

	bool ProcessTask_Hiden();

	bool ProcessTask_ConfirmConditional();

	bool ProcessTask_SwitchState();

	enum class EStep
	{
		kDisplay,
		kHiden,
		kConfirmConditional,
		kSwitchState,
		kComplete,
	};

	EStep Step = EStep::kDisplay;

	/**
	 * 建筑物
	 * 用于计算包围框
	 */
	TSet<AActor*> Result;


	int32 DataSmithSceneActorsSetIndex = 0;

	TArray<TSoftObjectPtr<ADatasmithSceneActor>> DataSmithSceneActorsSet;

	int32 ReplaceActorsSetIndex = 0;

	TArray<TSoftObjectPtr<AReplaceActorBase>> ReplaceActorsSet;

	int32 DisplayAryIndex = 0;

	TArray<AActor*> DisplayAry;

	
	int32 HideDataSmithSceneActorsSetIndex = 0;

	TArray<TSoftObjectPtr<ADatasmithSceneActor>> HideDataSmithSceneActorsSet;

	int32 HideRePlaceActorsSetIndex = 0;

	TArray<TSoftObjectPtr<AReplaceActorBase>> HideReplaceActorsSet;
	
	int32 HideAryIndex = 0;

	TArray<AActor*> HideAry;


	int32 RelatedActorsIndex = 0;
};

UCLASS()
class SMARTCITY_API UGT_SwitchSingleSceneElementState : public UGT_BatchBase
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&
		
		)>;

	UGT_SwitchSingleSceneElementState(
		const FObjectInitializer& ObjectInitializer
		);

	FSceneElementFilter SceneElementFilter;

	FSceneElementConditional FilterTags;

	FOnEnd OnEnd;

protected:
	virtual bool ProcessTask(
		float DeltaTime
		) override;

private:
	bool ProcessTask_Display();

	bool ProcessTask_Hiden();

	bool ProcessTask_ConfirmConditional();

	bool ProcessTask_SwitchState();

	enum class EStep
	{
		kDisplay,
		kHiden,
		kConfirmConditional,
		kSwitchState,
		kComplete,
	};

	EStep Step = EStep::kDisplay;

	/**
	 * 建筑物
	 * 用于计算包围框
	 */
	TSet<AActor*> Result;


	int32 DataSmithSceneActorsSetIndex = 0;

	TArray<TSoftObjectPtr<ADatasmithSceneActor>> DataSmithSceneActorsSet;

	int32 ReplaceActorsSetIndex = 0;

	TArray<TSoftObjectPtr<AReplaceActorBase>> ReplaceActorsSet;

	int32 DisplayAryIndex = 0;

	TArray<AActor*> DisplayAry;
	

	int32 RelatedActorsIndex = 0;
};

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_FloorSplit : public UGT_BatchBase
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool
		)>;

	UGT_FloorSplit(
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

	/**
	 * 是否清除之前的
	 */
	bool bClearPrevious = false;

	int32 HeightInterval = 500;

	float MoveDuration = 1.f;

	FOnEnd OnEnd;

protected:
	virtual bool ProcessTask(
		float DeltaTime
		) override;

private:
	bool ProcessTask_Sort();

	bool ProcessTask_ConfirmConditional();

	bool ProcessTask_Display();

	bool ProcessTask_Move(
		float DeltaTime
		);

	enum class EStep
	{
		kSort,
		kConfirmConditional,
		kDisplay,
		kMove,
		kComplete,
	};

	EStep Step = EStep::kSort;

	/**
	 * 等待显示
	 */
	int32 FilterIndex = 0;

	std::map<AActor*, int32> FilterCount;


	int32 DataSmithSceneActorsSetIndex = 0;

	TMap<int32, TArray<TSoftObjectPtr<ADatasmithSceneActor>>> DataSmithSceneActorsSet;

	int32 ReplaceActorsSetIndex = 0;

	TMap<int32, TArray<TSoftObjectPtr<AReplaceActorBase>>> ReplaceActorsSet;


	int32 HideDataSmithSceneActorsSetIndex = 0;

	TArray<TSoftObjectPtr<ADatasmithSceneActor>> HideDataSmithSceneActorsSet;

	int32 HideRePlaceActorsSetIndex = 0;

	TArray<TSoftObjectPtr<AReplaceActorBase>> HideReplaceActorsSet;


	float ConsumeTime = 0.f;
};

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_QuitFloorSplit : public UGT_BatchBase
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool
		)>;

	UGT_QuitFloorSplit(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void OnDestroy(
		bool bInOwnerFinished
		) override;

	USceneInteractionWorldSystem* SceneInteractionWorldSystemPtr = nullptr;

	/**
	 * 是否清除之前的
	 */
	bool bClearPrevious = false;

	int32 HeightInterval = 500;

	float MoveDuration = 1.f;

	FOnEnd OnEnd;

protected:
	virtual bool ProcessTask(
		float DeltaTime
		) override;

private:
	bool ProcessTask_Sort();

	bool ProcessTask_Move(
		float DeltaTime
		);

	enum class EStep
	{
		kSort,
		kMove,
		kComplete,
	};

	EStep Step = EStep::kSort;

	/**
	 * 等待显示
	 */
	int32 FilterIndex = 0;

	std::map<AActor*, int32> FilterCount;


	int32 DataSmithSceneActorsSetIndex = 0;

	TMap<int32, TArray<TSoftObjectPtr<ADatasmithSceneActor>>> DataSmithSceneActorsSet;

	int32 ReplaceActorsSetIndex = 0;

	TMap<int32, TArray<TSoftObjectPtr<AReplaceActorBase>>> ReplaceActorsSet;


	float ConsumeTime = 0.f;
};

#pragma endregion
