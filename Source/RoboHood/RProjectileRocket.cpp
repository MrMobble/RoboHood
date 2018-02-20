// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileRocket.h"

//Other Includes
#include "Public/EngineUtils.h"

//Class Includes
#include "RCharacter.h"

void ARProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{

	for (TActorIterator<ARCharacter> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);
		if (distance <= 400.0f)
		{
			FPointDamageEvent DmgEvent;
			aItr->TakeDamage(70.0f - (distance / 7), DmgEvent, GetInstigatorController(), this);
		}
	}

	Destroy();
}

void ARProjectileRocket::ProjectileDeathEffect_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileDeathParticle, GetActorTransform(), true);
}