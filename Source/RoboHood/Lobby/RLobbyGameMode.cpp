// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RLobbyGameMode.h"

//Class Includes
#include "RLobbyGameState.h"
#include "RLobbyPlayerState.h"
#include "RLobbyPlayerController.h"
#include "RGameInstance.h"
#include "RLobbyInterface.h"

ARLobbyGameMode::ARLobbyGameMode()
{
	//Player Controller
	PlayerControllerClass = ARLobbyPlayerController::StaticClass();

	//Player State
	PlayerStateClass = ARLobbyPlayerState::StaticClass();

	//Game State
	GameStateClass = ARLobbyGameState::StaticClass();

	DefaultPawnClass = nullptr;

}

AActor* ARLobbyGameMode::SpawnPlayer(TSubclassOf<AActor> ChosenCharacter)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* NewCharacter = GetWorld()->SpawnActor<AActor>(ChosenCharacter, FVector(0, 0, 0), FRotator(0, 0, 0), ActorSpawnParams);

	return NewCharacter;
}