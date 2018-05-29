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

	static ConstructorHelpers::FObjectFinder<USoundCue> bounceCueLoader(TEXT("'/Game/Sounds/Weapon_Sounds/Grenade/Bounce_Cue.Bounce_Cue'"));
	BounceSound = bounceCueLoader.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> explosionCueLoader(TEXT("'/Game/Sounds/Weapon_Sounds/Rocket/Rocket_Explode_Cue.Rocket_Explode_Cue'"));
	ExplosionSound = explosionCueLoader.Object;

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
	{
		HandleImpact(HitResult);
		PlayBounceSound();
	}
}

void ARProjectileBase::PlayBounceSound_Implementation()
{
	if (BounceSound && ProjectileMovement->Velocity.Z > AudibleBounceSpeed)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BounceSound, GetActorLocation());
}

void ARProjectileBase::PlayExplosionSound_Implementation()
{
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}
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

void ARProjectileBase::SpawnImpactParticle_Implementation(FTransform SpawnTransform, const FHitResult& Impact)
{
	if (Impact.GetActor() != NULL)
	{
		for (auto& Part : ImpactParticles)
		{
			if (Impact.GetActor()->ActorHasTag(Part.Key))
			{
				if (Part.Value != NULL) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Part.Value, SpawnTransform, true);
			}
		}
	}

	//DEBUG SHIT
	//TArray<FName> ObjTags = Impact.GetActor()->Tags;
	//if (ObjTags.Num() != 0)
	//{
	//	for (int N = 0; N < ObjTags.Num(); N++)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *Impact.GetActor()->GetName(), *ObjTags[N].ToString());
	//	}
	//}
}

void ARProjectileBase::SpawnParticle_Implementation(FTransform SpawnTransform)
{
	//If there is a valid impact particle effect spawn it
	if (DefaultImpactParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultImpactParticle, SpawnTransform, true);
}
