// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileRocket.h"
#include "Public/EngineUtils.h"
#include "RCharacter.h"

ARProjectileRocket::ARProjectileRocket()
{

}

void ARProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	for (TActorIterator<ARCharacter> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);

		if (distance <= 200.0f)
		{
			UGameplayStatics::ApplyDamage(*aItr, 100.0f - distance / 2, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}
}

void ARProjectileRocket::ProjectileDeathEffect_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileDeathParticle, GetActorTransform(), true);
}

// Called when the game starts or when spawned
void ARProjectileRocket::BeginPlay()
{
	Super::BeginPlay();
	CollisionSphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}

// Called every frame
void ARProjectileRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}