#include "PersonMark.h"

#include "NiagaraComponent.h"
#include "SceneElement_RadarMode.h"
#include "Kismet/KismetMathLibrary.h"

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

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(AnchorComponent);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1 / 24.f;
}

void APersonMark::BeginPlay()
{
	Super::BeginPlay();

	const auto Mats = StaticMeshComponent->GetMaterials();
	for (int32 Index = 0; Index < Mats.Num(); Index++)
	{
		auto Mat = UMaterialInstanceDynamic::Create(Mats[Index], this);

		StaticMeshComponent->SetMaterial(Index, Mat);
	}
}

void APersonMark::Tick(
	float DeltaTime
	)
{
	Super::Tick(DeltaTime);

	AnchorComponent->AddRelativeRotation(FRotator(0, DeltaTime * RotSpeed, 0));

	auto Pt = GetActorLocation();

	TObjectPtr<AActor> OwnerActor = GetAttachParentActor();
	if (!OwnerActor)
	{
		return;
	}
	auto SceneElement_RadarModePtr = Cast<ASceneElement_RadarMode>(OwnerActor);
	if (!SceneElement_RadarModePtr)
	{
		return;
	}
	
	const auto Transform = SceneElement_RadarModePtr->RelativeTransformComponent->GetComponentTransform();
	const auto RelativePt = Transform.InverseTransformPosition(Pt);
	if (FVector::Distance(RelativePt, TargetLocation) < 1.f)
	{
	}
	else
	{
		const auto Offset = TargetLocation - RelativePt;
		const auto Dir = Offset.GetSafeNormal();

		const auto Len = Offset.Length();

		const auto DeltaScale = DeltaTime * MoveSpeed;
		const auto MoveOffset = Dir * (DeltaScale > Len ? Len : DeltaScale);

		auto NewPt = RelativePt + MoveOffset;
		// NewPt.Z = 5;
		SetActorRelativeLocation(NewPt, false);
	}

	auto Dist = -1;

	if (!Marks)
	{
		return;
	}

	Pt = GetActorLocation();

	for (auto Iter : *Marks)
	{
		if (Iter == this)
		{
			continue;
		}

		const auto NewDist = FVector::Dist2D(Pt, Iter->GetActorLocation());
		if (NewDist < Dist || (Dist < 0))
		{
			Dist = NewDist;
		}
	}

	if (Dist < 0)
	{
		Dist = MaxDistance;
	}
	
	const auto Dis = FMath::Clamp(
																			Dist - MinDistance,
																			0,
																			MaxDistance - MinDistance
																		   );
	const auto Offset = MaxDistance - MinDistance;

	FLinearColor Color;
	if (Dis < (Offset * .3f))
	{
		Color = Color2;
	}
	else if (Dis < Offset * .6f)
	{
		Color = UKismetMathLibrary::LinearColorLerp(
														   Color2,
														   Color1,
															.5f
														  );
	}
	else
	{
		Color = Color1;
	}
	
	const auto Mats = StaticMeshComponent->GetMaterials();
	for (int32 Index = 0; Index < Mats.Num(); Index++)
	{
		auto Mat = Cast<UMaterialInstanceDynamic>(StaticMeshComponent->GetMaterial(Index));
		if (Mat)
		{
			Mat->SetVectorParameterValue(TEXT("Color"), Color);
		}
	}
}

void APersonMark::SetStartPt(
	const FVector& NewLocation
	)
{
	Update(NewLocation);
	
	SetActorRelativeLocation(TargetLocation, false);
}

void APersonMark::Update(
	const FVector& NewLocation
	)
{
	TargetLocation = NewLocation;

	TObjectPtr<AActor> OwnerActor = GetAttachParentActor();
	if (OwnerActor)
	{
		const auto Transform = OwnerActor->GetActorTransform();
		// DrawDebugSphere(GetWorld(), Transform.TransformPosition(TargetLocation), 10.0f, 12, FColor::Red, false, 1);
	}
}
