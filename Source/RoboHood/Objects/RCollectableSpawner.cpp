// Property Of Me

#include "RoboHood.h"
#include "RCollectableSpawner.h"

//Class Includes
#include "RCharacter.h"


// Sets default values
ARCollectableSpawner::ARCollectableSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpawnerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnerMesh"));
	SpawnerMesh->SetCollisionObjectType(ECC_WorldDynamic);
	SpawnerMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Set The RootComponent
	RootComponent = SpawnerMesh;

	SpawnerMesh->OnComponentBeginOverlap.AddDynamic(this, &ARCollectableSpawner::OnOverlapBegin);

	CollectableReference = nullptr;

}

void ARCollectableSpawner::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SpawnCollectable();
}

void ARCollectableSpawner::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARCharacter* Character = Cast<ARCharacter>(OtherActor);
	if (Character && bIsActive)
	{
		Character->AddAmmo(AmmoIndex);
		ActivateCollectable();
	}
}

void ARCollectableSpawner::ActivateCollectable()
{
	bIsActive = false;
	CollectableReference->Destroy();
	GetWorldTimerManager().SetTimer(Timehandle_Respawn, this, &ARCollectableSpawner::SpawnCollectable, SpawnRate);

}

void ARCollectableSpawner::SpawnCollectable()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Collectable = GetWorld()->SpawnActor<AActor>(AmmoIcon, SpawnInfo);
	Collectable->SetActorLocation(GetActorLocation());
	CollectableReference = Collectable;

	bIsActive = true;
}


