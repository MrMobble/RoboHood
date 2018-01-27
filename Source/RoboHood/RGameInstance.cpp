// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RGameInstance.h"

#include "GameFramework/Actor.h"

#include "Blueprint/UserWidget.h"

#include "RCharacter.h"
#include "Lobby/RLobbyPlayerController.h"

URGameInstance::URGameInstance()
{
	CharacterIndex = int32(0);
	CharacterDisplayName = FString("DefaultName");
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Widget Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

UUserWidget* URGameInstance::CreateMenuWidget(APlayerController* PlayerController)
{
	if (MenuWidget)
	{
		TheMenuWidget = CreateWidget<UUserWidget>(PlayerController, MenuWidget);
		if (TheMenuWidget)
		{
			TheMenuWidget->AddToViewport();
		}
	}
	return TheMenuWidget;
}

UUserWidget* URGameInstance::CreateLobbyWidget(APlayerController* PlayerController)
{
	if (LobbyWidget)
	{
		TheLobbyWidget = CreateWidget<UUserWidget>(PlayerController, LobbyWidget);
		if (TheLobbyWidget)
		{
			TheLobbyWidget->AddToViewport();
		}
	}
	return TheLobbyWidget;
}

UUserWidget* URGameInstance::CreateGameWidget(APlayerController* PlayerController)
{
	if (GameWidget)
	{
		TheGameWidget = CreateWidget<UUserWidget>(PlayerController, GameWidget);
		if (TheGameWidget)
		{
			TheGameWidget->AddToViewport();
		}
	}
	return TheGameWidget;
}

TArray<FString> URGameInstance::GetCurrentSettings(FString Section)
{
	return URSettingsManager::GetSettings(Section);
}

float URGameInstance::GetAudioSetting(FName ConVar, FString Section)
{
	return URSettingsManager::GetAudio(ConVar, Section);
}