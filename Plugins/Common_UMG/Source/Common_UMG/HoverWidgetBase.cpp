#include "HoverWidgetBase.h"

#include "LogWriter.h"

FVector2D UHoverWidgetBase::ModifyProjectedLocalPosition(
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
	auto Result = GEngine->GetFirstLocalPlayerController(GetWorld())->ProjectWorldLocationToScreen(
		 GetHoverPosition(),
		 ScreenLocation
		);
	if (Result)
	{
		int32 SizeX;
		int32 SizeY;

		GEngine->GetFirstLocalPlayerController(GetWorld())->GetViewportSize(SizeX, SizeY);
		SetPositionInViewport(ScreenLocation);
	}
	else
	{
		PRINTINVOKEINFO();
	}
}

FVector UHoverWidgetReBase::GetHoverPosition()
{
	return FVector::ZeroVector;
}
