// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RProjectileBase.h"
#include "RProjectileRocket.generated.h"

UCLASS()
class ROBOHOOD_API ARProjectileRocket : public ARProjectileBase
{
	GENERATED_BODY()
	
public:	

	void HandleImpact(const FHitResult& Impact);


};
