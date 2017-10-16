// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FireBall.generated.h"

UCLASS()
class ROBOHOOD_API AFireBall : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AFireBall();

protected:

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	/** Sphere Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;

	UPrimitiveComponent* CollisionComp;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Particles", Meta = (BlueprintProtected = "true"))
	UParticleSystem* Particle;


protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	/** Called when projectile hits something */
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Client Side Function
	void Explode();

	//Server Functions
	UFUNCTION(Reliable, NetMulticast)
	void SimulateExplosion();
	void SimulateExplosion_Implementation();

	virtual void Destroyed();

	
	
};
