// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "RLobbyGameState.generated.h"

//Variable Defines
#define CountDownTime 3.0f

UCLASS()
class ROBOHOOD_API ARLobbyGameState : public AGameState
{
	GENERATED_BODY()	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Server Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//Framework Replicate Crap
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	void UpdatePlayerList();

	//Multicast To Update The PlayerList
	UFUNCTION(Reliable, NetMulticast)
	void MultiUpdatePlayerList();
	void MultiUpdatePlayerList_Implementation();

	//Multicast To Display The LoadingScreen
	UFUNCTION(Reliable, NetMulticast)
	void MultiDisplayLoadingScreen();
	void MultiDisplayLoadingScreen_Implementation();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Start Game Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//Starts The CountDown Timer
	void StartCountDown();

	//This FUnction Is Called By The Timer Every 1Second
	void CountDown();

	//Handle For The CountDown Timer
	FTimerHandle CountDownHandle;

	//Replicated Int
	UPROPERTY(Replicated)
	int32 IntCountDown;

	//Function So Blueprints Can Get The IntCountDown Variable
	UFUNCTION(BlueprintCallable)
	int32 GetIntCountDown() { return IntCountDown; }

};
