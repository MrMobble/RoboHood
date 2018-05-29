// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "RGameState.generated.h"

UCLASS()
class ROBOHOOD_API ARGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	//Framework Replicate Crap
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float ReturnTime;
	
};
