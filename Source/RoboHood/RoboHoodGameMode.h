// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "RoboHoodGameMode.generated.h"

UCLASS(minimalapi)
class ARoboHoodGameMode : public AGameModeBase
{
	GENERATED_BODY()

	public:

		ARoboHoodGameMode();

		AActor* ChooseSpawnLocation(AController* Player);
};



