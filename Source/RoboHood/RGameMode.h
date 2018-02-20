// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "RGameMode.generated.h"

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

	void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn);
	
};
