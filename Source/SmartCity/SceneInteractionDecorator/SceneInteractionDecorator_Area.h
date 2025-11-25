// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"
#include "SceneElement_AccessControl.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionDecorator_Option.h"
#include "GameOptions.h"

#include "Tools.h"

class AViewerPawnBase;
class ASceneElement_Space;
class ABuilding_Floor_Mask;
class AFloorHelper;
class USceneInteractionWorldSystem;
class ASceneElement_PWR_Pipe;
class APersonMark;
class UGT_SwitchSceneElement_Tower;
class ASceneElement_DeviceBase;
class AFireMark;
class FArea_Decorator;

/**
 * 
 */
class SMARTCITY_API FArea_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FArea_Decorator,
	                    FDecoratorBase
	                   );

	FArea_Decorator(
		);

	virtual void Entry() override;

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	virtual void OnOtherDecoratorQuit(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	virtual void InitialType(
		FGameplayTag InMainDecoratorType,
		FGameplayTag InBranchDecoratorType
		) override;

	FGameplayTag GetCurrentInteraction_Area() const;

protected:
	void UpdateDisplay();

	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		UGT_SwitchSceneElement_Base* TaskPtr
		) override;

	FGameplayTag CurrentInteraction_Area;

	TArray<UGT_SwitchSceneElement_Tower*> SwitchSceneElementStateAry;
};

/**
 * 选中“整楼”区域
 */
class SMARTCITY_API FViewTower_Decorator : public FArea_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FViewTower_Decorator,
	                    FArea_Decorator
	                   );

	FViewTower_Decorator(
		);

	virtual void Entry() override;

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	virtual bool Operation(
		EOperatorType OperatorType
		) override;

	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		UGT_SwitchSceneElement_Base* TaskPtr
		) override;
};

/**
 * 选中“整楼”区域
 */
class SMARTCITY_API FViewPeriphery_Decorator : public FArea_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FViewPeriphery_Decorator,
	                    FArea_Decorator
	                   );

	FViewPeriphery_Decorator(
		);

	virtual void Entry() override;

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	virtual bool Operation(
		EOperatorType OperatorType
		) override;

	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		UGT_SwitchSceneElement_Base* TaskPtr
		) override;
};

/**
 * 楼层 展开
 */
class SMARTCITY_API FSplitFloor_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FSplitFloor_Decorator,
	                    FDecoratorBase
	                   );

	FSplitFloor_Decorator(
		);

	virtual void Entry() override;

	virtual void Quit() override;

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	virtual bool NeedAsync() const override;

	virtual bool Operation(
		EOperatorType OperatorType
		) override;
};

/**
 * 选中“单楼”区域
 */
class SMARTCITY_API FFloor_Decorator : public FArea_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FFloor_Decorator,
	                    FArea_Decorator
	                   );

	FFloor_Decorator(
		);

	virtual ~FFloor_Decorator();

	virtual void Entry() override;

	virtual void Quit() override;

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	virtual bool Operation(
		EOperatorType OperatorType
		) override;

	void UpdateParam(
		const TMap<FString, TMap<FString, FString>>& ExtensionParamMap
		);

protected:
	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		UGT_SwitchSceneElement_Base* TaskPtr
		) override;

	void Process();

	void AdjustCamera() const;

private:
	TObjectPtr<ABuilding_Floor_Mask> Building_Floor_Mask = nullptr;
};

/**
 * 选中“单楼”区域
 */
class SMARTCITY_API FViewDevice_Decorator : public FArea_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FViewDevice_Decorator,
	                    FArea_Decorator
	                   );

	FViewDevice_Decorator(
		);

	virtual void Entry() override;

	virtual void ReEntry() override;

	virtual void Quit() override;

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	TWeakObjectPtr<ASceneElement_DeviceBase> SceneElementPtr = nullptr;

private:
	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		UGT_SwitchSceneElement_Base* TaskPtr
		) override;

	void Process();

	void AdjustCamera() const;

	TObjectPtr<ABuilding_Floor_Mask> Building_Floor_Mask = nullptr;
};

class SMARTCITY_API FViewSpace_Decorator : public FArea_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FViewSpace_Decorator,
	                    FArea_Decorator
	                   );

	FViewSpace_Decorator(
		);

	virtual void Entry() override;

	virtual void ReEntry() override;

	virtual void Quit() override;

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	void UpdateParam(
		const TMap<FString, TMap<FString, FString>>& ExtensionParamMap
		);

	FGameplayTag Floor;

	TWeakObjectPtr<ASceneElement_Space> SceneElementPtr = nullptr;

private:
	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		UGT_SwitchSceneElement_Base* TaskPtr
		) override;

	void Process();

	void AdjustCamera() const;

	TObjectPtr<ABuilding_Floor_Mask> Building_Floor_Mask = nullptr;
};

class SMARTCITY_API FViewSpecialArea_Decorator : public FArea_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FViewSpecialArea_Decorator,
	                    FArea_Decorator
	                   );

	virtual void Entry() override;

	virtual void ReEntry() override;

	virtual void Quit() override;

	TSet<FGameplayTag> FloorSet;

	TSet<FGameplayTag> PriorityHideFloorSet;

	FGameplayTag AreaTag;

	FString Seat;

	TWeakObjectPtr<AViewerPawnBase> ViewerPawnBasePtr = nullptr;

private:
	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		UGT_SwitchSceneElement_Base* TaskPtr
		) override;

	void Process();

	void AdjustCamera() const;
};
