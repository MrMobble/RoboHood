// Property Of Me

#include "RoboHood.h"
#include "KillVolume.h"

#include "RCharacter.h"


// Sets default values
AKillVolume::AKillVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("KillVolume"));
	Volume->SetCollisionObjectType(ECC_WorldStatic);
	Volume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	RootComponent = Volume;

	Volume->OnComponentBeginOverlap.AddDynamic(this, &AKillVolume::OnOverlapBegin);
}

//Initialise The Weapon
void AKillVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Volume->SetBoxExtent(FVector(VolumeX, VolumeY, 10.0f));
}

// Called when the game starts or when spawned
void AKillVolume::BeginPlay()
{
	Super::BeginPlay();

	Volume->SetBoxExtent(FVector(VolumeX, VolumeY, 10.0f));
}

void AKillVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARCharacter* Character = Cast<ARCharacter>(OtherActor);
	if (Character != NULL)
	{
		Character->TakeDamage(200.0f, FDamageEvent(), nullptr, nullptr);
		UE_LOG(LogTemp, Warning, TEXT("Called"));
	}
}

