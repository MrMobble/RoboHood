// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/PlayerController.h"
#include "RPlayerController.generated.h"

UCLASS()
class ROBOHOOD_API ARPlayerController : public APlayerController
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
public:

	ARPlayerController();

	//TimerHandle For The Respawn
	FTimerHandle Timehandle_Respawn;

	//Pawn To Respawn
	TSubclassOf<APawn> ReSpawnCharacter;
	int32 ReSpawnID;

	//Player Respawn Functions
	void OnKilled();

	//Respawns The Player
	void RespawnPlayer();

	//Client Called When Joinging Game
	UFUNCTION(Reliable, Client)
	void ClientPostLogin();
	void ClientPostLogin_Implementation();

	//Server Init NewPlayer At Start Of Game
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerInitSpawn(TSubclassOf<APawn> ChosenCharacter);
	void ServerInitSpawn_Implementation(TSubclassOf<APawn> ChosenCharacter);
	bool ServerInitSpawn_Validate(TSubclassOf<APawn> ChosenCharacter) { return true; }

	APlayerCameraManager* GetCameraManager() { return PlayerCameraManager; }

	virtual void PawnPendingDestroy(APawn* P) override;

	UFUNCTION(Reliable, Client)
	void ClientSetSpectatorCamera(FVector CameraLocation, FRotator CameraRotation);

	bool FindDeathCameraSpot(FVector& CameraLocation, FRotator& CameraRotation);

};
