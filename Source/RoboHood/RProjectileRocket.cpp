// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileRocket.h"

//Other Includes
#include "Public/EngineUtils.h"

//Class Includes
#include "RCharacter.h"

void ARProjectileRocket::HandleImpact(const FHitResult& Impact)
{
	ApplyRadialDamage(ProjectileDamage, ProjectileRadius);

	//DrawDebugSphere(GetWorld(), GetActorLocation(), 500, 32, FColor(255, 0, 0), false, 2.5f, false, 5.f);

	FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint + Impact.ImpactNormal * 10.0f, ParticleScale);
	SpawnParticle(SpawnTransform);

	Destroy();
}