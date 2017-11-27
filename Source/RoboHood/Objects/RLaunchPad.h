// Property Of Me

#pragma once

#include "GameFramework/Actor.h"
#include "RLaunchPad.generated.h"

UCLASS()
class ROBOHOOD_API ARLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	ARLaunchPad();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleDefaultsOnly, Category = "PadMesh")
	UStaticMeshComponent* PadMesh;

	UPROPERTY(EditDefaultsOnly, Category = "LuanchPad Properties")
	FVector LaunchPower;
	
};
