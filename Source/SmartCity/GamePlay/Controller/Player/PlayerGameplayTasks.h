// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>
#include <map>
#include <set>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "GameplayTasksComponent.h"
#include "GenerateTypes.h"
#include "LogWriter.h"
#include "SceneInteractionWorldSystem.h"

#include "PlayerGameplayTasks.generated.h"

class UGT_CameraTransform;
class AViewerPawnBase;
class USceneInteractionWorldSystem;
class ADatasmithSceneActor;
class AReplaceActorBase;
class UGT_RuntimeTask;
class UGameplayTaskBase;
class ASceneElementBase;
class ASceneElement_DeviceBase;
class ASceneElement_Space;
class ASceneElementCategory;

struct FSceneElementMap;

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
	GameplayTaskType* StartGameplayTask(
		bool bBreakRuntimeTask,
		const std::function<void(
			GameplayTaskType*


			
			)>& Func
		);

	TArray<TObjectPtr<UGameplayTaskBase>> RuntimeTaskAry;
};

template <typename GameplayTaskType>
GameplayTaskType* UPlayerControllerGameplayTasksComponent::StartGameplayTask(
	bool bBreakRuntimeTask,
	const std::function<void(
		GameplayTaskType*


		
		)>& Func
	)
{
	if (bBreakRuntimeTask)
	{
		if constexpr (std::same_as<GameplayTaskType, UGT_InitializeSceneActors>)
		{
		}
		else
		{
			const auto TempKnownTasks = KnownTasks;
			for (auto Iter : TempKnownTasks)
			{
				if (Iter->IsA(UGT_CameraTransform::StaticClass()) || Iter->IsA(UGT_RuntimeTask::StaticClass()))
				{
					Iter->EndTask();
				}
			}
		}
	}

	if (KnownTasks.IsEmpty())
	{
	}
	else
	{
		PRINTINVOKEWITHSTR(FString( TEXT("")));
	}

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

	return GameplayTaskPtr;
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

	virtual void Activate() override;

	virtual void OnDestroy(
		bool bInOwnerFinished
		) override;

	FGameplayTag SeatTag;
};

UCLASS()
class SMARTCITY_API UGT_CameraTransformByPawnViewer : public UGT_CameraTransform
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

	TObjectPtr<AViewerPawnBase> ViewerPawnPtr = nullptr;
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

	FString ID;

	TWeakObjectPtr<ASceneElementBase> TargetDevicePtr = nullptr;
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
	bool ProcessTask_StructItemSet(
		ASceneElementCategory* SceneElementCategoryPtr,
		FSceneElementMap& AllReference
		);

	bool ProcessTask_InnerStructItemSet(
		ASceneElementCategory* SceneElementCategoryPtr,
		FSceneElementMap& AllReference);

	bool ProcessTask_SoftDecorationItemSet(
		ASceneElementCategory* SceneElementCategoryPtr,
		FSceneElementMap& AllReference);

	bool ProcessTask_SpaceItemSet(
		ASceneElementCategory* SceneElementCategoryPtr,
		FSceneElementMap& AllReference);

	bool ProcessTask_Lanscape(
		ASceneElementCategory* SceneElementCategoryPtr,
		TSet<TSoftObjectPtr<ADatasmithSceneActor>> &DatasmithSceneActorSet
		);

private:

	bool ProcessTask_Generic(
		ASceneElementCategory* SceneElementCategoryPtr,
		FSceneElementMap& AllReference,
		FSceneElementFilter &StructItemSet
		);

};

#pragma endregion

#pragma region 场景对象切换

UCLASS()
class SMARTCITY_API UGT_RuntimeTask : public UGT_BatchBase
{
	GENERATED_BODY()

public:
};

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_SwitchSceneElement_Base : public UGT_RuntimeTask
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool,
		UGT_SwitchSceneElement_Base*


		
		)>;

	UGT_SwitchSceneElement_Base(
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

	virtual bool ProcessTask_Display();

	virtual bool ProcessTask_Hiden();

	virtual bool ProcessTask_ConfirmConditional();

	virtual bool ProcessTask_SwitchState();

	virtual bool ProcessTask_SwitchState_Elevator();

	enum class EStep
	{
		kDisplay,
		kHiden,
		kConfirmConditional,
		kSwitchState,
		kSwitchState_Elevator,
		kComplete,
	};

	EStep Step = EStep::kDisplay;

protected:
	TArray<TObjectPtr<ASceneElementBase>> NeedDisplayAry;

	int32 DisplayAryIndex = 0;

	TArray<TObjectPtr<ASceneElementBase>> NeedHideAry;

	int32 HideAryIndex = 0;
};

UCLASS()
class SMARTCITY_API UGT_SwitchSceneElement_Tower : public UGT_SwitchSceneElement_Base
{
	GENERATED_BODY()

public:
	bool bDisplayInnerStruct = true;

protected:
private:
	virtual bool ProcessTask_Display() override;

	virtual bool ProcessTask_Hiden() override;

	virtual bool ProcessTask_ConfirmConditional() override;

	virtual bool ProcessTask_SwitchState() override;

	virtual bool ProcessTask_SwitchState_Elevator() override;
	
	TArray<TObjectPtr<ASceneElementBase>> BuildingsAry;

	int32 BuildingsAryIndex = 0;

};

UCLASS()
class SMARTCITY_API UGT_SwitchSceneElement_Floor : public UGT_SwitchSceneElement_Base
{
	GENERATED_BODY()

public:
	virtual bool ProcessTask_Display() override;

	virtual bool ProcessTask_Hiden() override;

};

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_SwitchSceneElement_Space : public UGT_SwitchSceneElement_Base
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	virtual bool ProcessTask_Display() override;

	virtual bool ProcessTask_Hiden() override;

	virtual bool ProcessTask_SwitchState() override;

	TWeakObjectPtr<ASceneElement_Space> SceneElementPtr = nullptr;

	FGameplayTag FloorTag;
};

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_SwitchSceneElement_Device : public UGT_SwitchSceneElement_Base
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	virtual bool ProcessTask_Display() override;

	virtual bool ProcessTask_Hiden() override;

	virtual bool ProcessTask_SwitchState() override;

	TSet<TObjectPtr<ASceneElementBase>> SceneElementSet;

	FGameplayTag FloorTag;
};

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_SwitchSceneElement_SpecialArea : public UGT_SwitchSceneElement_Base
{
	GENERATED_BODY()

public:
	virtual bool ProcessTask_Display() override;

	virtual bool ProcessTask_Hiden() override;

	virtual bool ProcessTask_SwitchState() override;

	TArray<TObjectPtr<ASceneElementBase>> PriorityHideFloorAryAry;

	int32 PriorityHideFloorAryIndex = 0;

	TSet<FGameplayTag> FloorSet;

	TSet<FGameplayTag> PriorityHideFloorSet;
};

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_FloorSplit : public UGT_RuntimeTask
{
	GENERATED_BODY()

public:
};

/**
 * 
 */
UCLASS()
class SMARTCITY_API UGT_QuitFloorSplit : public UGT_RuntimeTask
{
	GENERATED_BODY()

public:
};

#pragma endregion
