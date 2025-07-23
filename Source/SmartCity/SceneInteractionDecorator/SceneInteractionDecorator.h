// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"

#include "Tools.h"

class SMARTCITY_API FDecoratorBase
{
public:
	GENERATIONCLASSINFOONLYTHIS(FDecoratorBase);
	
	FDecoratorBase(
		FGameplayTag InMainDecoratorType,
		FGameplayTag InBranchDecoratorType
		);

	virtual ~FDecoratorBase();

	virtual void Entry();

	virtual void OnOtherDecoratorEntry(const TSharedPtr<FDecoratorBase>&NewDecoratorSPtr);

	/**
	 * 
	 * @param OperatorType 
	 * @return 是否消耗事件
	 */
	virtual bool Operation(
		EOperatorType OperatorType
		);

	FGameplayTag GetMainDecoratorType() const;

	FGameplayTag GetBranchDecoratorType() const;

protected:
	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors
		);

	FGameplayTag MainDecoratorType;

	FGameplayTag BranchDecoratorType;
};

#pragma region 模式
/**
 * 漫游时
 */
class SMARTCITY_API FTour_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FTour_Decorator,
	                    FDecoratorBase
	                   );

	FTour_Decorator();

	virtual void Entry() override;

	virtual bool Operation(
		EOperatorType OperatorType
		) override;
};

/**
 * 楼层 展开
 */
class SMARTCITY_API FSplitFloorMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FSplitFloorMode_Decorator,
	                    FDecoratorBase
	                   );

	FSplitFloorMode_Decorator();

	virtual void Entry() override;

	virtual bool Operation(
		EOperatorType OperatorType
		) override;
};

/**
 * 选择“场景”模式
 */
class SMARTCITY_API FSceneMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FSceneMode_Decorator,
	                    FDecoratorBase
	                   );

	FSceneMode_Decorator();

	virtual void Entry() override;

	virtual bool Operation(
		EOperatorType OperatorType
		) override;
};

/**
 * 选择“雷达控制”模式
 */
class SMARTCITY_API FRadarMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FRadarMode_Decorator,
	                    FDecoratorBase
	                   );

	FRadarMode_Decorator();

	virtual ~FRadarMode_Decorator() ;

	virtual void Entry() override;

	virtual bool Operation(
		EOperatorType OperatorType
		) override;

private:
	void RadarQuery();

	FTimerHandle QueryTimerHadnle;
};

/**
 * 选择“强电”模式
 */
class SMARTCITY_API FQDMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FQDMode_Decorator,
	                    FDecoratorBase
	                   );

	FQDMode_Decorator();

private:
};

#pragma endregion

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
		const FGameplayTag& Interaction_Area
		);

	virtual void Entry() override;

	virtual void OnOtherDecoratorEntry(const TSharedPtr<FDecoratorBase>&NewDecoratorSPtr) override;

protected:
	void UpdateDisplay();
	
	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors
		) override;

	FGameplayTag CurrentInteraction_Area;
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
		const FGameplayTag& Interaction_Area
		);

	virtual void Entry() override;

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
		const FGameplayTag& Interaction_Area
		);

	virtual void Entry() override;

	virtual bool Operation(
		EOperatorType OperatorType
		) override;

protected:
	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors
		) override;

private:
	void AddFocusDevice(
		AActor* DevicePtr
		);

	void RemoveFocusDevice(
		AActor* DevicePtr
		);

	void ClearFocus();

	TSet<AActor*> FocusActors;
};

#pragma endregion
