// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RLobbyPlayerState.h"

//Engine Includes
#include "Net/UnrealNetwork.h" 

//Class Includes
#include "RGameInstance.h"
#include "RLobbyGameState.h"
#include "RLobbyPlayerController.h"

ARLobbyPlayerState::ARLobbyPlayerState()
{
	isReady = false;
	DisplayName = FString("Test");
}

void ARLobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();

}

void ARLobbyPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARLobbyPlayerState, isReady);
	DOREPLIFETIME(ARLobbyPlayerState, DisplayName);
	DOREPLIFETIME(ARLobbyPlayerState, isServer);
}


//This is not used anymore
void ARLobbyPlayerState::UpdateLobbyPlayerList()
{
	ARLobbyGameState* LGState = Cast<ARLobbyGameState>(GetWorld()->GetGameState());
	if (LGState)
	{
		//Updates The PlayerList When Someone Joins The Lobby
		LGState->UpdatePlayerList();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GameInstance Save Class/Name Functions / Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ARLobbyPlayerState::ServerSetSelectedClass_Implementation(int32 Index)
{
	ClientSetSelectedClass(Index);
}

void ARLobbyPlayerState::ClientSetSelectedClass_Implementation(int32 Index)
{
	URGameInstance* GInstance = Cast<URGameInstance>(GetWorld()->GetGameInstance());
	if (GInstance) 
	{ 
		GInstance->CharacterIndex = Index;
	}
}

//----------------------------------------------------------------------------------------------------------------------

void ARLobbyPlayerState::ServerSaveDisplayName_Implementation(const FString& Name)
{
	ClientSaveDisplayName(Name);
}

void ARLobbyPlayerState::ClientSaveDisplayName_Implementation(const FString& Name)
{
	URGameInstance* GInstance = Cast<URGameInstance>(GetWorld()->GetGameInstance());
	if (GInstance)
	{ 
		GInstance->CharacterDisplayName = Name;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lobby Is Ready/Name Functions And Varibles
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

void ARLobbyPlayerState::SetisReady() 
{ 
	if (Role < ROLE_Authority) { ServerSetisReady(); }

	isReady = !isReady; 
}

void ARLobbyPlayerState::ServerSetisReady_Implementation()
{
	SetisReady();
}

//----------------------------------------------------------------------------------------------------------------------

bool ARLobbyPlayerState::CheckAuthority()
{
	if (Role == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("IsServer"));
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NotServer"));
		return false;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Result: %s"), (isServer ? TEXT("True") : TEXT("False")));
}

void ARLobbyPlayerState::SetDisplayName(FString NewName)
{
	if (Role < ROLE_Authority)
	{
		ServerSetDisplayName(NewName);
	}

	DisplayName = NewName;

	UpdateLobbyPlayerList();
}

void ARLobbyPlayerState::ServerSetDisplayName_Implementation(const FString& NewName)
{
	SetDisplayName(NewName);
}

//----------------------------------------------------------------------------------------------------------------------

void ARLobbyPlayerState::ServerCountDown_Implementation()
{
	ARLobbyGameState* LGState = Cast<ARLobbyGameState>(GetWorld()->GetGameState());
	if (LGState)
	{
		//Starts The CountDown On The Server
		LGState->StartCountDown();
	}
}
