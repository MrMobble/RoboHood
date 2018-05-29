// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "RGameMode.generated.h"

class ARPlayerState;

UCLASS()
class ROBOHOOD_API ARGameMode : public AGameMode
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//Default Constructor
	ARGameMode();

	//Choose Spawn Location
	AActor* ChooseSpawnLocation(AController* PlayerController);

	//Spawn A Player And Return Pointer To It
	APawn* SpawnPlayer(AController* PlayerController, TSubclassOf<APawn> ChosenCharacter);

	//PostLegin FUnction
	virtual void PostLogin(APlayerController* NewPlayer);

	virtual void BeginPlay();

	void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn);

	void DetermineMatchWinner();

	bool IsWinner(ARPlayerState* PlayerState) const;

	UPROPERTY(Transient)
	ARPlayerState* WinnerPlayerState;

	void DetermineGameState();

	UFUNCTION(Exec)
	void FinishMatch();
	
	void ReturnToLobby();

	FTimerHandle TimerHandle_DefaultTimer;

	virtual void PreInitializeComponents() override;

	void DefaultTimer();

	FTimerHandle TimeHandle_Return;
};
