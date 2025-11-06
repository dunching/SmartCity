#include "FireMark.h"

#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"

AFireMark::AFireMark(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RootComponent = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	NiagaraComponent = CreateOptionalDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
}

void AFireMark::Update(
	const FVector& NewLocation
	)
{
}
