// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "RProjectileBase.h"
#include "RProjectileExplosive.generated.h"

UCLASS()
class ROBOHOOD_API ARProjectileExplosive : public ARProjectileBase
{
	GENERATED_BODY()
	
public:

	void HandleImpact(const FHitResult& Impact);

	void HandleDeath();
};
