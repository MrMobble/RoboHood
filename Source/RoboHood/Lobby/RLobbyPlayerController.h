// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "RLobbyPlayerController.generated.h"

UCLASS()
class ROBOHOOD_API ARLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

	void BeginPlay();

public:

	AActor* SelectionReference =  nullptr;

	UFUNCTION(BlueprintCallable)
	void SpawnSelectedClass(int32 Index);

	UFUNCTION(BlueprintCallable)
	bool CheckAuthority();

	void UpdateLobbyList();
};
