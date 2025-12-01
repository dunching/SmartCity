#include "FeatureWheel.h"

#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"

#include "SceneElementBase.h"

void UFeatureItem::NativeConstruct()
{
	Super::NativeConstruct();
}

void UFeatureItem::InitalFeaturesItem(
	const FFeaturesItem& InFeatureItem
	)
{
	FeaturesItem = InFeatureItem;

	if (NameText)
	{
		NameText->SetText(FText::FromString(FeaturesItem.Text));
	}

	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
	}

	UpdateStateDescription();
}

void UFeatureItem::UpdateStateDescription()
{
	if (FeaturesItem.SceneElementPtr)
	{
		const auto StateDescription = FeaturesItem.SceneElementPtr->GetStateDescription();

		FString Text;
		for (const auto& Iter : StateDescription)
		{
			if (Iter.Key.IsEmpty())
			{
				Text.Append(Iter.Value);
			}
			else
			{
				Text.Append(Iter.Key);

				Text.Append(TEXT(":"));
				Text.Append(Iter.Value);
			}
		}
	}
}

void UFeatureItem::OnClicked()
{
	if (FeaturesItem.SceneElementPtr)
	{
		FeaturesItem.SceneElementPtr->BeginInteraction();
	}

	UpdateStateDescription();
}

void UFeatureWheel::NativeConstruct()
{
	Super::NativeConstruct();

	SetAlignmentInViewport(FVector2D(.5f, 1.f));
}

FVector UFeatureWheel::GetHoverPosition()
{
	return TargetPt;
}

void UFeatureWheel::InitalFeaturesItem(
	const FString& FeatureName,
	const TArray<FFeaturesItem>& Features
	)
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(FeatureName));
	}

	if (VerticalBoxPtr)
	{
		VerticalBoxPtr->ClearChildren();

		const auto Offset = (SizeBox->GetWidthOverride() / 2) - 100;

		const auto Num = Features.Num();
		int32 Index = 0;
		for (const auto& Iter : Features)
		{
			auto UIPtr = CreateWidget<UFeatureItem>(this, FeatureItemClass);
			if (UIPtr)
			{
				UIPtr->InitalFeaturesItem(Iter);

				// FVector Vec(0, 0, -Offset);
				// const auto Angle = Index / static_cast<float>(Num) * 360;
				// Vec = Vec.RotateAngleAxis(Angle, FVector::ForwardVector);

				Index++;

				auto SlotPtr = VerticalBoxPtr->AddChild(UIPtr);
			}
		}
	}
}

void UFeatureWheel::SetOncliced(
	const FOnButtonClickedEvent& OnClicked
	)
{
	Button->OnClicked = OnClicked;
}
