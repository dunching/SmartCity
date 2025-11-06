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
	APersonMark(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	virtual void Tick(
		float DeltaTime
		) override;

	void Update(
		const FVector& NewLocation
		);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USceneComponent> AnchorComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotSpeed = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MoveSpeed = 200;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor Color1 = FColor::Green;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor Color2 = FColor::Red;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinDistance = 50;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxDistance = 150;

	TSharedPtr<TSet<APersonMark*>> Marks;

	FVector TargetLocation;
};
