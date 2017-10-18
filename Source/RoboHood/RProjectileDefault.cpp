// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileDefault.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"

void ARProjectileDefault::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this)
	{
		RProjectileDeathEffect();
		Destroy();
	}

	if (OtherActor)
	{
		RProjectileDeathEffect();
		Destroy();

		FPointDamageEvent DmgEvent;
		OtherActor->TakeDamage(20, DmgEvent, nullptr, this);
	}
}

void ARProjectileDefault::RProjectileDeathEffect_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileDeathParticle, GetActorTransform(), true);
}

// Called when the game starts or when spawned
void ARProjectileDefault::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARProjectileDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}