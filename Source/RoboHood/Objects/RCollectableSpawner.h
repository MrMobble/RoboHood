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

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleDefaultsOnly, Category = "SpawnerMesh")
	UStaticMeshComponent* SpawnerMesh;


	UPROPERTY(EditDefaultsOnly, Category = "Collectable Properties")
	TSubclassOf<class AActor> AmmoIcon;

	AActor* CollectableReference;


	UPROPERTY(EditDefaultsOnly, Category = "Collectable Properties")
	int32 SpawnRate;

	UPROPERTY(EditDefaultsOnly, Category = "Collectable Properties")
	int32 AmmoIndex;

	void SpawnCollectable();

	void ActivateCollectable();

	//Handles TimeBetween Shots.
	FTimerHandle Timehandle_Respawn;

	bool bIsActive;
	
};
