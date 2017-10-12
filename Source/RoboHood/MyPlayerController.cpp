// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "MyPlayerController.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/Actor.h"
#include "RoboHoodGameMode.h"
#include "TestHUD.h"

void AMyPlayerController::OnKilled()
{
	UnPossess();

	ClientHUDMessage();

	GetWorldTimerManager().SetTimer(Timehandle_Respawn, this, &AMyPlayerController::OnRespawn, 5.f);

}

void AMyPlayerController::OnRespawn()
{
	ARoboHoodGameMode* const MyGameMode = GetWorld()->GetAuthGameMode<ARoboHoodGameMode>();
	if (MyGameMode)
	{
		APawn* NewPawn = MyGameMode->SpawnDefaultPawnFor(this, MyGameMode->ChooseSpawnLocation(this));
		Possess(NewPawn);
	}
}

void AMyPlayerController::ClientHUDMessage_Implementation()
{
	ATestHUD* HUD = Cast<ATestHUD>(GetHUD());
	if (HUD)
	{
		HUD->RespawnTextAdd();
	}
}
