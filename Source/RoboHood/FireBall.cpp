// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "FireBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/DamageType.h"

// Sets default values
AFireBall::AFireBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionEffect (TEXT("/Game/StarterContent/Particles/P_Explosion"));
	ParticleSystemTemplate = ExplosionEffect.Object;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = GetRootComponent();
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->SetIsReplicated(true);

	//SetUp StaticMeshes
	ArrowHeadComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow_Head"));

	RootComponent = ArrowHeadComponent;

	ArrowHeadComponent->OnComponentHit.AddDynamic(this, &AFireBall::OnHit);
	// Die after 3 seconds by default
	InitialLifeSpan = 10.0f;
}

void AFireBall::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this)
	{
		Explode();
		Destroy();
	}

	if (OtherActor)
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
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystemTemplate, GetActorTransform(), true);
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

