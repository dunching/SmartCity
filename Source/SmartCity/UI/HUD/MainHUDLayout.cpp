#include "MainHUDLayout.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"

inline void UMainHUDLayout::InitalFeaturesItem(
	const FString& FeatureName,
	const TArray<FString>& Features
	)
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(FeatureName));
	}

	if (FeatureCanvas)
	{
		FeatureCanvas->ClearChildren();

		const auto Offset = (SizeBox->GetWidthOverride() / 2)- 100;

		const auto Num = Features.Num();
		int32 Index = 0;
		for (const auto& Iter : Features)
		{
			auto UIPtr = CreateWidget<UFeatureItem>(this, FeatureItemClass);
			if (UIPtr)
			{
				FVector Vec(0, 0, -Offset);
				const auto Angle = Index / static_cast<float>(Num) * 360;
				Vec = Vec.RotateAngleAxis(Angle, FVector::ForwardVector);

				Index++;

				auto SlotPtr = FeatureCanvas->AddChildToCanvas(UIPtr);
				SlotPtr->SetAutoSize(true);
				SlotPtr->SetAnchors(.5f);
				SlotPtr->SetAlignment(FVector2D(.5f));
				SlotPtr->SetPosition(FVector2D(Vec.Y, Vec.Z));
			}
		}
	}
}
