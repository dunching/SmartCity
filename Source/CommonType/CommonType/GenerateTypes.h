// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "GenerateTypes.generated.h"

UENUM()
enum class EDecoratorType : uint8
{
	kMode,
	kMode_Tour,
	kMode_Scene,
	kMode_Radar,

	kArea,
	kArea_ExternalWall,
	kArea_Floor,
	kArea_Space,

	kNone,
};

UENUM()
enum class EOperatorType: uint8
{
	kLeftMouseButton,

	kRightMouseButton,

	kNone,
};
