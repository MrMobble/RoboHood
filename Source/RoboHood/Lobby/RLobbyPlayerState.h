// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "RLobbyPlayerState.generated.h"

UCLASS()
class ROBOHOOD_API ARLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Stuff??
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

public:

	ARLobbyPlayerState();

	void BeginPlay();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	//Updates Player List DUH
	void UpdateLobbyPlayerList();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GameInstance Save Class/Name Functions / Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

public:

	//Server Set Selected Class
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable)
	void ServerSetSelectedClass(int32 Index);
	void ServerSetSelectedClass_Implementation(int32 Index);
	bool ServerSetSelectedClass_Validate(int32 Index) { return true; }

	//Client Set Selected Class
	UFUNCTION(Reliable, Client)
	void ClientSetSelectedClass(int32 Index);
	void ClientSetSelectedClass_Implementation(int32 Index);

//----------------------------------------------------------------------------------------------------------------------

	//Server Save Display Name
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable)
	void ServerSaveDisplayName(const FString& Name);
	void ServerSaveDisplayName_Implementation(const FString& Name);
	bool ServerSaveDisplayName_Validate(const FString& Name) { return true; }

	//Client Save Display Name
	UFUNCTION(Reliable, Client)
	void ClientSaveDisplayName(const FString& Name);
	void ClientSaveDisplayName_Implementation(const FString& Name);

	UPROPERTY(Replicated)
	FString DisplayName;

	UPROPERTY(Replicated)
	bool isServer;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lobby Is Ready/Name Functions And Varibles
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

public:

	//Getter - Setter (IsReady)
	UFUNCTION(BlueprintCallable)
	void SetisReady();

	//Server Set isReady
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetisReady();
	void ServerSetisReady_Implementation();
	bool ServerSetisReady_Validate() { return true; }

	//Get Lobby Is Ready
	UFUNCTION(BlueprintCallable)
	bool GetisReady() { return isReady; }

	//Get The Lobby DisplayName
	UFUNCTION(BlueprintCallable)
	FString GetDisplayName() { return DisplayName; }

	//Get Lobby Is Ready
	UFUNCTION(BlueprintCallable)
	bool CheckAuthority();

	//Set The Lobby DisplayName
	UFUNCTION(BlueprintCallable)
	void SetDisplayName(FString NewName);

	//Sets The DisplayName Only For The Lobby
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetDisplayName(const FString& NewName);
	void ServerSetDisplayName_Implementation(const FString& NewName);
	bool ServerSetDisplayName_Validate(const FString& NewName) { return true; }

	//Only The Server Should Check If To Start Or Not
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable)
	void ServerCountDown(bool bStart);
	void ServerCountDown_Implementation(bool bStart);
	bool ServerCountDown_Validate(bool bStart) { return true; }

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool isReady;
	
};
