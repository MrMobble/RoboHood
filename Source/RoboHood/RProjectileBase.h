// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RProjectileBase.generated.h"

UCLASS()
class ROBOHOOD_API ARProjectileBase : public AActor
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	ARProjectileBase();

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Attributes", Meta = (BlueprintProtected = "true"))
	bool Sticky;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Attributes", Meta = (BlueprintProtected = "true"))
	UParticleSystem* ProjectileDeathParticle;

	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	/** Sphere Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionSphereComponent;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {}
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {}

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Server Functions
	UFUNCTION(Reliable, NetMulticast)
	virtual void ProjectileDeathEffect();
	virtual void ProjectileDeathEffect_Implementation() {}

	virtual void Destroyed();
};
