// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ARProjectileBase::ARProjectileBase()
{
	// Set this actor to call Tick() severy frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setting up the Projectile Movement Component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = GetRootComponent();
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = 500.f;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->Bounciness = 0.f;
	//ProjectileMovement->SetIsReplicated(true);

	// Bool for sticky projectiles
	Sticky = false;

	// Setting up the Collision Sphere Component
	CollisionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionSphereComponent->SetSphereRadius(6.f);

	RootComponent = CollisionSphereComponent;

	CollisionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ARProjectileBase::OnOverlapBegin);
	CollisionSphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}

// Called when the game starts or when spawned
void ARProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARProjectileBase::Destroyed()
{
	ProjectileDeathEffect();
}