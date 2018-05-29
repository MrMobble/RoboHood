// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RGameMode.h"

//Other Classes
#include "RCharacter.h"
#include "RPlayerController.h"
#include "RSpawnPoint.h"
#include "RHUD.h"
#include "RPlayerState.h"
#include "RGameState.h"
#include "RGameInstance.h"

//Other Includes
#include "Public/EngineUtils.h"
#include "GameFramework/Actor.h"

ARGameMode::ARGameMode()
{
	//Player Controller
	PlayerControllerClass = ARPlayerController::StaticClass();

	//Player State
	PlayerStateClass = ARPlayerState::StaticClass();

	//Game State
	GameStateClass = ARGameState::StaticClass();

	//HUD
	static ConstructorHelpers::FObjectFinder<UClass> Blueprint(TEXT("/Game/UI/BP_HUD.BP_HUD_C"));
	if (Blueprint.Object != NULL)
	{
		HUDClass = Blueprint.Object;
	}
}

void ARGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &ARGameMode::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void ARGameMode::DefaultTimer()
{
	ARGameState* const MyGameState = Cast<ARGameState>(GameState);
	if (MyGameState && MyGameState->ReturnTime > 0)
	{
		MyGameState->ReturnTime--;
	}
}

//Find Random SpawnPoint FUnction
AActor* ARGameMode::ChooseSpawnLocation(AController* PlayerController)
{
	if (PlayerController)
	{
		TArray<ARSpawnPoint*> SpawnPoints;
		for (TActorIterator<ARSpawnPoint> Itr(GetWorld()); Itr; ++Itr)
		{
			SpawnPoints.Add(*Itr);
		}

		if (SpawnPoints.Num() != 0)
			return SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];	
	}

	return nullptr;
}

APawn* ARGameMode::SpawnPlayer(AController* PlayerController, TSubclassOf<APawn> ChosenCharacter)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnLocation = ChooseSpawnLocation(PlayerController);
	if (SpawnLocation)
	{
		APawn* NewCharacter = GetWorld()->SpawnActor<APawn>(ChosenCharacter, SpawnLocation->GetActorLocation(), SpawnLocation->GetActorRotation(), ActorSpawnParams);
		return NewCharacter;
	}

	return nullptr;
}

void ARGameMode::PostLogin(APlayerController* NewPlayer)
{
	ARPlayerController* PController = Cast<ARPlayerController>(NewPlayer);
	if (PController)
	{
		PController->ClientPostLogin();
	}

	ARPlayerState* PState = Cast<ARPlayerState>(PController->PlayerState);
	if (PState)
	{
		PState->ClientPostLogin();
	}
}

void ARGameMode::BeginPlay()
{
	StartMatch();
}

void ARGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn)
{
	ARPlayerState* KillerPlayerState = Killer ? Cast<ARPlayerState>(Killer->PlayerState) : NULL;
	ARPlayerController* VictimPlayerController = KilledPlayer ? Cast<ARPlayerController>(KilledPlayer) : NULL;

	if (KillerPlayerState && Killer != KilledPlayer)
	{
		KillerPlayerState->AddScore();
	}

	DetermineGameState();

	if (VictimPlayerController && IsMatchInProgress())
	{
		VictimPlayerController->OnKilled();
	}

}

void ARGameMode::DetermineMatchWinner()
{
	ARGameState* const MyGameState = CastChecked<ARGameState>(GameState);

	float BestScore = 0;
	int32 BestPlayer = -1;
	int32 NumBestPlayers = 0;

	for (int32 i = 0; i < MyGameState->PlayerArray.Num(); i++)
	{
		const float PlayerScore = MyGameState->PlayerArray[i]->Score;
		if (BestScore < PlayerScore)
		{
			BestScore = PlayerScore;
			BestPlayer = i;
			NumBestPlayers = 1;
		}
		else if (BestScore == PlayerScore)
		{
			NumBestPlayers++;
		}
	}

	WinnerPlayerState = (NumBestPlayers == 1) ? Cast<ARPlayerState>(MyGameState->PlayerArray[BestPlayer]) : NULL;

}


bool ARGameMode::IsWinner(ARPlayerState* PlayerState) const
{
	return PlayerState && PlayerState == WinnerPlayerState;
}

void ARGameMode::DetermineGameState()
{
	ARGameState* const MyGameState = CastChecked<ARGameState>(GameState);

	for (int32 i = 0; i < MyGameState->PlayerArray.Num(); i++)
	{
		const float PlayerScore = MyGameState->PlayerArray[i]->Score;
		if (PlayerScore >= 2)
		{
			if (GetMatchState() == MatchState::InProgress)
			{
				FinishMatch();
			}
		}
	}

}

void ARGameMode::FinishMatch()
{
	ARGameState* const MyGameState = Cast<ARGameState>(GameState);
	if (IsMatchInProgress())
	{
		EndMatch();
		DetermineMatchWinner();

		// notify players
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			ARPlayerState* PlayerState = Cast<ARPlayerState>((*It)->PlayerState);
			const bool bIsWinner = IsWinner(PlayerState);

			(*It)->GameHasEnded(NULL, bIsWinner);
		}

		// lock all pawns
		// pawns are not marked as keep for seamless travel, so we will create new pawns on the next match rather than
		// turning these back on.
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			(*It)->TurnOff();
		}

		GetWorldTimerManager().SetTimer(TimeHandle_Return, this, &ARGameMode::ReturnToLobby, 10.0f);
		MyGameState->ReturnTime = 10;
	}

}

void ARGameMode::ReturnToLobby()
{
	URGameInstance* GameInstance = Cast<URGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->EndServerTravel(2);
	}
}


