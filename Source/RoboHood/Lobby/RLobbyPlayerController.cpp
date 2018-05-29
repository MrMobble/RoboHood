// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RLobbyPlayerController.h"
#include "Lobby/RLobbyPlayerState.h"
#include "RLobbyGameMode.h"
#include "RGameInstance.h"

void ARLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ARLobbyPlayerController::SpawnSelectedClass(int32 Index)
{

	if (SelectionReference) { SelectionReference->Destroy(); }

	URGameInstance* GInstance = Cast<URGameInstance>(GetWorld()->GetGameInstance());
	if (GInstance)
	{

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* NewCharacter = GetWorld()->SpawnActor<AActor>(GInstance->LobbyCharacters[Index], FVector(0, 0, 0), FRotator(0, 0, 0), ActorSpawnParams);
		SelectionReference = NewCharacter;
		
	}
}

bool ARLobbyPlayerController::CheckAuthority()
{
	ARLobbyPlayerState* PlayState = Cast<ARLobbyPlayerState>(PlayerState);
	if (PlayState)
	{
		return PlayState->isServer;
	}

	return true;
}

void ARLobbyPlayerController::UpdateLobbyList()
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

