// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RGameMode.h"

//Other Classes
#include "RCharacter.h"
#include "RPlayerController.h"
#include "RSpawnPoint.h"
#include "RHUD.h"
#include "RPlayerState.h"
#include "RGameState.h"

//Other Includes
#include "Public/EngineUtils.h"
#include "GameFramework/Actor.h"

ARGameMode::ARGameMode()
{
	//Player Controller
	PlayerControllerClass = ARPlayerController::StaticClass();

	//Player State
	PlayerStateClass = ARPlayerState::StaticClass();

	//Game State
	GameStateClass = ARGameState::StaticClass();

	//HUD
	HUDClass = ARHUD::StaticClass();
}

//Find Random SpawnPoint FUnction
AActor* ARGameMode::ChooseSpawnLocation(AController* PlayerController)
{
	if (PlayerController)
	{
		TArray<ARSpawnPoint*> SpawnPoints;
		for (TActorIterator<ARSpawnPoint> Itr(GetWorld()); Itr; ++Itr)
		{
			SpawnPoints.Add(*Itr);
		}

		if (SpawnPoints.Num() != 0)
			return SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];	
	}

	return nullptr;
}

APawn* ARGameMode::SpawnPlayer(AController* PlayerController, TSubclassOf<APawn> ChosenCharacter)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnLocation = ChooseSpawnLocation(PlayerController);
	if (SpawnLocation)
	{
		APawn* NewCharacter = GetWorld()->SpawnActor<APawn>(ChosenCharacter, SpawnLocation->GetActorLocation(), SpawnLocation->GetActorRotation(), ActorSpawnParams);
		return NewCharacter;
	}

	return nullptr;
}

void ARGameMode::PostLogin(APlayerController* NewPlayer)
{
	ARPlayerController* PController = Cast<ARPlayerController>(NewPlayer);
	if (PController)
	{
		PController->ClientPostLogin();
	}

	ARPlayerState* PState = Cast<ARPlayerState>(PController->PlayerState);
	if (PState)
	{
		PState->ClientPostLogin();
	}
}

void ARGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn)
{
	ARPlayerState* KillerPlayerState = Killer ? Cast<ARPlayerState>(Killer->PlayerState) : NULL;
	ARPlayerController* VictimPlayerController = KilledPlayer ? Cast<ARPlayerController>(KilledPlayer) : NULL;

	if (KillerPlayerState && Killer != KilledPlayer)
	{
		KillerPlayerState->AddScore();
	}

	if (VictimPlayerController)
	{
		VictimPlayerController->OnKilled();
	}
}

