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

void ARLobbyGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &ARLobbyGameMode::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

AActor* ARLobbyGameMode::SpawnPlayer(TSubclassOf<AActor> ChosenCharacter)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* NewCharacter = GetWorld()->SpawnActor<AActor>(ChosenCharacter, FVector(0, 0, 0), FRotator(0, 0, 0), ActorSpawnParams);

	return NewCharacter;
}

////////----------------------------


void ARLobbyGameMode::StartCountDown()
{
	GetWorldTimerManager().SetTimer(TimerHandle_CountDown, this, &ARLobbyGameMode::StartMultiplayerGame, CountDownTime);

	ARLobbyGameState* const MyGameState = Cast<ARLobbyGameState>(GameState);
	if (MyGameState)
	{
		MyGameState->TimeValue = CountDownTime;
	}
}

void ARLobbyGameMode::StopCountDown()
{
	//Clears The CountDown Handle So It NoLonger Ticks
	GetWorldTimerManager().ClearTimer(TimerHandle_CountDown);

	//GetWorldTimerManager().ClearTimer(TimerHandle_DefaultTimer);
}

void ARLobbyGameMode::StartMultiplayerGame()
{
	//Displays LoadingScreen For All Players + Server
	ARLobbyGameState* const MyGameState = Cast<ARLobbyGameState>(GameState);
	if (MyGameState)
	{
		MyGameState->MultiDisplayLoadingScreen();
	}

	URGameInstance* GameInstance = Cast<URGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		URLobbyInterface* Lobby = Cast<URLobbyInterface>(GameInstance->GetLobbyWidget());
		if (Lobby)
		{
			//Calls ServerTravel Event In The LobbyWidget
			Lobby->ServerTravel();
		}
	}
}

void ARLobbyGameMode::DefaultTimer()
{
	ARLobbyGameState* const MyGameState = Cast<ARLobbyGameState>(GameState);
	if (MyGameState)
	{
		if (MyGameState->TimeValue > 0) MyGameState->TimeValue--;
	}
}