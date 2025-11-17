// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "SceneInteractionDecorator.h"
#include "SceneInteractionDecorator_Option.h"
#include "SceneElementBase.h"

#include "SceneInteractionWorldSystem.generated.h"

class FDecoratorBase;
class UGT_InitializeSceneActors;
class UGT_SwitchSceneElement_Tower;
class UGT_SwitchSceneElement_Space;
class URouteMarker;
class ASceneElementBase;
class ASceneElement_Space;

/*
 * 
 */
UCLASS()
class SMARTCITY_API USceneInteractionWorldSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	friend UGT_InitializeSceneActors;
	friend UGT_SwitchSceneElement_Tower;

	static USceneInteractionWorldSystem* GetInstance();

	TSharedPtr<FDecoratorBase> GetInteractionModeDecorator() const;

	TSharedPtr<FDecoratorBase> GetDecorator(
		const FGameplayTag& Interaction
		) const;

	FGameplayTagContainer GetAllInteractionTags() const;

	/**
	 * 切换交互选项
	 * @param Interaction_Mode 
	 */
	void SwitchInteractionOption(
		const FGameplayTag& Interaction_Mode
		);

	void SetInteractionOption(
		const FGameplayTag& Interaction_Mode,
		const std::function<void(
			const TSharedPtr<FInteraction_Decorator>&


			
			)>& Func,
		bool bImmediatelyUpdate
		);

	/**
	 * 切换交互模式
	 * @param Interaction_Mode 
	 */
	void SwitchInteractionMode(
		const FGameplayTag& Interaction_Mode
		);

	/**
	 * 切换交互区域
	 * @param Interaction_Area 
	 */
	void SwitchInteractionArea(
		const FGameplayTag& Interaction_Area,
		const std::function<void(

			const TSharedPtr<FDecoratorBase>&


			
			)>& Func = nullptr
		);

	/**
	 * 直接修改状态
	 * @param FocusActorsAry 
	 * @param FilterTags 
	 */
	void SwitchInteractionType(
		const TSet<ASceneElementBase*>& FocusActorsAry,
		const FSceneElementConditional& FilterTags
		);

	void Operation(
		EOperatorType OperatorType
		) const;

	UGT_SwitchSceneElement_Base* UpdateFilter_Tower(
		const FSceneElementConditional& FilterTags,
		bool bBreakRuntimeTask,
		const TMulticastDelegate<void(
			bool,
			UGT_SwitchSceneElement_Base*
			
			)>& OnEnd
		);

	UGT_SwitchSceneElement_Base* UpdateFilter_Floor(
		const FSceneElementConditional& FilterTags,
		bool bBreakRuntimeTask,
		const TMulticastDelegate<void(
			bool,
			UGT_SwitchSceneElement_Base*
			
			)>& OnEnd
		);

	UGT_SwitchSceneElement_Base* UpdateFilter_Space(
		const FSceneElementConditional& FilterTags,
		bool bBreakRuntimeTask,
		const TMulticastDelegate<void(
			bool,
			UGT_SwitchSceneElement_Base*
			
			)>& OnEnd,
		TWeakObjectPtr<ASceneElement_Space> SceneElementPtr
		);

	UGT_SwitchSceneElement_Base* UpdateFilter_Device(
		const FSceneElementConditional& FilterTags,
		bool bBreakRuntimeTask,
		const TMulticastDelegate<void(
			bool,
			UGT_SwitchSceneElement_Base*
			
			)>& OnEnd,
		TWeakObjectPtr<ASceneElement_DeviceBase> SceneElementPtr
		);

	UGT_SwitchSceneElement_Base* UpdateFilter_SpeacialArea(
		const FSceneElementConditional& FilterTags,
		bool bBreakRuntimeTask,
		const TMulticastDelegate<void(
			bool,
			UGT_SwitchSceneElement_Base*
			
			)>& OnEnd,

		const TSet<FGameplayTag>& FloorSet,
		const TSet<FGameplayTag>& PriorityHideFloorSet

		);

	void InitializeSceneActors();

	TWeakObjectPtr<ASceneElementBase> FindSceneActor(
		const FString& ID
		) const;

	void SetSceneActor(
		const FString& ID,
		ASceneElementBase*SceneElementPtr
		) ;

	FString GetName(
		AActor* DevicePtr
		) const;

	/**
	 * 根据选择的装饰器获取所有过滤条件
	 * @return 
	 */
	FGameplayTagContainer GetAllFilterTags() const;

	void AddFocusActor(
		AActor* ActorPtr
		);

	void ClearFocus();

	void ClearRouteMarker();

	template <typename Decorator>
	void SwitchDecoratorImp(
		const FGameplayTag& MainTag,
		const FGameplayTag& BranchTag,
		const std::function<void(

			const TSharedPtr<FDecoratorBase>&


			
			)>& Func = nullptr
		);

