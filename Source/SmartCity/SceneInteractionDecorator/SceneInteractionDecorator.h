// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"

#include "Tools.h"

class USceneInteractionWorldSystem;
class ASceneElement_PWR_Pipe;
class APersonMark;
class UGT_SwitchSceneElementState;
class ASceneElement_DeviceBase;

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
		const TSet<AActor*>& InActors,
		UGT_SwitchSceneElementState* TaskPtr
		);

protected:
	void IncreaseWaitTaskCount();

	void DecreaseWaitTaskCount();

	int32 GetWaitTaskCount() const;

	TDelegate<void()> OnAsyncQuitComplete;

	FGameplayTag MainDecoratorType;

	FGameplayTag BranchDecoratorType;

private:
	int32 WaitTaskCount = 0;
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
 * 选择“火灾-逃生路线”模式
 */
class SMARTCITY_API FEmergencyMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FEmergencyMode_Decorator,
	                    FDecoratorBase
	                   );

	FEmergencyMode_Decorator();

	virtual void Entry() override;

	virtual void Quit() override;

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	virtual void OnOtherDecoratorQuit(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors,
		UGT_SwitchSceneElementState* TaskPtr
		) override;
};

/**
 * 选择能耗
 */
class SMARTCITY_API FEnergyMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FEnergyMode_Decorator,
	                    FDecoratorBase
	                   );

	FEnergyMode_Decorator();

	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors,
		UGT_SwitchSceneElementState* TaskPtr
		) override;

	TMap<FString, ASceneElement_PWR_Pipe*>IDMap;
	
private:
	TSet<ASceneElement_PWR_Pipe*> PipeActors;
	
	TSet<ASceneElement_DeviceBase*> OtherDevices;
};

/**
 * 选择环境感知
 */
class SMARTCITY_API FEnvironmentalPerceptionMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FEnvironmentalPerceptionMode_Decorator,
	                    FDecoratorBase
	                   );

	FEnvironmentalPerceptionMode_Decorator();
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
};

/**
 * 选择“设备管理”模式
 */
class SMARTCITY_API FDeviceManaggerMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FDeviceManaggerMode_Decorator,
	                    FDecoratorBase
	                   );

	FDeviceManaggerMode_Decorator();

	FDeviceManaggerMode_Decorator(
		FGameplayTag InBranchDecoratorType
		);
};

/**
 * 选择“强电”模式
 */
class SMARTCITY_API FDeviceManaggerPWRMode_Decorator : public FDeviceManaggerMode_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FDeviceManaggerPWRMode_Decorator,
	                    FDeviceManaggerMode_Decorator
	                   );

	FDeviceManaggerPWRMode_Decorator();

	FDeviceManaggerPWRMode_Decorator(
		FGameplayTag InBranchDecoratorType
		);

private:
};

/**
 * 选择暖通
 */
class SMARTCITY_API FPWRHVACMode_Decorator : public FDeviceManaggerPWRMode_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FPWRHVACMode_Decorator,
	                    FDeviceManaggerPWRMode_Decorator
	                   );

	FPWRHVACMode_Decorator();
};

/**
 * 选择照明
 */
class SMARTCITY_API FPWRLightingMode_Decorator : public FDeviceManaggerPWRMode_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FPWRLightingMode_Decorator,
	                    FDeviceManaggerPWRMode_Decorator
	                   );

	FPWRLightingMode_Decorator();

	virtual void Entry() override;

	virtual void Quit() override;

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;

	virtual void OnOtherDecoratorQuit(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;
};

/**
 * 选择“门禁”模式
 */
class SMARTCITY_API FAccessControlMode_Decorator : public FDeviceManaggerPWRMode_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FAccessControlMode_Decorator,
	                    FDeviceManaggerPWRMode_Decorator
	                   );

	FAccessControlMode_Decorator();

	virtual void Entry() override;

private:
};

/**
 * 选择“遮阳”设备
 */
class SMARTCITY_API FSunShadeMode_Decorator : public FDeviceManaggerPWRMode_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FSunShadeMode_Decorator,
	                    FDeviceManaggerPWRMode_Decorator
	                   );

	FSunShadeMode_Decorator();

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

	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		const TSet<AActor*>& InActors,
		UGT_SwitchSceneElementState* TaskPtr
		) override;

private:
};

/**
 * 选中“单个设备”
 */
class SMARTCITY_API FSingleDeviceMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FSingleDeviceMode_Decorator,
	                    FDecoratorBase
	                   );

	FSingleDeviceMode_Decorator(
		const TObjectPtr<AActor>& TargetDevicePtr
		);

	virtual void Entry() override;

	TObjectPtr<AActor> TargetDevicePtr = nullptr;
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
		const FGameplayTag& Interaction_Area
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
		const TSet<AActor*>& InActors,
		UGT_SwitchSceneElementState* TaskPtr
		) override;

private:
	TSet<AActor*> PreviousActors;
};

#pragma endregion

#pragma region 操作方式

/**
 * 
 */
class SMARTCITY_API FInteraction_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FInteraction_Decorator,
	                    FDecoratorBase
	                   );

	enum class EInteractionType:uint8
	{
		kDevice,
		kSpace,
	};

	FInteraction_Decorator();

	virtual void Entry() override;

	void SwitchIteractionType(
		EInteractionType NewInteractionType
		);

	EInteractionType GetInteractionType() const;

	FGameplayTag GetCurrentWeather() const;

	void SetCurrentWeather(
		const FGameplayTag& WeatherTag
		);

	int32 GetCurrentHour() const;

	void SetCurrentHour(
		int32 Hour
		);

	/**
	 * 墙体透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	int32 WallTranlucent = 100;

	/**
	 * 墙体透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	int32 PillarTranlucent = 100;

	/**
	 * 楼梯透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	int32 StairsTranlucent = 100;

	/**
	 * 幕墙墙体透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	bool bShowCurtainWall = true;

	/**
	 * 是否显示家具
	 */
	bool bShowFurniture = true;

	float ViewPitchMin = 45;

	float ViewPitchMax = 45;

private:
	EInteractionType InteractionType = EInteractionType::kDevice;

	FGameplayTag CurrentWeather;

	int32 CurrentHour;
};

#pragma endregion
