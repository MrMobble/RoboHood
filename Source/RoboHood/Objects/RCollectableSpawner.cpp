// Property Of Me

#include "RoboHood.h"
#include "RCollectableSpawner.h"

//Class Includes
#include "RCharacter.h"


// Sets default values
ARCollectableSpawner::ARCollectableSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpCollider"));
	Collider->SetCollisionObjectType(ECC_WorldStatic);
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	RootComponent = Collider;

	SpawnerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnerMesh"));
	SpawnerMesh->SetCollisionObjectType(ECC_WorldStatic);
	SpawnerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnerMesh->SetupAttachment(RootComponent);

	SpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation"));
	SpawnLocation->SetupAttachment(RootComponent);

	MeshEffect = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshEffect"));
	MeshEffect->SetCollisionObjectType(ECC_WorldStatic);
	MeshEffect->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshEffect->SetupAttachment(RootComponent);

	ParticleEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleEffect"));
	ParticleEffect->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	Collider->OnComponentBeginOverlap.AddDynamic(this, &ARCollectableSpawner::OnOverlapBegin);

	CollectableReference = nullptr;
}

void ARCollectableSpawner::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Material != NULL)
	{
		DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
		SpawnerMesh->SetMaterial(1, DynMaterial);
	}

	SpawnCollectable();
}

void ARCollectableSpawner::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (!bIsActive) return;

	ARCharacter* Character = Cast<ARCharacter>(OtherActor);
	if (Character != NULL)
	{
		if (bHealth) Character->AddHealth(Ammount);
		else Character->AddAmmo(AmmoIndex, Ammount);
		ActivateCollectable();
	}
}

void ARCollectableSpawner::ActivateCollectable()
{
	bIsActive = false;

	//MeshEffect->SetVisibility(false);
	ParticleEffect->SetVisibility(false);

	CollectableReference->Destroy();
	GetWorldTimerManager().SetTimer(Timehandle_Respawn, this, &ARCollectableSpawner::SpawnCollectable, RespawnTime);

}

void ARCollectableSpawner::SpawnCollectable()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (PickupMesh != NULL)
	{
		AActor* Collectable = GetWorld()->SpawnActor<AActor>(PickupMesh, SpawnInfo);
		Collectable->SetActorLocation(SpawnLocation->GetComponentLocation());
		CollectableReference = Collectable;

		bIsActive = true;

		//MeshEffect->SetVisibility(true);
		ParticleEffect->SetVisibility(true);
	}

}

void ARCollectableSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActive)
	{
		if (Material != NULL)
		{
			float TimeRemaining = (GetWorldTimerManager().GetTimerRemaining(Timehandle_Respawn) / RespawnTime);

			DynMaterial->SetScalarParameterValue("Progress", TimeRemaining);
		}
	}

}

