// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "RLobbyInterface.generated.h"

UCLASS()
class ROBOHOOD_API URLobbyInterface : public UUserWidget
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lobby Widget Callable Events
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "UpdateFunctions")
	void UpdatePlayerList();

	UFUNCTION(BlueprintImplementableEvent, Category = "LobbyFunction")
	void ServerTravel();

	UFUNCTION(BlueprintImplementableEvent, Category = "LobbyFunction")
	void ShowLoadingScreen();
	
};
