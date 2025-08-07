// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"

#include "Tools.h"

class USceneInteractionWorldSystem;
class ASceneElement_PWR_Pipe;
class APersonMark;

class SMARTCITY_API FDecoratorBase
{
public:
	friend USceneInteractionWorldSystem;

	GENERATIONCLASSINFOONLYTHIS(FDecoratorBase);

	FDecoratorBase(
		FGameplayTag InMainDecoratorType,
		FGameplayTag InBranchDecoratorType
		);

	virtual ~FDecoratorBase();

	virtual void Entry();

	virtual void Quit();

	virtual bool NeedAsync() const;

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		);

	virtual void OnOtherDecoratorQuit(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		);

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

	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors
		);

protected:
	TDelegate<void()> OnAsyncQuitComplete;

	FGameplayTag MainDecoratorType;

	FGameplayTag BranchDecoratorType;
};

#pragma region 模式
/**
 * “空”模式
 */
class SMARTCITY_API FEmpty_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FEmpty_Decorator,
	                    FDecoratorBase
	                   );

	FEmpty_Decorator();
};

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
class SMARTCITY_API FELVRadarMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FELVRadarMode_Decorator,
	                    FDecoratorBase
	                   );

	FELVRadarMode_Decorator();

	virtual ~FELVRadarMode_Decorator();

	virtual void Entry() override;

	virtual void Quit() override;

	virtual bool Operation(
		EOperatorType OperatorType
		) override;

private:
	void RadarQuery();

	void QueryComplete();
	
	FTimerHandle QueryTimerHadnle;

	TArray<APersonMark*>GeneratedMarkers;
};

/**
 * 选择“强电”模式
 */
class SMARTCITY_API FPWRMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FPWRMode_Decorator,
	                    FDecoratorBase
	                   );

	FPWRMode_Decorator();

private:
};

/**
 * 选择能耗
 */
class SMARTCITY_API FPWREnergyMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FPWREnergyMode_Decorator,
	                    FDecoratorBase
	                   );

	FPWREnergyMode_Decorator();

	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors
		) override;

private:
	TSet<ASceneElement_PWR_Pipe*> PipeActors;
};

/**
 * 选择能耗
 */
class SMARTCITY_API FPWRHVACMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FPWRHVACMode_Decorator,
	                    FDecoratorBase
	                   );

	FPWRHVACMode_Decorator();
};

/**
 * 选择“门禁”模式
 */
class SMARTCITY_API FAccessControlMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FPWRMode_Decorator,
	                    FDecoratorBase
	                   );

	FAccessControlMode_Decorator();

	virtual void Entry() override;

private:
};

/**
 * 选择“电梯”模式
 */
class SMARTCITY_API FElevatorMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FElevatorMode_Decorator,
	                    FDecoratorBase
	                   );

	FElevatorMode_Decorator();

	virtual void Entry() override;

	virtual void Quit() override;

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

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	virtual void OnOtherDecoratorQuit(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	FGameplayTag GetCurrentInteraction_Area()const;
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

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	virtual bool Operation(
		EOperatorType OperatorType
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
		const FGameplayTag& Interaction_Area
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
		const FGameplayTag& Interaction_Area
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
		const TSet<AActor*>& InActors
		) override;

private:
};

#pragma endregion
