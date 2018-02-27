// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "RProjectileBase.h"
#include "RProjectileDefault.generated.h"

UCLASS()
class ROBOHOOD_API ARProjectileDefault : public ARProjectileBase
{
	GENERATED_BODY()

public:

	void HandleImpact(const FHitResult& Impact);
};
