// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RLobbyGameState.h"

//Engine Includes
#include "Public/TimerManager.h"
#include "Net/UnrealNetwork.h" 

//Class Includes
#include "RGameInstance.h"
#include "RLobbyInterface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Server Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ARLobbyGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Makes Sure The Variable Is Replicated So That Both Server And Client Can See It
	DOREPLIFETIME(ARLobbyGameState, IntCountDown);
}

void ARLobbyGameState::MultiUpdatePlayerList_Implementation()
{
	URGameInstance* GameInstance = Cast<URGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		URLobbyInterface* Lobby = Cast<URLobbyInterface>(GameInstance->GetLobbyWidget());
		if (Lobby)
		{
			Lobby->UpdateList();
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

void ARLobbyGameState::StartCountDown()
{
	IntCountDown = CountDownTime;

	//Starts The CountDown Timer
	GetWorldTimerManager().SetTimer(CountDownHandle, this, &ARLobbyGameState::CountDown, 1.0f, true, 0.0f);
}

void ARLobbyGameState::CountDown()
{
	if (IntCountDown <= 0)
	{
		//Clears The CountDown Handle So It NoLonger Ticks
		GetWorldTimerManager().ClearTimer(CountDownHandle);

		//Displays LoadingScreen For All Players + Server
		MultiDisplayLoadingScreen();

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

	//Simple
	IntCountDown--;
}