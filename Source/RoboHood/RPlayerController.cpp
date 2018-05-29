// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RPlayerController.h"

//Engine Includes
#include "GameFramework/GameMode.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

//Class Includes
#include "RGameMode.h"
#include "RHUD.h"
#include "RCharacter.h"
#include "RGameState.h"
#include "RGameInstance.h"
#include "RPlayerState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ARPlayerController::ARPlayerController()
{
	bAllowGameActions = true;
}

void ARPlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	// Is this the first frame after the game has ended
	if (bGameEndedFrame)
	{
		bGameEndedFrame = false;

	}
}

void ARPlayerController::OnKilled()
{
	GetWorldTimerManager().SetTimer(Timehandle_Respawn, this, &ARPlayerController::RespawnPlayer, 5.f);
}

void ARPlayerController::ClientPostLogin_Implementation()
{
	URGameInstance* GInstance = Cast<URGameInstance>(GetWorld()->GetGameInstance());
	if (GInstance)
	{
		//Gets The GameInstance And Uses That To Set The Character
		ServerInitSpawn(GInstance->GameCharacters[GInstance->CharacterIndex]);
	}
}

void ARPlayerController::ServerInitSpawn_Implementation(TSubclassOf<APawn> ChosenCharacter)
{
	ReSpawnCharacter = ChosenCharacter;

	SetViewTarget(this);

	ARGameMode* GMode = Cast<ARGameMode>(GetWorld()->GetAuthGameMode());
	if (GMode)
	{
		APawn* NewCharacter = GMode->SpawnPlayer(this, ReSpawnCharacter);
		Possess(NewCharacter);
	}
}

void ARPlayerController::RespawnPlayer()
{
	ARGameMode* GMode = Cast<ARGameMode>(GetWorld()->GetAuthGameMode());
	if (GMode)
	{
		APawn* NewCharacter = GMode->SpawnPlayer(this, ReSpawnCharacter);
		Possess(NewCharacter);
	}
}

void ARPlayerController::PawnPendingDestroy(APawn* P)
{
	FVector LastDeathLocation = P->GetActorLocation();
	CameraLocation = LastDeathLocation + FVector(0, 0, 300.0f);
	CameraRotation = FRotator(-90.0f, 0.0f, 0.0f);

	//Returns CamLocation And Rotation.
	FindDeathCameraSpot(CameraLocation, CameraRotation);

	Super::PawnPendingDestroy(P);

	//This delay is to fix the death camera, for some reason the client needs this :/
	GetWorldTimerManager().SetTimer(Timehandle_Camera, this, &ARPlayerController::SetSpectatorCamera, 0.1f);

}

void ARPlayerController::SetSpectatorCamera()
{
	ClientSetSpectatorCamera(CameraLocation, CameraRotation);
}

void ARPlayerController::ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

	bAllowGameActions = false;

	// Disable controls now the game has ended
	SetIgnoreMoveInput(true);

	// Make sure that we still have valid view target
	SetViewTarget(GetPawn());

	// ONLY PUT CODE HERE WHICH YOU DON'T WANT TO BE DONE DUE TO HOST LOSS
	ARHUD* GameHUD = Cast<ARHUD>(GetHUD());
	if (GameHUD)
	{
		GameHUD->ShowEndScreen(bIsWinner);
	}

	// Flag that the game has just ended (if it's ended due to host loss we want to wait for ClientReturnToMainMenu_Implementation first, incase we don't want to process)
	bGameEndedFrame = true;
}

void ARPlayerController::ClientSetSpectatorCamera_Implementation(FVector _CameraLocation, FRotator _CameraRotation)
{
	SetInitialLocationAndRotation(_CameraLocation, _CameraRotation);
	SetViewTarget(this);
}

bool ARPlayerController::FindDeathCameraSpot(FVector& _CameraLocation, FRotator& _CameraRotation)
{
	const FVector PawnLocation = GetPawn()->GetActorLocation();
	FRotator ViewDir = GetControlRotation();
	ViewDir.Pitch = -45.0f;

	const float YawOffsets[] = { 0.0f, -180.0f, 90.0f, -90.0f, 45.0f, -45.0f, 135.0f, -135.0f };
	const float CameraOffset = 600.0f;
	FCollisionQueryParams TraceParams(FName(TEXT("DeathCam")), true, GetPawn());

	FHitResult HitResult;
	for (int32 i = 0; i < ARRAY_COUNT(YawOffsets); i++)
	{
		FRotator CameraDir = ViewDir;
		CameraDir.Yaw += YawOffsets[i];
		CameraDir.Normalize();

		const FVector TestLocation = PawnLocation - CameraDir.Vector() * CameraOffset;

		const bool bBlocked = GetWorld()->LineTraceSingleByChannel(HitResult, PawnLocation, TestLocation, ECC_Camera, TraceParams);

		if (!bBlocked)
		{
			CameraLocation = TestLocation;
			CameraRotation = CameraDir;
			return true;
		}
	}

	return false;
}

bool ARPlayerController::IsGameInputAllowed() const
{
	return bAllowGameActions;
}
