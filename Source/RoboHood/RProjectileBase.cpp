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

	// Setting up the Projectile Movement Component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = GetRootComponent();
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = 500.f;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->Bounciness = 0.f;
	bReplicates = true;

	//Setting up the Collision Sphere Component
	CollisionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionSphereComponent->SetSphereRadius(6.0f);

	//Set Root Component
	RootComponent = CollisionSphereComponent;

	//Add Dynamic Event To The CollisionSphere Component
	CollisionSphereComponent->OnComponentHit.AddDynamic(this, &ARProjectileBase::OnHit);
}

void ARProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CollisionSphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}

void ARProjectileBase::Destroyed()
{
	ProjectileDeathEffect();
}