// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GameMode_Main.h"

#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"
#include "GameplayCommand.h"
#include "GameplayTagsLibrary.h"
#include "SceneInteractionWorldSystem.h"
#include "WeatherSystem.h"
#include "DatasmithAssetUserData.h"
#include "FloorHelper.h"
#include "IPSSI.h"
#include "LogWriter.h"
#include "ViewBuildingProcessor.h"
#include "TourPawn.h"

AGameMode_Main::AGameMode_Main() :
                                 Super()
{
}

void AGameMode_Main::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);
}

void AGameMode_Main::BeginPlay()
{
	Super::BeginPlay();

	// 天气
	UWeatherSystem::GetInstance()->RegisterCallback();
	UWeatherSystem::GetInstance()->ResetTime();

	//
	TArray<AActor*> ResultAry;

	// 视角
	// SmartCityCommand::ReplyCameraTransform();

	// 
	USceneInteractionWorldSystem::GetInstance()->InitializeSceneActors();

#pragma region 场景
	// 整楼、外墙
	ProcessExternalWall();

	// 楼层
	ProcessFloor();

	// 区域
	ProcessSpace();
#pragma endregion

	FOnActorSpawned::FDelegate Delegate_OnActorSpawned;

	Delegate_OnActorSpawned.BindLambda(
	                                   [](
	                                   AActor* ActorPtr
	                                   )
	                                   {
		                                   if (ActorPtr && ActorPtr->IsA(AFloorHelper::StaticClass()))
		                                   {
			                                   PRINTINVOKEINFO();
		                                   }
	                                   }
	                                  );

	GWorld->AddOnActorSpawnedHandler(Delegate_OnActorSpawned);
}

void AGameMode_Main::EndPlay(
	const EEndPlayReason::Type EndPlayReason
	)
{
	Super::EndPlay(EndPlayReason);
}

void AGameMode_Main::BeginDestroy()
{
	Super::BeginDestroy();
}

void AGameMode_Main::ProcessExternalWall()
{
}

void AGameMode_Main::ProcessSpace()
{
}

void AGameMode_Main::ProcessFloor()
{
	TArray<AActor*> ResultAry;
	UGameplayStatics::GetAllActorsOfClass(
	                                      this,
	                                      AStaticMeshActor::StaticClass(),
	                                      ResultAry
	                                     );

	for (auto Iter : ResultAry)
	{
		auto STMPtr = Cast<AStaticMeshActor>(Iter);
		if (STMPtr)
		{
			auto STPtr = STMPtr->GetStaticMeshComponent()->GetStaticMesh();
			if (!STPtr)
			{
				continue;
			}
			auto AUDPtr = STPtr
				->
				GetAssetUserDataOfClass(
				                        UDatasmithAssetUserData::StaticClass()
				                       );
			if (AUDPtr)
			{
			}
		}
	}
}
