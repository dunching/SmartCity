#include "SceneElement_RollerBlind.h"

#include "ActorSequenceComponent.h"
#include "ActorSequencePlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Engine/StaticMeshActor.h"

#include "AssetRefMap.h"
#include "FloorHelper.h"
#include "MessageBody.h"
#include "SmartCitySuiteTags.h"
#include "WebChannelWorldSystem.h"
#include "Components/BoxComponent.h"

ASceneElement_RollerBlind::ASceneElement_RollerBlind(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void ASceneElement_RollerBlind::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneElement_RollerBlind::EndPlay(
	const EEndPlayReason::Type EndPlayReason
	)
{
	Super::EndPlay(EndPlayReason);
}

void ASceneElement_RollerBlind::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);

	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto ParentPtr = ActorPtr->GetAttachParentActor();

		AFloorHelper* FloorPtr = nullptr;
		for (; ParentPtr;)
		{
			ParentPtr = ParentPtr->GetAttachParentActor();
			FloorPtr = Cast<AFloorHelper>(ParentPtr);
			if (FloorPtr)
			{
				break;
			}
		}

		if (!FloorPtr)
		{
			return;
		}

		const auto FloorCenter = FloorPtr->BoxComponentPtr->CalcBounds(
		                                                               FloorPtr->BoxComponentPtr->
		                                                               GetComponentToWorld()
		                                                              )
		                                 .GetBox().GetCenter();
		const auto Dir = FloorCenter - GetActorLocation();

		const auto Dot = FVector::DotProduct(Dir, GetActorRightVector());

		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			FBox Box(ForceInit);

			FBox STBox(ForceInit);
			STBox.IsValid = true;
			STPtr->GetStaticMeshComponent()->GetLocalBounds(STBox.Min, STBox.Max);
			const auto TemoBox = STBox.TransformBy(STPtr->GetStaticMeshComponent()->GetRelativeTransform());
			Box += TemoBox;

			if (!UserData.Contains(TEXT("Datasmith_UniqueId")))
			{
				return;
			}

			const auto Size = Box.GetSize();
			if (Dot > 0)
			{
			}
			else
			{
			}

			if (STBox.GetSize().X > STBox.GetSize().Y)
			{
				SetActorRelativeRotation(STPtr->GetStaticMeshComponent()->GetRelativeRotation());
			}
			else
			{
				SetActorRelativeRotation(STPtr->GetStaticMeshComponent()->GetRelativeRotation() + FRotator(0,90,0));
			}
			
			if (Size.X > Size.Y)
			{
			
				SetActorRelativeScale3D(
								FVector(
										Size.X / DefaultSize.
										X,
										1,
										(Size.Z ) /
										DefaultSize.Z
									   )
							   );
				
			}
			else
			{
			
				SetActorRelativeScale3D(
								FVector(
										Size.Y / DefaultSize.
										X,
										1,
										(Size.Z ) /
										DefaultSize.Z
									   )
							   );
				
			}
			
			SetActorRelativeLocation(Box.GetCenter() - FVector(0, 0, Size.Z / 2));
		}
	}
}

void ASceneElement_RollerBlind::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall) ||
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_Periphery)
		)
		{
			QuitAllState();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_RollerBlind)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_BatchControl)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
		)
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode)
		)
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if ((ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
		     ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)))
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_Focus)
		)
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}

		//
		QuitAllState();

		return;
	}
}

void ASceneElement_RollerBlind::EntryFocusDevice()
{
	Super::EntryFocusDevice();

	TArray<UStaticMeshComponent*> OutComponents;
	GetComponents<UStaticMeshComponent>(OutComponents);

	for (auto Iter : OutComponents)
	{
		if (Iter)
		{
			Iter->SetRenderCustomDepth(true);
			Iter->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
		}
	}

	auto MessageBodySPtr = MakeShared<FMessageBody_ViewDevice>();

	MessageBodySPtr->DeviceID = SceneElementID;
	MessageBodySPtr->Type = DeviceTypeStr;

	UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);
}

void ASceneElement_RollerBlind::EntryViewDevice()
{
	Super::EntryViewDevice();
	SetActorHiddenInGame(false);
}

void ASceneElement_RollerBlind::EntryShowDevice()
{
	Super::EntryShowDevice();
	SetActorHiddenInGame(false);

	PlayAnimation(0);
}

void ASceneElement_RollerBlind::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();

	SetActorHiddenInGame(false);

	if (ExtensionParamMap.Contains(TEXT("Percent")))
	{
		const auto Value = UKismetStringLibrary::Conv_StringToInt(ExtensionParamMap[TEXT("Percent")]);
		PlayAnimation(Value / 100.0f);

		return;
	}

	if (ExtensionParamMap.Contains(TEXT("开关")))
	{
		const auto Value = UKismetStringLibrary::Conv_StringToInt(ExtensionParamMap[TEXT("开关")]);

		PlayAnimation(1 - (Value / 100.0f));

		return;
	}

	EntryShowDevice();
}

void ASceneElement_RollerBlind::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(true);

	TArray<UStaticMeshComponent*> OutComponents;
	GetComponents<UStaticMeshComponent>(OutComponents);

	for (auto Iter : OutComponents)
	{
		if (Iter)
		{
			Iter->SetRenderCustomDepth(false);
		}
	}


	PlayAnimation(0);
}

void ASceneElement_RollerBlind::PlayAnimation(
	float Percent
	)
{
	if (MySequenceComponent->GetSequencePlayer() != nullptr)
	{
		const auto Second = MySequenceComponent->GetSequencePlayer()->GetDuration().AsSeconds();

		FMovieSceneSequencePlaybackParams PlaybackParams(Percent * Second, EUpdatePositionMethod::Play);
		FMovieSceneSequencePlayToParams PlayToParams;
		MySequenceComponent->GetSequencePlayer()->PlayTo(PlaybackParams, PlayToParams);
	}
}
