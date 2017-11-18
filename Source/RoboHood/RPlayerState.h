// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/PlayerState.h"
#include "RPlayerState.generated.h"

UCLASS()
class ROBOHOOD_API ARPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	UFUNCTION(Reliable, Client)
	void ClientPostLogin();
	void ClientPostLogin_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetDisplayName(const FString& NewName);
	void ServerSetDisplayName_Implementation(const FString& NewName);
	bool ServerSetDisplayName_Validate(const FString& NewName) { return true; }

	void SetDisplayName(FString NewName);

	UPROPERTY(Replicated, BlueprintReadOnly)
	FString DisplayName;
	
	
};
