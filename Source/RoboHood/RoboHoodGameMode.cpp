// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RoboHood.h"
#include "RoboHoodGameMode.h"
#include "TestCharacter.h"
#include "TestHUD.h"
#include "MyPlayerController.h"
#include "PlayerSpawnPoint.h"

#include "Public/EngineUtils.h"
ARoboHoodGameMode::ARoboHoodGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/MyTestCharacter"));
	if (PlayerPawnBPClass.Class != NULL) 
	{ 
		DefaultPawnClass = PlayerPawnBPClass.Class; 
		UE_LOG(LogTemp, Warning, TEXT("DefaultPawnLoaded")); 
	}

	// Set default pawn class to our Blueprinted character because I am lazy
	static ConstructorHelpers::FClassFinder<AHUD> TestHUD(TEXT("/Game/MyTestHUD"));
	if (TestHUD.Class!= NULL) 
	{
		HUDClass = TestHUD.Class; 
		UE_LOG(LogTemp, Warning, TEXT("HUDLoaded")); 
	}

	PlayerControllerClass = AMyPlayerController::StaticClass();

}

AActor * ARoboHoodGameMode::ChooseSpawnLocation(AController * Player)
{
	if (Player)
	{
		TArray<APlayerSpawnPoint*> SpawnPoints;
		for (TActorIterator<APlayerSpawnPoint> StartItr(GetWorld()); StartItr; ++StartItr)
		{
			SpawnPoints.Add(*StartItr);
		}

		return SpawnPoints[FMath::RandRange(0, 4)];
	}
	return nullptr;
}
