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

	//TimerHandle For The Respawn
	FTimerHandle Timehandle_Respawn;

	//Player Respawn Functions
	void OnKilled();
	void OnRespawn();

	//Server Function For Displaying A HUD Message
	UFUNCTION(Reliable, Client)
	void ClientHUDMessage();
	void ClientHUDMessage_Implementation();
	
	
};
