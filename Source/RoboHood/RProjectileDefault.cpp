// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileDefault.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"

void ARProjectileDefault::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherActor != GetInstigator())
	{
		Destroy();
	}

	if (OtherActor != GetInstigator())
	{
		Destroy();

		FPointDamageEvent DmgEvent;
		OtherActor->TakeDamage(5, DmgEvent, nullptr, this);
	}
}

void ARProjectileDefault::ProjectileDeathEffect_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileDeathParticle, GetActorTransform(), true);
}