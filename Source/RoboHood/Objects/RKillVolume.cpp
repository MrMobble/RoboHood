// Property Of Me

#include "RoboHood.h"
#include "RKillVolume.h"

//Other Classes
#include "RCharacter.h"
#include "Net/UnrealNetwork.h"

ARKillVolume::ARKillVolume()
{
	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &ARKillVolume::OnOverlapBegin);
}

void ARKillVolume::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	Kill(OtherActor);
}

void ARKillVolume::Kill(class AActor* OtherActor)
{
	ARCharacter* Character = Cast<ARCharacter>(OtherActor);
	if (Character != NULL)
	{
		Character->TakeDamage(200.0f, FDamageEvent(), nullptr, nullptr);
	
		if (bSetPhysics)
		{
			Character->SetEnvironmentalPhysics();
		}		
	}
}

