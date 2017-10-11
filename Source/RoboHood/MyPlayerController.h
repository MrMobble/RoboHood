// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ROBOHOOD_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

		FTimerHandle Timehandle_Respawn;

	public:

		void OnKilled();

		void OnRespawn();
	
	
};
