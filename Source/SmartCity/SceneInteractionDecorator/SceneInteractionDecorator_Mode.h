// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"
#include "SceneInteractionDecorator.h"

#include "Tools.h"

class AFloorHelper;
class USceneInteractionWorldSystem;
class ASceneElement_PWR_Pipe;
class APersonMark;
class UGT_SwitchSceneElement_Tower;
class ASceneElement_DeviceBase;
class AFireMark;
class FArea_Decorator;

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
		UGT_SwitchSceneElement_Base* TaskPtr
		) override;

private:

	void Spawn(const TSharedPtr<FArea_Decorator>& AreaDecoratorSPtr);
	
	void Clear();
	
	TSet<AFireMark*>FireMarkSet;
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
		UGT_SwitchSceneElement_Base* TaskPtr
		) override;

	// TMap<FString, ASceneElement_PWR_Pipe*>IDMap;
	
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
 * 选择安全模式
 */
class SMARTCITY_API FSafeManagementMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FSafeManagementMode_Decorator,
	                    FDecoratorBase
	                   );

	FSafeManagementMode_Decorator();
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
class SMARTCITY_API FHVACMode_Decorator : public FDeviceManaggerPWRMode_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FHVACMode_Decorator,
	                    FDeviceManaggerPWRMode_Decorator
	                   );

	FHVACMode_Decorator();
};

/**
 * 选择照明
 */
class SMARTCITY_API FLightingMode_Decorator : public FDeviceManaggerPWRMode_Decorator
{
public:
	GENERATIONCLASSINFO(
	                    FLightingMode_Decorator,
	                    FDeviceManaggerPWRMode_Decorator
	                   );

	FLightingMode_Decorator();

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
		UGT_SwitchSceneElement_Base* TaskPtr
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
		);

	virtual void Entry() override;

	TWeakObjectPtr<ASceneElement_DeviceBase>TargetDevicePtr = nullptr;
};


/**
 * 按区域控制
 */
class SMARTCITY_API FBatchControlMode_Decorator : public FDecoratorBase
{
public:
	GENERATIONCLASSINFO(
	                    FBatchControlMode_Decorator,
	                    FDecoratorBase
	                   );

	FBatchControlMode_Decorator(
		);

	virtual void Entry() override;
	
	virtual void ReEntry() override;

	virtual void OnOtherDecoratorEntry(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) override;
	
	void Initial();

	/**
	 * Key:设备类型
	 * Value：参数
	 */
	TMap<FString, TMap<FString, FString>> ExtensionParamMap;

	TSet<TObjectPtr<ASceneElement_DeviceBase> >SceneElementSet;

	FGameplayTag FloorTag;
	
protected:
	virtual void OnUpdateFilterComplete(
		bool bIsOK,
		UGT_SwitchSceneElement_Base* TaskPtr
		) override;

	void Process();
};

