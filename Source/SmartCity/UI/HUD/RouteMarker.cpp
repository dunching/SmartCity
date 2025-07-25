#include "RouteMarker.h"

#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void URouteMarker::NativeConstruct()
{
	Super::NativeConstruct();

	if (TextSizeBox)
	{
		TextSizeBox->SetHeightOverride(TextSize);
	}

	if (Text)
	{
		Text->SetText(FText::FromString(TextStr));
	}

	if (HeightSizeBox)
	{
		HeightSizeBox->SetHeightOverride(Height);
	}
}

FVector URouteMarker::GetHoverPosition()
{
	if (TargetActor.IsValid())
	{
		return TargetActor->GetActorLocation();
	}

	return FVector::ZeroVector;
}
