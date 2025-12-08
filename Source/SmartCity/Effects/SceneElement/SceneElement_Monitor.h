// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

#include "GameOptions.h"
#include "HoverWidgetBase.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_Monitor.generated.h"

class UWidgetSwitcher;
class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class UActorSequenceComponent;
class UMonitorHoverWidget;
class UButton;

/**
 * 门禁
 */
UCLASS()
class SMARTCITY_API ASceneElement_Monitor :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:

	ASceneElement_Monitor(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;
	
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	virtual void EntryFocusDevice(
		) override;

	virtual void EntryViewDevice(
		) override;

	virtual void EntryShowDevice(
		) override;

	virtual void EntryShoweviceEffect(
		) override;

	virtual void QuitAllState(
		) override;

	UFUNCTION()
	void OnClicked();
	
	/**
	 * 网格体
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMonitorHoverWidget> MonitorHoverWidgetClass;

	UPROPERTY()
	TObjectPtr<UMonitorHoverWidget>MonitorHoverWidgetPtr = nullptr;
};

UCLASS()
class SMARTCITY_API UMonitorHoverWidget : public UHoverWidgetReBase
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

	virtual FVector GetHoverPosition() override;

	void SetIsSelected(bool bIsSelected);
	
	void SetOncliced(const FOnButtonClickedEvent&OnClicked);

	// void SetViewType(bool bIs);
	
	FVector TargetPt = FVector::ZeroVector;

	TObjectPtr<ASceneElement_Monitor>SceneElement_MonitorPtr = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* Switcher = nullptr;
private:

protected:
};
