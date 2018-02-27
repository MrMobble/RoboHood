// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileBase.h"

//Other Includes
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ARProjectileBase::ARProjectileBase()
{
	// Set this actor to call Tick() severy frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Setting up the Collision Sphere Component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComponent->SetSphereRadius(6.0f);
	RootComponent = CollisionComponent;

	//Particle Component
	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComponent->SetupAttachment(RootComponent);

	// Setting up the Projectile Movement Component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = GetRootComponent();
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateMovement = true;

}

void ARProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Add Dynamic Event To The CollisionSphere Component
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &ARProjectileBase::OnImpact);
	ProjectileMovement->OnProjectileBounce.AddDynamic(this, &ARProjectileBase::OnBounce);
	
	//Gets Instigator And Adds It To The IngoreWhenMoving Array.
	CollisionComponent->MoveIgnoreActors.Add(Instigator);
}

void ARProjectileBase::OnImpact(const FHitResult & HitResult)
{
	if (Role == ROLE_Authority)
		HandleImpact(HitResult);
	
}

void ARProjectileBase::OnBounce(const FHitResult& HitResult, const FVector& ImpactVelocity)
{
	if (Role == ROLE_Authority)
		HandleImpact(HitResult);
	
}

void ARProjectileBase::Destroyed()
{
	if (Role == ROLE_Authority)
		HandleDeath();
	
}

void ARProjectileBase::ApplyRadialDamage(float BaseDamage, float Radius)
{
	UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), Radius, UDamageType::StaticClass(), TArray<AActor*>(), this, GetInstigatorController());
}

void ARProjectileBase::ApplyDamage(const FHitResult& Impact, float BaseDamage)
{
	Impact.GetActor()->TakeDamage(BaseDamage, FDamageEvent(), GetInstigatorController(), this);
}

void ARProjectileBase::SpawnImpactParticle_Implementation(FTransform SpawnTransform)
{
	//If there is a valid impact particle effect spawn it
	if (ProjectileImpactParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileImpactParticle, SpawnTransform, true);
}
