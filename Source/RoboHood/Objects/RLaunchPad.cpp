// Property Of Me

#include "RoboHood.h"
#include "RLaunchPad.h"

//Class Includes
#include "RCharacter.h"


// Sets default values
ARLaunchPad::ARLaunchPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnerMesh"));
	PadMesh->SetCollisionObjectType(ECC_WorldDynamic);
	PadMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	RootComponent = PadMesh;

	PadMesh->OnComponentBeginOverlap.AddDynamic(this, &ARLaunchPad::OnOverlapBegin);

}

void ARLaunchPad::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARCharacter* Character = Cast<ARCharacter>(OtherActor);
	if (Character)
	{
		Character->LaunchCharacter(LaunchPower, false, true);
	}
}

