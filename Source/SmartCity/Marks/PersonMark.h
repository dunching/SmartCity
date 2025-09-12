 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "PersonMark.generated.h"

class UNiagaraComponent;
class USceneComponent;

/*
 * 用于表示人物的标记点
 */
UCLASS()
class SMARTCITY_API APersonMark : public AActor
{
	GENERATED_BODY()

public:
	APersonMark(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	
	void Update(const FVector& NewLocation);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USceneComponent> AnchorComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotSpeed = 10;

};

/*
 * 用于表示火灾的标记点
 */
UCLASS()
class SMARTCITY_API AFireMark : public AActor
{
	GENERATED_BODY()

public:
	AFireMark(const FObjectInitializer& ObjectInitializer);

	void Update(const FVector& NewLocation);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

};
