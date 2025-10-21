// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionDecorator_Option.h"

#include "Tools.h"

class ABuilding_Floor_Mask;
class AFloorHelper;
class USceneInteractionWorldSystem;
class ASceneElement_PWR_Pipe;
class APersonMark;
class UGT_SwitchSceneElementState;
class ASceneElement_DeviceBase;
class AFireMark;
class FArea_Decorator;

#pragma region 区域

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
		FGameplayTag InBranchDecoratorType) override;
	
	FGameplayTag GetCurrentInteraction_Area() const;

protected:
	void UpdateDisplay();

	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors,
		UGT_SwitchSceneElementState* TaskPtr
		) override;

	FGameplayTag CurrentInteraction_Area;

	TArray<UGT_SwitchSceneElementState*> SwitchSceneElementStateAry;
};

/**
 * 选中“整楼”区域
 */
class SMARTCITY_API FExternalWall_Decorator : public FArea_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FExternalWall_Decorator,
	                    FArea_Decorator
	                   );

	FExternalWall_Decorator(
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
		const TSet<AActor*>& InActors,
		UGT_SwitchSceneElementState* TaskPtr
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

protected:
	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors,
		UGT_SwitchSceneElementState* TaskPtr
		) override;

private:
	TSet<AActor*> PreviousActors;

	TObjectPtr<ABuilding_Floor_Mask>Building_Floor_Mask = nullptr;
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

	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors,
		UGT_SwitchSceneElementState* TaskPtr
		) override;

	TWeakObjectPtr<ASceneElement_DeviceBase>SceneElementPtr = nullptr;

private:

	void Process();
	
	void AdjustCamera()const;
	
	TWeakObjectPtr<ASceneElement_DeviceBase>PreviousSceneElementPtr = nullptr;

	TObjectPtr<AFloorHelper>PreviousFloorHelper = nullptr;

	FInteraction_Decorator::FConfig Config;
};

class SMARTCITY_API FViewSpecialArea_Decorator : public FArea_Decorator
{
public:
	GENERATIONCLASSINFO(
						FViewSpecialArea_Decorator,
						FArea_Decorator
					   );

};

#pragma endregion
