// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RProjectileBase.h"
#include "RProjectileRocket.generated.h"

UCLASS()
class ROBOHOOD_API ARProjectileRocket : public ARProjectileBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARProjectileRocket();
	void ProjectileDeathEffect_Implementation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
