// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileDefault.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"

void ARProjectileDefault::HandleImpact(const FHitResult& Impact)
{
	if (Impact.Actor != GetInstigator())
	{
		ApplyDamage(Impact, 5.0f);
	}

	DrawDebugPoint(GetWorld(), GetActorLocation(), 7.5, FColor(255, 0, 0), false, 2.5f);

	FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint + Impact.ImpactNormal * 10.0f);
	SpawnImpactParticle(SpawnTransform);
	
	Destroy();
}