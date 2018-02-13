// Property Of Me

#include "RoboHood.h"
#include "RBot.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
ARBot::ARBot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Direction = GetActorForwardVector();

	//Setting up the Collision Sphere Component
	CollisionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

	//Set Root Component
	RootComponent = CollisionSphereComponent;

	//Add Dynamic Event To The CollisionSphere Component
	CollisionSphereComponent->OnComponentHit.AddDynamic(this, &ARBot::OnHit);
}

// Called when the game starts or when spawned
void ARBot::BeginPlay()
{
	Super::BeginPlay();

}

void ARBot::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	NewForwardDirection.Yaw = FMath::FRandRange(-180.f, 180.f);
	SetActorRotation(NewForwardDirection);
	Direction = GetActorForwardVector();
}


// Called every frame
void ARBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Movement->AddInputVector(GetActorForwardVector()*0.5);
}

