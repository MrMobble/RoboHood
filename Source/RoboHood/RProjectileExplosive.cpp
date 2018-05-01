// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileExplosive.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RCharacter.h"
#include "Public/EngineUtils.h"

void ARProjectileExplosive::HandleImpact(const FHitResult& Impact)
{
	if (Impact.Actor->IsA(ARCharacter::StaticClass()))
	{
		Destroy();
	}
}

void ARProjectileExplosive::HandleDeath()
{
	ApplyRadialDamage(ProjectileDamage, ProjectileRadius);

	//DrawDebugSphere(GetWorld(), GetActorLocation(), 500, 32, FColor(255, 0, 0), false, 2.5f, false, 5.f);

	FTransform const SpawnTransform(GetActorRotation(), GetActorLocation(), ParticleScale);
	SpawnParticle(SpawnTransform);
	PlayExplosionSound();
}

