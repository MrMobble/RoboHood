// Property Of Me

#pragma once

#include "GameFramework/Actor.h"
#include "RCollectableSpawner.generated.h"

UCLASS()
class ROBOHOOD_API ARCollectableSpawner : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	ARCollectableSpawner();

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//////////////////////////////////////////////////////////////////////////////////////////
	// Components

	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent* Collider;

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* SpawnerMesh;

	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* SpawnLocation;

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* MeshEffect;

	UPROPERTY(VisibleDefaultsOnly)
	UParticleSystemComponent* ParticleEffect;

	//
	//////////////////////////////////////////////////////////////////////////////////////////
	// Public Variables

	UPROPERTY(EditAnywhere, Category = "Collectable Properties")
	TSubclassOf<class AActor> PickupMesh;

	UPROPERTY(EditAnywhere, Category = "Collectable Properties")
	int32 RespawnTime;

	UPROPERTY(EditAnywhere, Category = "Collectable Properties")
	int32 AmmoIndex;

	UPROPERTY(EditAnywhere, Category = "Collectable Properties")
	int32 Ammount;

	UPROPERTY(EditAnywhere, Category = "Collectable Properties")
	bool bHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Material Properties")
	UMaterialInterface* Material;

	//
	//////////////////////////////////////////////////////////////////////////////////////////
	// Other

	void SpawnCollectable();

	void ActivateCollectable();

	UMaterialInstanceDynamic* DynMaterial;

	AActor* CollectableReference;

	FTimerHandle Timehandle_Respawn;

	bool bIsActive;

	//
	//////////////////////////////////////////////////////////////////////////////////////////
	
};
