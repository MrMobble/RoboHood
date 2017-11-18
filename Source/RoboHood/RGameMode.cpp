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

////Find Random SpawnPoint FUnction
//AActor* ARGameMode::ChooseSpawnLocation(AController* PlayerController)
//{
//
//		TArray<ARSpawnPoint*> SpawnPoints;
//		for (TActorIterator<ARSpawnPoint> Itr(GetWorld()); Itr; ++Itr)
//		{
//			SpawnPoints.Add(*Itr);
//		}
//
//		TArray<ARCharacter*> Players;
//		for (TActorIterator<ARCharacter> Itr(GetWorld()); Itr; ++Itr)
//		{
//			if ((*Itr) != PlayerController->GetOwner())
//			{
//				Players.Add(*Itr);
//			}
//		}
//
//		return SpawnPoints[0];
//}

APawn* ARGameMode::SpawnPlayer(TSubclassOf<APawn> ChosenCharacter)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APawn* NewCharacter = GetWorld()->SpawnActor<APawn>(ChosenCharacter, FVector(0, 0, 2000), FRotator(0, 0, 0), ActorSpawnParams);
	
	return NewCharacter;
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

