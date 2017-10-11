// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "MyPlayerController.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/Actor.h"
#include "RoboHoodGameMode.h"

void AMyPlayerController::OnKilled()
{
	UnPossess();

	GetWorldTimerManager().SetTimer(Timehandle_Respawn, this, &AMyPlayerController::OnRespawn, 5.f);

}

void AMyPlayerController::OnRespawn()
{
	//AGameMode* GameMode = Cast<AGameMode>(GetWorld()->GetAuthGameMode());

	ARoboHoodGameMode* const MyGameMode = GetWorld()->GetAuthGameMode<ARoboHoodGameMode>();
	if (MyGameMode)
	{
		APawn* NewPawn = MyGameMode->SpawnDefaultPawnFor(this, StartSpot.Get());
		Possess(NewPawn);

		UE_LOG(LogTemp, Warning, TEXT("Respawned"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Respawn Fail"));
	}
}
