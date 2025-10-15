#include "Algorithm.h"

#include "Tools.h"

TPair<FTransform, float> UKismetAlgorithm::GetCameraSeat(
	const TSet<AActor*>& Actors,
	const FRotator& Rot,
	float FOV
	)
{
	TPair<FTransform, float> Result;

	FBox Box;

	// FBox BigBox;
	// AActor* BigActor = nullptr;
	//
	// FBox zBigBox;
	// AActor* zBigActor = nullptr;
	
	for (auto Iter : Actors)
	{
		if (Iter)
		{
			const auto NewBox = Iter->GetComponentsBoundingBox();
			// DrawDebugBox(
			// 			 GetWorldImp(),
			// 			 NewBox.GetCenter(),
			// 			 NewBox.GetExtent(),
			// 			 FColor::Red,
			// 			 false,
			// 			 20
			// 			);

			Box += NewBox;

			// if (BigActor)
			// {
			// 	if (NewBox .GetSize().Size() > BigBox.GetSize().Size())
			// 	{
			// 		BigActor = Iter;
			// 		BigBox = NewBox;
			// 	}
			// }
			// else
			// {
			// 	BigActor = Iter;
			// 	BigBox = NewBox;
			// }
			//
			// if (zBigActor)
			// {
			// 	if (NewBox .GetCenter().Z > zBigBox.GetCenter().Z)
			// 	{
			// 		zBigActor = Iter;
			// 		zBigBox = NewBox;
			// 	}
			// }
			// else
			// {
			// 	zBigActor = Iter;
			// 	zBigBox = NewBox;
			// }
		}
	}

	// DrawDebugBox(
	//              GetWorldImp(),
	//              Box.GetCenter(),
	//              Box.GetExtent(),
	//              FColor::Green,
	//              false,
	//              10
	//             );

	Result.Key.SetLocation(Box.GetCenter());
	Result.Key.SetRotation(Rot.Quaternion());

	const auto Radians = FMath::DegreesToRadians(45.0f);
	const auto Value = FMath::Cos(Radians);
	Result.Value = Box.GetSize().Length() / 2 / Value;

	return Result;
}

FBox UKismetAlgorithm::GetActorBox(
	const TSet<AActor*>& Actors
	)
{
	FBox Box;
	for (auto Iter : Actors)
	{
		if (Iter)
		{
			const auto NewBox = Iter->GetComponentsBoundingBox();
			// DrawDebugBox(
			// 			 GetWorldImp(),
			// 			 NewBox.GetCenter(),
			// 			 NewBox.GetExtent(),
			// 			 FColor::Green,
			// 			 false,
			// 			 10
			// 			);

			Box += NewBox;
		}
	}

	// DrawDebugBox(
	//              GetWorldImp(),
	//              Box.GetCenter(),
	//              Box.GetExtent(),
	//              FColor::Green,
	//              false,
	//              10
	//             );

	return Box;
}
