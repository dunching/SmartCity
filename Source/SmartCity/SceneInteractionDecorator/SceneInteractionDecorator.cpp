#include "SceneInteractionDecorator.h"

#include "Kismet/GameplayStatics.h"
#include "Net/WebChannelWorldSystem.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "GameOptions.h"
#include "LogWriter.h"
#include "MessageBody.h"
#include "SceneInteractionWorldSystem.h"
#include "Algorithm.h"
#include "DatasmithSceneActor.h"
#include "Dynamic_WeatherBase.h"
#include "FloorHelper.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "SceneElement_PWR_Pipe.h"
#include "TemplateHelper.h"
#include "FloorHelperBase.h"
#include "IPSSI.h"
#include "NavagationPaths.h"
#include "PersonMark.h"
#include "SceneElement_Space.h"
#include "SmartCitySuiteTags.h"
#include "ViewSingleFloorProcessor.h"
#include "WeatherSystem.h"
#include "TourPawn.h"
#include "ViewBuildingProcessor.h"
#include "ViewSingleDeviceProcessor.h"
#include "ViewSingleFloorViewEnergyProcessor.h"
#include "ViewSplitFloorProcessor.h"
#include "SceneElement_AccessControl.h"

FDecoratorBase::FDecoratorBase(
	)
{
}

FDecoratorBase::~FDecoratorBase()
{
}

void FDecoratorBase::InitialType(
	FGameplayTag InMainDecoratorType,
	FGameplayTag InBranchDecoratorType
	)
{
	MainDecoratorType = InMainDecoratorType;
	BranchDecoratorType = InBranchDecoratorType;
}

void FDecoratorBase::Entry()
{
}

void FDecoratorBase::ReEntry()
{
}

void FDecoratorBase::Quit()
{
}

bool FDecoratorBase::NeedAsync() const
{
	return false;
}

void FDecoratorBase::OnOtherDecoratorQuit(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
}

void FDecoratorBase::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
}

bool FDecoratorBase::Operation(
	EOperatorType OperatorType
	)
{
	return false;
}

FGameplayTag FDecoratorBase::GetMainDecoratorType() const
{
	return MainDecoratorType;
}

FGameplayTag FDecoratorBase::GetBranchDecoratorType() const
{
	return BranchDecoratorType;
}

void FDecoratorBase::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors,
	UGT_SwitchSceneElementState* TaskPtr
	)
{
}

void FDecoratorBase::IncreaseWaitTaskCount()
{
	WaitTaskCount++;
}

void FDecoratorBase::DecreaseWaitTaskCount()
{
	WaitTaskCount--;
}

int32 FDecoratorBase::GetWaitTaskCount() const
{
	return WaitTaskCount;
}
