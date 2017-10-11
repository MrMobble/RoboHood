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

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	/** Arrow_Head Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ArrowHeadComponent;

	//Particle system variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", Meta = (BlueprintProtected = "true"))
	class UParticleSystemComponent* ParticleSystem;
	UParticleSystem* ParticleSystemTemplate;

	UPrimitiveComponent* CollisionComp;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	TSubclassOf<UDamageType> DamageType;

	//Client Side Function
	void Explode();

	//Server Functions
	UFUNCTION(Reliable, NetMulticast)
	void SimulateExplosion();
	void SimulateExplosion_Implementation();
	virtual void Destroyed();

	
	
};
