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
	UParticleSystem* ProjectileDeathParticle;

	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BProjectile Components
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	/** Sphere Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionSphereComponent;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//Server Functions
	UFUNCTION(Reliable, NetMulticast)
	virtual void ProjectileDeathEffect();
	virtual void ProjectileDeathEffect_Implementation() {}

	//OnHit Function
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {}

	virtual void Destroyed();
};
