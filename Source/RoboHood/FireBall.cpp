// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "FireBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/DamageType.h"
#include "Templates/SharedPointer.h"

// Sets default values
AFireBall::AFireBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = GetRootComponent();
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->SetIsReplicated(true);

	//SetUp StaticMeshes
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComponent->SetSphereRadius(6.f);

	RootComponent = SphereComponent;

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AFireBall::OnOverlapBegin);
	SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);

	// Die after 3 seconds by default
	InitialLifeSpan = 5.f;
}

void AFireBall::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor != GetInstigator() && OtherActor != this)
	{

		Explode();
		Destroy();

		FPointDamageEvent DmgEvent;
		OtherActor->TakeDamage(20, DmgEvent, nullptr, this);
	}
}

void AFireBall::Destroyed()
{
	Explode();
}

void AFireBall::Explode()
{
	SimulateExplosion();
}

void AFireBall::SimulateExplosion_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, GetActorTransform(), true);
}

// Called when the game starts or when spawned
void AFireBall::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFireBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

