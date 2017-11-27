// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileRocket.h"

//Other Includes
#include "Public/EngineUtils.h"

//Class Includes
#include "RCharacter.h"

void ARProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{

	if (OtherActor != GetInstigator())
	{
		for (TActorIterator<ARCharacter> aItr(GetWorld()); aItr; ++aItr)
		{
			float distance = GetDistanceTo(*aItr);

			if (distance <= 200.0f)
			{
				UGameplayStatics::ApplyDamage(*aItr, 100.0f - distance / 2, GetInstigatorController(), this, UDamageType::StaticClass());
			}
		}

		Destroy();
	}
}

void ARProjectileRocket::ProjectileDeathEffect_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileDeathParticle, GetActorTransform(), true);
}