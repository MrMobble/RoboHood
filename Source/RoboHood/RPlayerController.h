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

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	//TimerHandle For The Re spawn
	FTimerHandle Timehandle_Respawn;

	//Pawn To Re spawn
	TSubclassOf<APawn> ReSpawnCharacter;
	int32 ReSpawnID;

	//Player Re spawn Functions
	void OnKilled();

	//Re spawns The Player
	void RespawnPlayer();

	//Client Called When Joining Game
	UFUNCTION(Reliable, Client)
	void ClientPostLogin();
	void ClientPostLogin_Implementation();

	//Server Init NewPlayer At Start Of Game
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerInitSpawn(TSubclassOf<APawn> ChosenCharacter);
	void ServerInitSpawn_Implementation(TSubclassOf<APawn> ChosenCharacter);
	bool ServerInitSpawn_Validate(TSubclassOf<APawn> ChosenCharacter) { return true; }

	virtual void PawnPendingDestroy(APawn* P) override;

	void SetSpectatorCamera();

	virtual void ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner);

	UFUNCTION(Reliable, Client)
	void ClientSetSpectatorCamera(FVector _CameraLocation, FRotator _CameraRotation);

	bool FindDeathCameraSpot(FVector& _CameraLocation, FRotator& _CameraRotation);

	bool bGameEndedFrame;

	bool bAllowGameActions;

	bool IsGameInputAllowed() const;

protected:

	FVector CameraLocation;
	FRotator CameraRotation;

	FTimerHandle Timehandle_Camera;

};
