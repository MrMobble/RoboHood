// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RGameMode.h"

//Other Classes
#include "RCharacter.h"
#include "RPlayerController.h"
#include "RSpawnPoint.h"
#include "RHUD.h"

//Other Includes
#include "Public/EngineUtils.h"

ARGameMode::ARGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/BP_Character"));
	if (PlayerPawnBPClass.Class != NULL) { DefaultPawnClass = PlayerPawnBPClass.Class; }
	

	// Set default pawn class to our Blueprinted character because I am lazy
	static ConstructorHelpers::FClassFinder<AHUD> TestHUD(TEXT("/Game/UI/BP_HUD"));
	if (TestHUD.Class != NULL) { HUDClass = TestHUD.Class; }

	PlayerControllerClass = ARPlayerController::StaticClass();

}

AActor* ARGameMode::ChooseSpawnLocation(AController* Player)
{
	if (Player)
	{
		TArray<ARSpawnPoint*> SpawnPoints;
		for (TActorIterator<ARSpawnPoint> StartItr(GetWorld()); StartItr; ++StartItr)
		{
			SpawnPoints.Add(*StartItr);
		}

		return SpawnPoints[FMath::RandRange(0, 4)];
	}
	return nullptr;
}


