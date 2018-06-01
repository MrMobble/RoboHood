// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RLobbyGameState.h"

//Engine Includes
#include "Public/TimerManager.h"
#include "Net/UnrealNetwork.h" 

//Class Includes
#include "RGameInstance.h"
#include "RLobbyInterface.h"
#include "Lobby/RLobbyPlayerController.h"

void ARLobbyGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	//GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &ARLobbyGameState::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Server Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ARLobbyGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARLobbyGameState, TimeValue);
}

//Not in use
void ARLobbyGameState::UpdatePlayerList()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ARLobbyPlayerController* PlayerController = Cast<ARLobbyPlayerController>((*It));
		if (PlayerController)
		{
			PlayerController->UpdateLobbyList();
		}
	}
}

//Not in use
void ARLobbyGameState::MultiUpdatePlayerList_Implementation()
{
	URGameInstance* GameInstance = Cast<URGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		URLobbyInterface* Lobby = Cast<URLobbyInterface>(GameInstance->GetLobbyWidget());
		if (Lobby)
		{
			Lobby->UpdatePlayerList();
		}
	}
}

void ARLobbyGameState::MultiDisplayLoadingScreen_Implementation()
{
	URGameInstance* GameInstance = Cast<URGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		URLobbyInterface* Lobby = Cast<URLobbyInterface>(GameInstance->GetLobbyWidget());
		if (Lobby)
		{
			Lobby->ShowLoadingScreen();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Start Game Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//void ARLobbyGameState::StartCountDown()
//{
//	GetWorldTimerManager().SetTimer(TimerHandle_CountDown, this, &ARLobbyGameState::StartMultiplayerGame, 6.0f);
//
//	TimeValue = CountDownTime;
//}
//
//void ARLobbyGameState::StopCountDown()
//{
//	//Clears The CountDown Handle So It NoLonger Ticks
//	GetWorldTimerManager().ClearTimer(TimerHandle_CountDown);
//
//	GetWorldTimerManager().ClearTimer(TimerHandle_DefaultTimer);
//}
//
//void ARLobbyGameState::StartMultiplayerGame()
//{
//	//Displays LoadingScreen For All Players + Server
//	MultiDisplayLoadingScreen();
//
//	URGameInstance* GameInstance = Cast<URGameInstance>(GetWorld()->GetGameInstance());
//	if (GameInstance)
//	{
//		URLobbyInterface* Lobby = Cast<URLobbyInterface>(GameInstance->GetLobbyWidget());
//		if (Lobby)
//		{
//			//Calls ServerTravel Event In The LobbyWidget
//			Lobby->ServerTravel();
//		}
//	}
//}
//
//void ARLobbyGameState::DefaultTimer()
//{
//	TimeValue--; //= GetWorldTimerManager().GetTimerRemaining(TimerHandle_CountDown);
//}
