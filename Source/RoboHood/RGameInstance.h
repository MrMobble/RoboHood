// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "RGameInstance.generated.h"

UCLASS()
class ROBOHOOD_API URGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Multiplayer Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

public:

	//GameInstance Constrcutor
	URGameInstance();

	//This Is An Array Of Characters You Declare In The BP Version Of The Game_Instance
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<APawn>> GameCharacters;

	//The Index Of The Character You Selected
	UPROPERTY(BlueprintReadOnly)
	int32 CharacterIndex;

	//Your DisplayName 
	UPROPERTY(BlueprintReadOnly)
	FString CharacterDisplayName;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Widget Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//Menu Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	TSubclassOf<UUserWidget> MenuWidget;
	UUserWidget* TheMenuWidget;

	//Lobby Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	TSubclassOf<UUserWidget> LobbyWidget;
	UUserWidget* TheLobbyWidget;

	//Game Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	TSubclassOf<UUserWidget> GameWidget;
	UUserWidget* TheGameWidget;

	UFUNCTION(BlueprintCallable)
	UUserWidget* CreateMenuWidget(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	UUserWidget* CreateLobbyWidget(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	UUserWidget* CreateGameWidget(APlayerController* PlayerController);

	//Getters
	UFUNCTION(BlueprintCallable)
	UUserWidget* GetLobbyWidget() { return TheLobbyWidget; }

};
