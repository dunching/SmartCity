 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "PersonMark.generated.h"



/*
 * 用于表示人物的标记点
 */
UCLASS()
class SMARTCITY_API APersonMark : public AActor
{
	GENERATED_BODY()

public:
	APersonMark(const FObjectInitializer& ObjectInitializer);

	void Update(const FVector& NewLocation);
	
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

};