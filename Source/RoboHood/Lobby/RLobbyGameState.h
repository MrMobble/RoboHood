// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "RLobbyGameState.generated.h"

UCLASS()
class ROBOHOOD_API ARLobbyGameState : public AGameState
{
	GENERATED_BODY()	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Server Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	virtual void PreInitializeComponents() override;

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

	//void StartCountDown();

	//void StopCountDown();

	//This FUnction Is Called By The Timer Every 1Second
	//void StartMultiplayerGame();

	//FTimerHandle TimerHandle_CountDown;

	//FTimerHandle TimerHandle_DefaultTimer;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 TimeValue;

	//void DefaultTimer();

};
