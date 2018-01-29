// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RLobbyPlayerController.h"
#include "RLobbyGameMode.h"
#include "RGameInstance.h"

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

