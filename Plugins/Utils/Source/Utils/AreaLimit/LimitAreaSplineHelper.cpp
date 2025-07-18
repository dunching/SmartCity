
#include "LimitAreaSplineHelper.h"

#include <Components/SplineComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/SplineMeshComponent.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Kismet/GameplayStatics.h>

ALimitAreaSplineHelper::ALimitAreaSplineHelper(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SplineComponentPtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));

#if WITH_EDITOR
	SplineComponentPtr->bDrawDebug = true;
#else
	SplineComponentPtr->bDrawDebug = false;
#endif

	RootComponent = SplineComponentPtr;

	PrimaryActorTick.bCanEverTick = true;
}

void ALimitAreaSplineHelper::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!SplineComponentPtr)
	{
		return;
	}
	for (auto Iter : SplineMeshAry)
	{
		if (Iter)
		{
			Iter->DestroyComponent();
		}
	}
	SplineMeshAry.Empty();
	MatInstAry.Empty();

	auto MeshPtr = SplineMesh.LoadSynchronous();

	const auto Num = SplineComponentPtr->GetNumberOfSplinePoints();
	for (int32 Index = 0; Index < Num - 1; Index++)
	{
		const auto StartTransform = SplineComponentPtr->GetTransformAtSplinePoint(Index, ESplineCoordinateSpace::World);
		const auto EndTransform = SplineComponentPtr->GetTransformAtSplinePoint(Index + 1, ESplineCoordinateSpace::World);

		const auto StartTan = SplineComponentPtr->GetTangentAtSplinePoint(Index, ESplineCoordinateSpace::World);
		const auto EndTan = SplineComponentPtr->GetTangentAtSplinePoint(Index + 1, ESplineCoordinateSpace::World);

		const auto Length =
			(SplineComponentPtr->GetDistanceAlongSplineAtSplinePoint(Index + 1) -
				SplineComponentPtr->GetDistanceAlongSplineAtSplinePoint(Index))
			/ Interaval;

		auto SplineMeshPtr = NewObject<USplineMeshComponent>(this);
		SplineMeshPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SplineMeshPtr->RegisterComponent();

		auto MatPtr = UMaterialInstanceDynamic::Create(limitMat.LoadSynchronous(), this);
		MatInstAry.Add(MatPtr);
		MatPtr->SetVectorParameterValue(TEXT("UV"), FVector(Length, 500.f * 100.f / Interaval, 0.f));

		SplineMeshPtr->SetStaticMesh(MeshPtr);
		SplineMeshPtr->SetMaterial(0, MatPtr);

		SplineMeshPtr->ForwardAxis = ESplineMeshAxis::Z;

// 		const auto Distance = FVector::Distance(StartTransform.GetLocation(), EndTransform.GetLocation());
// 		const auto Tan1 = StartTransform.GetRotation().GetForwardVector() * StartTransform.GetScale3D() * Distance;
// 		const auto Tan2 = EndTransform.GetRotation().GetForwardVector() * EndTransform.GetScale3D() * Distance;

		SplineMeshPtr->SetStartPosition(StartTransform.GetLocation());
		SplineMeshPtr->SetEndPosition(EndTransform.GetLocation());
 		SplineMeshPtr->SetStartScale(FVector2D(0.1f, 100.f));
 		SplineMeshPtr->SetEndScale(FVector2D(0.1f, 100.f));

		SplineMeshPtr->SetCollisionObjectType(ECC_WorldDynamic);
		SplineMeshPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SplineMeshPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SplineMeshPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

		SplineMeshAry.Add(SplineMeshPtr);
	}

	if (Num > 1)
	{
		const auto StartTransform = SplineComponentPtr->GetTransformAtSplinePoint(Num - 1, ESplineCoordinateSpace::World);
		const auto EndTransform = SplineComponentPtr->GetTransformAtSplinePoint(0, ESplineCoordinateSpace::World);

		const auto StartTan = SplineComponentPtr->GetTangentAtSplinePoint(Num - 1, ESplineCoordinateSpace::World);
		const auto EndTan = SplineComponentPtr->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);

		const auto Length =
			FVector::Distance(StartTransform.GetLocation(), EndTransform.GetLocation())
			/ Interaval;

		auto SplineMeshPtr = NewObject<USplineMeshComponent>(this);
		SplineMeshPtr->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SplineMeshPtr->RegisterComponent();

		auto MatPtr = UMaterialInstanceDynamic::Create(limitMat.LoadSynchronous(), this);
		MatInstAry.Add(MatPtr);
		MatPtr->SetVectorParameterValue(TEXT("UV"), FVector(Length, 500.f * 100.f / Interaval, 0.f));

		SplineMeshPtr->SetStaticMesh(MeshPtr);
		SplineMeshPtr->SetMaterial(0, MatPtr);

		SplineMeshPtr->ForwardAxis = ESplineMeshAxis::Z;

// 		const auto Distance = FVector::Distance(StartTransform.GetLocation(), EndTransform.GetLocation());
// 		const auto Tan1 = StartTransform.GetRotation().GetForwardVector() * StartTransform.GetScale3D() * Distance;
// 		const auto Tan2 = EndTransform.GetRotation().GetForwardVector() * EndTransform.GetScale3D() * Distance;

		SplineMeshPtr->SetStartPosition(StartTransform.GetLocation());
		SplineMeshPtr->SetEndPosition(EndTransform.GetLocation());
 		SplineMeshPtr->SetStartScale(FVector2D(0.1f, 100.f));
 		SplineMeshPtr->SetEndScale(FVector2D(0.1f, 100.f));

		SplineMeshPtr->SetCollisionObjectType(ECC_WorldDynamic);
		SplineMeshPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SplineMeshPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SplineMeshPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

		SplineMeshAry.Add(SplineMeshPtr);
	}
}

void ALimitAreaSplineHelper::BeginPlay()
{
	Super::BeginPlay();

	PawnPtr = Cast<APawn>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ALimitAreaSplineHelper::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentDeltaTime+=DeltaSeconds;
	if (CurrentDeltaTime > DeltaTime)
	{
		CurrentDeltaTime = 0.f;

		for (auto Iter : MatInstAry)
		{
		}
	}
}
