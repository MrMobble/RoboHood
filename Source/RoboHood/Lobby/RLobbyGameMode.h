// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "RLobbyGameMode.generated.h"

#define CountDownTime 8.0f

UCLASS()
class ROBOHOOD_API ARLobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	virtual void PreInitializeComponents() override;

	//Default Constructor
	ARLobbyGameMode();

	//Spawn A Player And Return Pointer To It
	AActor* SpawnPlayer(TSubclassOf<AActor> ChosenCharacter);

public:

	void StartCountDown();

	void StopCountDown();

	//This FUnction Is Called By The Timer Every 1Second
	void StartMultiplayerGame();

	FTimerHandle TimerHandle_CountDown;

	FTimerHandle TimerHandle_DefaultTimer;

	void DefaultTimer();
	
};
