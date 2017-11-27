// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/HUD.h"
#include "RHUD.generated.h"

UCLASS()
class ROBOHOOD_API ARHUD : public AHUD
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	ARHUD(const FObjectInitializer& ObjectInitializer);

	FCanvasIcon CenterDotIcon;

	/** Main HUD update loop. */
	virtual void DrawHUD() override;

	void DrawCenterDot();

	//This Is An Event You Can Call From C++ In Blueprint
	UFUNCTION(BlueprintImplementableEvent, Category = "HUDEvents")
	void RespawnTextAdd();

	//This Is An Event You Can Call From C++ In Blueprint
	UFUNCTION(BlueprintImplementableEvent, Category = "HUDEvents")
	void RespawnTextRemove();
	
	
	
};
