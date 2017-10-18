// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RProjectileBase.h"
#include "RProjectileDefault.generated.h"

/**
 * 
 */
UCLASS()
class ROBOHOOD_API ARProjectileDefault : public ARProjectileBase
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

public:
	void Tick(float DeltaTime) override;
	void RProjectileDeathEffect() {}
	void RProjectileDeathEffect_Implementation();
	
};
