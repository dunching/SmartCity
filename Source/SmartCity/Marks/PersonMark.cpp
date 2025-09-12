#include "PersonMark.h"

#include "NiagaraComponent.h"

APersonMark::APersonMark(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RootComponent = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	AnchorComponent = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("Mesh"));
	if (AnchorComponent)
	{
		AnchorComponent->SetupAttachment(RootComponent);
	}

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1 / 24.f;
}

void APersonMark::Tick(
	float DeltaTime
	)
{
	Super::Tick(DeltaTime);

	AnchorComponent->AddRelativeRotation(FRotator(0,DeltaTime * RotSpeed,0));
}

void APersonMark::Update(
	const FVector& NewLocation
	)
{
	SetActorRelativeLocation(NewLocation);
}

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
