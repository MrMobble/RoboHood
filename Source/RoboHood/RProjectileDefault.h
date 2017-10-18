// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "RProjectileBase.h"
#include "RProjectileDefault.generated.h"

UCLASS()
class ROBOHOOD_API ARProjectileDefault : public ARProjectileBase
{
	GENERATED_BODY()

protected:

	void BeginPlay() override;
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:

	void Tick(float DeltaTime) override;

	void ProjectileDeathEffect_Implementation();
	
};
