// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "Tools.h"

enum class EDecoratorType
{
	kMode,
	kMode_Tour,
	kMode_Scene,

	kArea,
	kArea_ExternalWall,
	kArea_Floor,
	kArea_Space,

	kNone,
};

enum class EOperatorType
{
	kLeftMouseButton,
	
	kRightMouseButton,

	kNone,
};

class SMARTCITY_API FDecoratorBase
{
public:
	FDecoratorBase(EDecoratorType InMainDecoratorType,
	               EDecoratorType InBranchDecoratorType);

	virtual ~FDecoratorBase();

	virtual void Entry() const;

	virtual void Operation(EOperatorType OperatorType) const;

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
class SMARTCITY_API FTourDecorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(FTourDecorator,
	                    FDecoratorBase);

	FTourDecorator();

	virtual void Operation(EOperatorType OperatorType) const override;
};

/**
 * 选择“场景”模式
 */
class SMARTCITY_API FSceneMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(FSceneMode_Decorator,
	                    FDecoratorBase);

	FSceneMode_Decorator();

	virtual void Operation(EOperatorType OperatorType) const override;
};

#pragma endregion

#pragma region 区域

/**
 * 选中“整楼”区域
 */
class SMARTCITY_API FArea_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(FArea_Decorator,
	                    FDecoratorBase);

	FArea_Decorator(EDecoratorType InBranchDecoratorType,
	                const FGameplayTag& Interaction_Area);

	virtual void Entry() const override;

protected:
	FGameplayTag CurrentInteraction_Area;

private:
	void SwitchViewArea(const TSet<TSoftObjectPtr<UDataLayerAsset>>& DalaLayerAssetMap) const;
};

/**
 * 选中“整楼”区域
 */
class SMARTCITY_API FExternalWall_Decorator : public FArea_Decorator
{
public:
	GENERATIONCLASSINFO(FExternalWall_Decorator,
	                    FArea_Decorator);

	FExternalWall_Decorator(
		const FGameplayTag& Interaction_Area);

	virtual void Operation(EOperatorType OperatorType) const override;
};

/**
 * 选中“单楼”区域
 */
class SMARTCITY_API FFloor_Decorator : public FArea_Decorator
{
public:
	GENERATIONCLASSINFO(FFloor_Decorator,
	                    FArea_Decorator);

	FFloor_Decorator(
		const FGameplayTag& Interaction_Area);

	virtual void Operation(EOperatorType OperatorType) const override;
};

#pragma endregion
