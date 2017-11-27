// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RPlayerController.h"

//Engine Includes
#include "GameFramework/GameMode.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

//Class Includes
#include "RGameMode.h"
#include "RHUD.h"
#include "RCharacter.h"
#include "RGameState.h"
#include "RGameInstance.h"
#include "RPlayerState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ARPlayerController::ARPlayerController()
{
	//Seems To Be Empty
}

void ARPlayerController::OnKilled()
{
	UnPossess();
	GetWorldTimerManager().SetTimer(Timehandle_Respawn, this, &ARPlayerController::RespawnPlayer, 5.f);
}

void ARPlayerController::ClientPostLogin_Implementation()
{
	URGameInstance* GInstance = Cast<URGameInstance>(GetWorld()->GetGameInstance());
	if (GInstance)
	{
		//Gets The GameInstance And Uses That To Set The Character
		ServerInitSpawn(GInstance->GameCharacters[GInstance->CharacterIndex]);
	}
}

void ARPlayerController::ServerInitSpawn_Implementation(TSubclassOf<APawn> ChosenCharacter)
{
	ReSpawnCharacter = ChosenCharacter;

	ARGameMode* GMode = Cast<ARGameMode>(GetWorld()->GetAuthGameMode());
	if (GMode)
	{
		APawn* NewCharacter = GMode->SpawnPlayer(ReSpawnCharacter);
		Possess(NewCharacter);
	}

	//GetHUD()->DrawHUD();
}

void ARPlayerController::RespawnPlayer()
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APawn* NewCharacter = GetWorld()->SpawnActor<ARCharacter>(ReSpawnCharacter, FVector(0, 0, 5000), FRotator(0, 0, 0), ActorSpawnParams);
	Possess(NewCharacter);
}

