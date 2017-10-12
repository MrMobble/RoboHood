// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "TestHUD.generated.h"

/**
 * 
 */
UCLASS()
class ROBOHOOD_API ATestHUD : public AHUD
{
	GENERATED_BODY()

		ATestHUD();
	
public:

	/* An event hook to call HUD text events to display in the HUD. Blueprint HUD class must implement how to deal with this event. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUDEvents")
	void RespawnTextAdd();

	/* An event hook to call HUD text events to display in the HUD. Blueprint HUD class must implement how to deal with this event. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUDEvents")
	void RespawnTextRemove();
};
