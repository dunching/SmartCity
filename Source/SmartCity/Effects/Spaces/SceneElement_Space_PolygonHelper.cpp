#include "SceneElement_Space_PolygonHelper.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

ASceneElement_Space_PolygonHelper::ASceneElement_Space_PolygonHelper(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SplineComponentPtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponentPtr->SetupAttachment(RootComponent);

	SplineComponentPtr->SetClosedLoop(true);
}

void ASceneElement_Space_PolygonHelper::BeginPlay()
{
	Super::BeginPlay();

	SplineMeshComponentsAry.Empty();

	auto Lambda = [this](
		USplineComponent* SplineComponentPtr,
		int32 Start,
		int32 End
		)
	{
		const auto Transform1 = SplineComponentPtr->GetTransformAtSplinePoint(
			 Start,
			 ESplineCoordinateSpace::World,
			 true
			);
		const auto Tangent1 = SplineComponentPtr->GetTangentAtSplinePoint(Start, ESplineCoordinateSpace::World);

		const auto Transform2 = SplineComponentPtr->GetTransformAtSplinePoint(
			 End,
			 ESplineCoordinateSpace::World,
			 true
			);
		const auto Tangent2 = SplineComponentPtr->GetTangentAtSplinePoint(End, ESplineCoordinateSpace::World);

		auto SplineMeshPtr = Cast<USplineMeshComponent>(
		                                                AddComponentByClass(
		                                                                    USplineMeshComponent::StaticClass(),
		                                                                    true,
		                                                                    FTransform::Identity,
		                                                                    false
		                                                                   )
		                                               );

		SplineMeshComponentsAry.Add(SplineMeshPtr);

		SplineMeshPtr->SetStaticMesh(StaticMeshRef.LoadSynchronous());
		SplineMeshPtr->SetForwardAxis(ESplineMeshAxis::Z);
		SplineMeshPtr->SetMaterial(0, MaterialRef.LoadSynchronous());

		SplineMeshPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		const auto Dir = Transform1.GetLocation() - 	Transform1.GetLocation();
		
		SplineMeshPtr->SetStartPosition(Transform1.GetLocation());
		SplineMeshPtr->SetStartTangent(Dir);
		SplineMeshPtr->SetStartScale(Scale);

		SplineMeshPtr->SetEndPosition(Transform2.GetLocation());
		SplineMeshPtr->SetEndTangent(-Dir);
		SplineMeshPtr->SetEndScale(Scale);
	};

	const auto Num = SplineComponentPtr->GetNumberOfSplinePoints();
	if (Num > 1)
	{
		for (int32 Index = 0; Index < Num - 1; Index++)
		{

			Lambda(SplineComponentPtr, Index, Index + 1);
		}

		Lambda(SplineComponentPtr, Num - 1, 0);
	}
}
