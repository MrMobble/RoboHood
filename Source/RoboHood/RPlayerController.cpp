// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RPlayerController.h"

//GameFrameWork
#include "GameFramework/GameMode.h"
#include "GameFramework/Actor.h"

//Other Classes
#include "RGameMode.h"
#include "RHUD.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Called When Player Is Killed
void ARPlayerController::OnKilled()
{
	UnPossess();

	ClientHUDMessage();

	GetWorldTimerManager().SetTimer(Timehandle_Respawn, this, &ARPlayerController::OnRespawn, 5.f);
}

//Respawns The Player
void ARPlayerController::OnRespawn()
{
	ARGameMode* const MyGameMode = GetWorld()->GetAuthGameMode<ARGameMode>();
	if (MyGameMode)
	{
		APawn* NewPawn = MyGameMode->SpawnDefaultPawnFor(this, MyGameMode->ChooseSpawnLocation(this));
		Possess(NewPawn);
	}
}

//Displays HUD Message
void ARPlayerController::ClientHUDMessage_Implementation()
{
	ARHUD* HUD = Cast<ARHUD>(GetHUD());
	if (HUD)
	{
		HUD->RespawnTextAdd();
	}
}

