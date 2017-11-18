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

}


