// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"

#include "Tools.h"

class SMARTCITY_API FDecoratorBase
{
public:
	FDecoratorBase(
		EDecoratorType InMainDecoratorType,
		EDecoratorType InBranchDecoratorType
		);

	virtual ~FDecoratorBase();

	virtual void Entry();

	virtual void Operation(
		EOperatorType OperatorType
		) const;

	EDecoratorType GetMainDecoratorType() const;

	EDecoratorType GetBranchDecoratorType() const;

protected:
	EDecoratorType MainDecoratorType = EDecoratorType::kNone;

	EDecoratorType BranchDecoratorType = EDecoratorType::kNone;
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

	virtual void Operation(
		EOperatorType OperatorType
		) const override;
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

	virtual void Operation(
		EOperatorType OperatorType
		) const override;
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

	virtual ~FRadarMode_Decorator();

	virtual void Entry() override;

	virtual void Operation(
		EOperatorType OperatorType
		) const override;

private:
	void RadarQuery();

	FTimerHandle QueryTimerHadnle;
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
		EDecoratorType InBranchDecoratorType,
		const FGameplayTag& Interaction_Area
		);

	virtual void Entry() override;

protected:
	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors
		);

	FGameplayTag CurrentInteraction_Area;

	TSet<AActor*> Actors;

private:
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

	virtual void Operation(
		EOperatorType OperatorType
		) const override;
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

	virtual void Operation(
		EOperatorType OperatorType
		) const override;

protected:
	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors
		) override;
};

#pragma endregion
