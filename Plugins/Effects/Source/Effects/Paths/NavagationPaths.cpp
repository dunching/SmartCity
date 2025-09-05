#include "NavagationPaths.h"

#include "Components/SplineComponent.h"

ANavagationPaths::ANavagationPaths(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SplineComponentPtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponentPtr->SetupAttachment(RootComponent);
}

void ANavagationPaths::SwitchDisplay(
	bool bIsDisplay
	)
{
	if (bIsDisplay)
	{
		SetActorHiddenInGame(false);

		if (!StaticMeshComponentsAry.IsEmpty())
		{
			return;
		}
		
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::UpdateMarks, Frequence, true);

		TArray<USplineComponent*>ComponentsAry;
		GetComponents(ComponentsAry);
		
		for (auto Iter :ComponentsAry)
		{
			const auto Len = Iter->GetSplineLength();
			
			FMarksHelper MarksHelper;
			
			for (int32 CurLen = Interval; CurLen < Len; CurLen += Interval)
			{
				auto NewSceneComponentPtr = Cast<USceneComponent>(
																  AddComponentByClass(
																	   UStaticMeshComponent::StaticClass(),
																	   true,
																	   FTransform::Identity,
																	   false
																	  )
																 );
				NewSceneComponentPtr->AttachToComponent(Iter, FAttachmentTransformRules::KeepRelativeTransform);
				
				auto NewMeshComponentPtr = Cast<UStaticMeshComponent>(
																  AddComponentByClass(
																	   UStaticMeshComponent::StaticClass(),
																	   true,
																	   RelativeTransform,
																	   false
																	  )
																 );
				NewMeshComponentPtr->AttachToComponent(NewSceneComponentPtr, FAttachmentTransformRules::KeepRelativeTransform);
				NewMeshComponentPtr->SetStaticMesh(StaticMesh.LoadSynchronous());

				NewMeshComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				NewMeshComponentPtr->SetRenderCustomDepth(false);

				MarksHelper.LenMap.Add(NewSceneComponentPtr, CurLen);
			}

			StaticMeshComponentsAry.Add(Iter, MarksHelper);
		}
	}
	else
	{
		SetActorHiddenInGame(true);

		GetWorldTimerManager().ClearTimer(TimerHandle);

		for (auto Iter : StaticMeshComponentsAry)
		{
			for (auto SecondIter : Iter.Value.LenMap)
			{
				TArray<USceneComponent*> ChildrenAry;
				SecondIter.Key->GetChildrenComponents(true, ChildrenAry);
				for (auto ThirdIter : ChildrenAry)
				{
					ThirdIter->DestroyComponent();	
				}
				SecondIter.Key->DestroyComponent();
			}
		}
		StaticMeshComponentsAry.Empty();
	}
}

void ANavagationPaths::UpdateMarks()
{
	for (auto &Iter : StaticMeshComponentsAry)
	{
		const auto Len = Iter.Key->GetSplineLength();

		for (auto &SecondIter : Iter.Value.LenMap)
		{
			auto NewPtInLen = SecondIter.Value + (Speed * Frequence);
			if (NewPtInLen > Len)
			{
				NewPtInLen = NewPtInLen - Len;
			}

			SecondIter.Value = NewPtInLen;

			const auto Tranform = Iter.Key->GetTransformAtDistanceAlongSpline(
				 NewPtInLen,
				 ESplineCoordinateSpace::Local
				);
			SecondIter.Key->SetRelativeTransform(Tranform);
		}
	}
}
