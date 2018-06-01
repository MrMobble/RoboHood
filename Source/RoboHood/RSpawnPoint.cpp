// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RSpawnPoint.h"


ARSpawnPoint::ARSpawnPoint()
{
	bIsUsed = false;
}

void ARSpawnPoint::SetUsed()
{
	bIsUsed = true;
	GetWorldTimerManager().SetTimer(Timehandle_SpawnPoint, this, &ARSpawnPoint::Reset, 2.0f);
}

void ARSpawnPoint::Reset()
{
	bIsUsed = false;
}
