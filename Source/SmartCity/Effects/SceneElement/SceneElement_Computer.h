// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"

#include "GameOptions.h"
#include "HoverWidgetBase.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_Computer.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class UActorSequenceComponent;
class AComputerMarkGroup;
class UTextBlock;

UCLASS()
class SMARTCITY_API UComputerMarkGroupWidget : public UHoverWidgetReBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetText(
		const FString& InText
		);

	virtual FVector GetHoverPosition() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText = nullptr;

	FVector TargetPt = FVector::ZeroVector;

};

/**
 * 门禁
 */
UCLASS()
class SMARTCITY_API ASceneElement_Computer :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:

	ASceneElement_Computer(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;
	
	virtual void Merge(
		const TSoftObjectPtr<AActor>& ActorRef,
		const TPair<FName, FString>& InUserData
		, const TMap<FName, FString>& NewUserData
		) override;

	virtual void InitialSceneElement() override;

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

	virtual TPair<FTransform, float> GetViewSeat()const override;

	void DisplayGroupWidget();
	
protected:
	
	TPair<FName, FString> CurrentUserData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UStaticMeshComponent*>StaticMeshComponentsAry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UComputerMarkGroupWidget> ComputerMarkClass = nullptr;

	TObjectPtr<UComputerMarkGroupWidget>WidgetPtr = nullptr;
};
