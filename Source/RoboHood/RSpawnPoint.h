// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/PlayerStart.h"
#include "RSpawnPoint.generated.h"

UCLASS()
class ROBOHOOD_API ARSpawnPoint : public APlayerStart
{
	GENERATED_BODY()

public:

	ARSpawnPoint();

	bool bIsUsed;

	void SetUsed();

protected:

	FTimerHandle Timehandle_SpawnPoint;

	void Reset();
	
	
};
