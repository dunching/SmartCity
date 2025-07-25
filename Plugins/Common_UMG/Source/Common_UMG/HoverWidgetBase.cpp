
#include "HoverWidgetBase.h"

inline FVector2D UHoverWidgetBase::ModifyProjectedLocalPosition(
	const FGeometry& ViewportGeometry,
	const FVector2D& LocalPosition
)
{
	return LocalPosition;
}

FVector UHoverWidgetBase::GetHoverPosition()
{
	return FVector::ZeroVector;
}

void UHoverWidgetReBase::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::UpdatePosition, UpdateRate, true);

	SetAlignmentInViewport(FVector2D(.5f, 1.f));
	UpdatePosition();
}

void UHoverWidgetReBase::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	
	Super::NativeDestruct();
}

void UHoverWidgetReBase::UpdatePosition()
{
	FVector2D ScreenLocation;
	GEngine->GetFirstLocalPlayerController(GetWorld())->ProjectWorldLocationToScreen(GetHoverPosition(),ScreenLocation);
	SetPositionInViewport(ScreenLocation);
}

FVector UHoverWidgetReBase::GetHoverPosition()
{
	return FVector::ZeroVector;
}
