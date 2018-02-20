// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RPlayerState.h"

#include "Net/UnrealNetwork.h" 

#include "RGameInstance.h"

void ARPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPlayerState, DisplayName);
}

void ARPlayerState::ClientPostLogin_Implementation()
{
	URGameInstance* GInstance = Cast<URGameInstance>(GetWorld()->GetGameInstance());
	if (GInstance)
	{
		SetDisplayName(GInstance->CharacterDisplayName);
	}
}

void ARPlayerState::SetDisplayName(FString NewName)
{
	DisplayName = NewName;

	if (Role < ROLE_Authority)
	{
		ServerSetDisplayName(NewName);
	}
}

void ARPlayerState::ServerSetDisplayName_Implementation(const FString& NewName)
{
	SetDisplayName(NewName);
}

void ARPlayerState::AddScore()
{
	Score++;
}

