
#include "TowerHelperBase.h"

#include "Components/BoxComponent.h"

ATowerHelperBase::ATowerHelperBase(
	const FObjectInitializer& ObjectInitializer
	):Super(ObjectInitializer)
{
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	BoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponentPtr->SetupAttachment(RootComponent);

	BoxComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
