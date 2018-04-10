// Property Of Me

#pragma once

#include "GameFramework/Actor.h"
#include "RTeleporter.generated.h"

UCLASS()
class ROBOHOOD_API ARTeleporter : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	ARTeleporter();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleDefaultsOnly, Category = "PadMesh")
	UBoxComponent* CollisionCube;

	UPROPERTY(VisibleDefaultsOnly, Category = "PadMesh")
	UStaticMeshComponent* TelepadMesh;

	UPROPERTY(EditAnywhere, Category = "Teleporter Properties")
	ARTeleporter* Target;

	
	
};
