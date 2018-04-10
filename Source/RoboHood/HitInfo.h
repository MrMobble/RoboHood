// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "HitInfo.generated.h"

/** replicated information on a hit we've taken */
USTRUCT()
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

	/** Who hit us */
	UPROPERTY()
	class AController* Killer;

	/** Who actually caused the damage */
	UPROPERTY()
	class AActor* DamageCauser;

	/** Rather this was a kill */
	UPROPERTY()
	uint32 bKilled : 1;

};