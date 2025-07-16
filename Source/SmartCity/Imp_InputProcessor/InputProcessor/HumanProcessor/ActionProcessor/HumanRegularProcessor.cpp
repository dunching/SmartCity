#include "HumanRegularProcessor.h"

#include "DrawDebugHelpers.h"
#include "Async/Async.h"
#include "Engine/LocalPlayer.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "HumanCharacter_Player.h"
#include "InputProcessorSubSystemBase.h"

#include "GameplayTagsLibrary.h"
#include "HumanCharacter_AI.h"
#include "InputProcessorSubSystem_Imp.h"

static TAutoConsoleVariable<int32> HumanRegularProcessor(
                                                         TEXT("Skill.DrawDebug.HumanRegularProcessor"),
                                                         0,
                                                         TEXT("")
                                                         TEXT(" default: 0")
                                                        );

namespace HumanProcessor
{
	FHumanRegularProcessor::FHumanRegularProcessor(
		FOwnerPawnType* CharacterPtr
		) :
		  Super(CharacterPtr)
	{
	}

	FHumanRegularProcessor::~FHumanRegularProcessor()
	{
	}

	void FHumanRegularProcessor::EnterAction()
	{
		Super::EnterAction();

		Async(
		      EAsyncExecution::ThreadPool,
		      std::bind(&ThisClass::UpdateLookAtObject, this)
		     );

		SwitchCurrentWeapon();

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			SwitchShowCursor(false);

			AddOrRemoveUseMenuItemEvent(true);
		}
	}

	void FHumanRegularProcessor::TickImp(
		float Delta
		)
	{
		Super::TickImp(Delta);

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (!OnwerActorPtr)
		{
			return;
		}

	}

	void FHumanRegularProcessor::SwitchWeapon()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr)
		{
		}
	}

	void FHumanRegularProcessor::SwitchCurrentWeapon()
	{
	}

	void FHumanRegularProcessor::QuitAction()
	{
		AddOrRemoveUseMenuItemEvent(false);

		// UUIManagerSubSystem::GetInstance()->DisplayActionLayout(false);

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
		}

		Super::QuitAction();
	}

	bool FHumanRegularProcessor::InputKey(
		const FInputKeyEventArgs& EventArgs
		)
	{
		switch (EventArgs.Event)
		{
		case IE_Pressed:
			{
				auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
				if (!OnwerActorPtr)
				{
					return false;
				}
			}
			break;
		case IE_Released:
			{
			}
			break;
		}

		return Super::InputKey(EventArgs);
	}

	bool FHumanRegularProcessor::InputAxis(
		const FInputKeyEventArgs& EventArgs
		)
	{
		return Super::InputAxis(EventArgs);
	}

	void FHumanRegularProcessor::AddOrRemoveUseMenuItemEvent(
		bool bIsAdd
		)
	{
	}

	void FHumanRegularProcessor::UpdateLookAtObject()
	{
		IncreaseAsyncTaskNum();
		ON_SCOPE_EXIT
		{
			ReduceAsyncTaskNum();
		};
	}
}
