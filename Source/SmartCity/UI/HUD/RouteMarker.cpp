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
		FVector Origin;
		FVector BoxExtent;

		TargetActor->GetActorBounds(false, Origin, BoxExtent);
		return Origin;
	}

	return FVector::ZeroVector;
}
