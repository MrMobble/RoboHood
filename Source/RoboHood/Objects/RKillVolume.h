// Property Of Me

#pragma once

#include "Engine/TriggerVolume.h"
#include "RKillVolume.generated.h"

UCLASS()
class ROBOHOOD_API ARKillVolume : public ATriggerVolume
{
	GENERATED_BODY()
	
public:

	// constructor sets default values for this actor's properties
	ARKillVolume();

	// overlap begin function
	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UPROPERTY(EditAnywhere)
	bool bSetPhysics;

	void Kill(class AActor* OtherActor);

	//UFUNCTION(Reliable, Server, WithValidation)
	//void ServerKill(class AActor* OtherActor);
	//void ServerKill_Implementation(class AActor* OtherActor);
	//bool ServerKill_Validate(class AActor* OtherActor) { return true; }
	
	
	
};
