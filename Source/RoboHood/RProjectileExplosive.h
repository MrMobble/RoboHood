// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "RProjectileBase.h"
#include "RProjectileExplosive.generated.h"

UCLASS()
class ROBOHOOD_API ARProjectileExplosive : public ARProjectileBase
{
	GENERATED_BODY()
	
protected:

	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

public:

	ARProjectileExplosive();
	void ProjectileDeathEffect_Implementation();	
};
