// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileDefault.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"

void ARProjectileDefault::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && OtherActor != GetInstigator())
	{
		Destroy();
	}

	if (OtherActor != GetInstigator())
	{
		Destroy();

		FPointDamageEvent DmgEvent;
		OtherActor->TakeDamage(20, DmgEvent, nullptr, this);
	}
}

void ARProjectileDefault::ProjectileDeathEffect_Implementation()
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