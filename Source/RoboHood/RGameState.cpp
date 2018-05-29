// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RGameState.h"

void ARGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Makes Sure The Variable Is Replicated So That Both Server And Client Can See It
	DOREPLIFETIME(ARGameState, ReturnTime);
}
