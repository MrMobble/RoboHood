// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileExplosive.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RCharacter.h"
#include "Public/EngineUtils.h"

ARProjectileExplosive::ARProjectileExplosive()
{
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.8f;

	CollisionSphereComponent->SetSphereRadius(18.0f);

	RootComponent = CollisionSphereComponent;

	InitialLifeSpan = 3.0f;
}

void ARProjectileExplosive::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA(ARCharacter::StaticClass()) && OtherActor != GetInstigator())
	{
		Destroy();		
	}
}

void ARProjectileExplosive::ProjectileDeathEffect_Implementation()
{
	for (TActorIterator<ARCharacter> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);
		if (distance <= 350.0f)
		{
			FPointDamageEvent DmgEvent;
			aItr->TakeDamage(60.0f - (distance / 6), DmgEvent, nullptr, this);
		}
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileDeathParticle, GetActorTransform(), true);
}


