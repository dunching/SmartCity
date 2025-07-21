// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/HUD.h"

#include "UserWidget_Override.h"
#include "TemplateHelper.h"

#include "MainHUD.generated.h"

class UMainHUDLayout;
class URegularActionLayout;
class UEndangeredStateLayout;
class UGetItemInfosList;
class UUIManagerSubSystem;
struct FOnAttributeChangeData;

UCLASS()
class SMARTCITY_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	friend UUIManagerSubSystem;

	using FOnInitaliedGroupSharedInfo =
	TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	virtual void BeginPlay() override;

	virtual void ShowHUD() override;

	void InitalHUD();

protected:

};
