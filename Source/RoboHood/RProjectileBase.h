// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RProjectileBase.generated.h"

UCLASS()
class ROBOHOOD_API ARProjectileBase : public AActor
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic Weapon Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//PostInitComponents
	virtual void PostInitializeComponents() override;

	//Sets default values for this actor's properties
	ARProjectileBase();

	//OnHit Particle Effect
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Attributes", Meta = (BlueprintProtected = "true"))
	UParticleSystem* DefaultImpactParticle;

	//ImpactParticles For Basic Ammo
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Attributes", Meta = (BlueprintProtected = "true"))
	TMap<FName, UParticleSystem*> ImpactParticles;

	//Scale Of The Particles, Applies To All Particles
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Attributes", Meta = (BlueprintProtected = "true"))
	FVector ParticleScale;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Attributes", Meta = (BlueprintProtected = "true"))
	float ProjectileDamage;

	//Radius of explosion if applies.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Attributes", Meta = (BlueprintProtected = "true"))
	float ProjectileRadius;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BProjectile Components
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	/** Projectile movement component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UProjectileMovementComponent* ProjectileMovement;

	/** Sphere Component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComponent;

	/** Sphere Component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UParticleSystemComponent* ParticleComponent;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//OnHit Function
	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);

	UFUNCTION()
	void OnBounce(const FHitResult& HitResult, const FVector& ImpactVelocity);

	virtual void HandleImpact(const FHitResult& Impact) {}
	virtual void HandleDeath() {}

	void ApplyRadialDamage(float BaseDamage, float Radius);
	void ApplyDamage(const FHitResult& Impact, float BaseDamage);

	virtual void Destroyed();

	//Server Functions
	UFUNCTION(Reliable, NetMulticast)
	void SpawnImpactParticle(FTransform SpawnTransform, const FHitResult& Impact);
	void SpawnImpactParticle_Implementation(FTransform SpawnTransform, const FHitResult& Impact);

	UFUNCTION(Reliable, NetMulticast)
	void SpawnParticle(FTransform SpawnTransform);
	void SpawnParticle_Implementation(FTransform SpawnTransform);

};