private:
	void NotifyOtherDecoratorsWhenEntry(
		const FGameplayTag& MainTag,
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) const;

	void NotifyOtherDecoratorsWhenQuit(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) const;

	/**
	 * Key:装饰的类型
	 * Value：当前装饰
	 */
	TMap<FGameplayTag, TSharedPtr<FDecoratorBase>> DecoratorLayerAssetMap;

	TArray<TSharedPtr<FDecoratorBase>> DecoratorLayerCache;

	TMap<FString, ASceneElementBase*> SceneElementMap;

	TSet<AActor*> FocusActors;

	UPROPERTY(Transient)
	TMap<AActor*, URouteMarker*> RouteMarkers;
};

template <typename Decorator>
void USceneInteractionWorldSystem::SwitchDecoratorImp(
	const FGameplayTag& MainTag,
	const FGameplayTag& BranchTag,
	const std::function<void(
		const TSharedPtr<FDecoratorBase>&


		
		)>& Func
	)
{
	if (DecoratorLayerAssetMap.Contains(MainTag))
	{
		auto OldDecoratorSPtr = DecoratorLayerAssetMap[MainTag];
		if (OldDecoratorSPtr->GetBranchDecoratorType() == BranchTag)
		{
			if (Func)
			{
				Func(OldDecoratorSPtr);
			}

			OldDecoratorSPtr->ReEntry();

			return;
		}

		if (OldDecoratorSPtr->NeedAsync())
		{
			OldDecoratorSPtr->Quit();
			NotifyOtherDecoratorsWhenQuit(OldDecoratorSPtr);

			OldDecoratorSPtr->OnAsyncQuitComplete.BindLambda(
			                                                 [this, MainTag]()
			                                                 {
				                                                 auto DecoratorSPtr = MakeShared<Decorator>();

				                                                 NotifyOtherDecoratorsWhenEntry(MainTag, DecoratorSPtr);

				                                                 DecoratorLayerAssetMap.Add(
					                                                  MainTag,
					                                                  DecoratorSPtr
					                                                 );

				                                                 DecoratorSPtr->Entry();
			                                                 }
			                                                );
		}
		else
		{
			auto DecoratorSPtr = MakeShared<Decorator>();

			DecoratorSPtr->InitialType(MainTag, BranchTag);

			if (Func)
			{
				Func(DecoratorSPtr);
			}

			NotifyOtherDecoratorsWhenEntry(MainTag, DecoratorSPtr);

			DecoratorLayerAssetMap.Add(
			                           MainTag,
			                           DecoratorSPtr
			                          );

			if (OldDecoratorSPtr)
			{
				// 下一帧移除，避免在此装饰里面进行修改容器的操作
				// 避免悬空
				DecoratorLayerCache.Add(OldDecoratorSPtr);
				GetWorldImp()->GetTimerManager().SetTimerForNextTick(
				                                                     [this]()
				                                                     {
					                                                     DecoratorLayerCache.Empty();
				                                                     }
				                                                    );

				OldDecoratorSPtr->Quit();
				NotifyOtherDecoratorsWhenQuit(OldDecoratorSPtr);
			}

			DecoratorSPtr->Entry();
		}
	}
	else
	{
		auto DecoratorSPtr = MakeShared<Decorator>();

		DecoratorSPtr->InitialType(MainTag, BranchTag);

		if (Func)
		{
			Func(DecoratorSPtr);
		}

		NotifyOtherDecoratorsWhenEntry(MainTag, DecoratorSPtr);

		DecoratorLayerAssetMap.Add(
		                           MainTag,
		                           DecoratorSPtr
		                          );

		DecoratorSPtr->Entry();
	}
}
