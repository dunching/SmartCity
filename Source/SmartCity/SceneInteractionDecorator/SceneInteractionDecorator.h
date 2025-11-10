// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"

#include "Tools.h"

class AFloorHelper;
class USceneInteractionWorldSystem;
class ASceneElement_PWR_Pipe;
class APersonMark;
class UGT_SwitchSceneElement_Base;
class ASceneElement_DeviceBase;
class AFireMark;
class FArea_Decorator;

class SMARTCITY_API FDecoratorBase
{
public:
	friend USceneInteractionWorldSystem;

	GENERATIONCLASSINFOONLYTHIS(FDecoratorBase);

	FDecoratorBase(
		);

	virtual ~FDecoratorBase();

	virtual void InitialType(
		FGameplayTag InMainDecoratorType,
		FGameplayTag InBranchDecoratorType);
	
	virtual void Entry();

	virtual void ReEntry();

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
		UGT_SwitchSceneElement_Base* TaskPtr
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
