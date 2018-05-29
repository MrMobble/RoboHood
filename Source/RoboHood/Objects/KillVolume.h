// Property Of Me

#pragma once

#include "GameFramework/Actor.h"
#include "KillVolume.generated.h"

UCLASS()
class ROBOHOOD_API AKillVolume : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
	float VolumeX;

	UPROPERTY(EditAnywhere)
	float VolumeY;

	AKillVolume();

	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent* Volume;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Allow Actors To Initialize Themselves.
	virtual void PostInitializeComponents();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	
	
};
