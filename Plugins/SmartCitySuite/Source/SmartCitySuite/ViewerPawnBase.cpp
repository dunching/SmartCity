#include "ViewerPawnBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AViewerPawnBase::AViewerPawnBase(const FObjectInitializer& ObjectInitializer):
	Super(
		ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}