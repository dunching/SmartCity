
#include "EditorCommand.h"

#include "DatasmithSceneActor.h"
#include "Kismet/GameplayStatics.h"

#include "Tools.h"

void EditorCommand::ModifyMoveable()
{
	TArray<AActor*> Ary;
	UGameplayStatics::GetAllActorsOfClass(GetWorldImp(), ADatasmithSceneActor::StaticClass(), Ary);
	for (auto Iter : Ary)
	{
		auto ActorPtr = Cast<ADatasmithSceneActor>(Iter);
		if ( ActorPtr)
		{
			ActorPtr->GetRootComponent()->SetMobility(EComponentMobility::Movable);
		}
	}
}
