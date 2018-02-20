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
#include "RCameraManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ARPlayerController::ARPlayerController()
{
	PlayerCameraManagerClass = ARCameraManager::StaticClass();

	//ARCameraManager* CM = Cast<ARCameraManager>(PlayerCameraManagerClass);
	//CM->SetPivotOffset(FVector(50, 50, 50));
	//Seems To Be Empty
}

void ARPlayerController::OnKilled()
{
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

	SetViewTarget(this);

	ARGameMode* GMode = Cast<ARGameMode>(GetWorld()->GetAuthGameMode());
	if (GMode)
	{
		APawn* NewCharacter = GMode->SpawnPlayer(this, ReSpawnCharacter);
		Possess(NewCharacter);
	}
}

void ARPlayerController::RespawnPlayer()
{
	ARGameMode* GMode = Cast<ARGameMode>(GetWorld()->GetAuthGameMode());
	if (GMode)
	{
		APawn* NewCharacter = GMode->SpawnPlayer(this, ReSpawnCharacter);
		Possess(NewCharacter);
	}
}

