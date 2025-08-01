#include "MainHUDLayout.h"

#include "Algorithm.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/KismetMathLibrary.h"

#include "SceneElementBase.h"
#include "FeatureWheel.h"
#include "SceneElement_Space.h"

void UMainHUDLayout::NativeConstruct()
{
	Super::NativeConstruct();

	if (OverlapPtr)
	{
		OverlapPtr->ClearChildren();
	}
}

void UMainHUDLayout::InitalFeaturesItem(
	ASceneElement_Space* InSceneElement_SpacePtr,
	const FString& FeatureName,
	const TArray<FFeaturesItem>& Features
	)
{
	if (!InSceneElement_SpacePtr)
	{
		return;
	}
	
	if (SceneElement_SpacePtr == InSceneElement_SpacePtr)
	{
		return;
	}
	else
	{
		SceneElement_SpacePtr = InSceneElement_SpacePtr;
	}

	if (FeatureWheelPtr)
	{
		FeatureWheelPtr->RemoveFromParent();
	}
	
	FeatureWheelPtr = CreateWidget<UFeatureWheel>(this, FeatureWheelClass);
	if (FeatureWheelPtr)
	{
		auto TargetPt = UKismetAlgorithm::GetActorBox(
		                                              {SceneElement_SpacePtr}
		                                             );

		FeatureWheelPtr->TargetPt = TargetPt.GetCenter();
		FeatureWheelPtr->InitalFeaturesItem(FeatureName, Features);

		FeatureWheelPtr->AddToViewport();

		// auto SlotPtr = OverlapPtr->AddChildToOverlay(FeatureWheelPtr);
		// if (SlotPtr)
		// {
		// 	SlotPtr->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		// 	SlotPtr->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		// }
	}
}

void UMainHUDLayout::RemoveFeatures()
{
	if (FeatureWheelPtr)
	{
		FeatureWheelPtr->RemoveFromParent();
	}
	SceneElement_SpacePtr = nullptr;
	FeatureWheelPtr = nullptr;
}
