// Property Of Me

#include "RoboHood.h"
#include "RTeleporter.h"

//Class Includes
#include "RCharacter.h"
#include "RPlayerController.h"

// Sets default values
ARTeleporter::ARTeleporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TelepadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleporterMesh"));

	RootComponent = TelepadMesh;

	CollisionCube = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionCube"));
	CollisionCube->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionCube->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionCube->SetupAttachment(TelepadMesh);

	//Overlap event.
	CollisionCube->OnComponentBeginOverlap.AddDynamic(this, &ARTeleporter::OnOverlapBegin);

}

void ARTeleporter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Target->IsValidLowLevel())
	{
		ARCharacter* Character = OtherActor ? Cast<ARCharacter>(OtherActor) : NULL;
		if (Character != NULL)
		{
			//#MagicNumber
			FVector Offset = FRotationMatrix(Target->GetActorRotation()).GetScaledAxis(EAxis::X) * 200;
			//Character->TeleportTo(Target->GetActorLocation() + Offset, Target->GetActorRotation());
			Character->SetActorLocation(Target->GetActorLocation() + Offset);

			//Needed to force the controll rotation in the correct direction
			ARPlayerController* Pc = Character->Controller ? Cast<ARPlayerController>(Character->Controller) : NULL;
			if (Pc != NULL)
			{
				Pc->SetControlRotation(FRotator(Character->GetControlRotation().Pitch, Target->GetActorRotation().Yaw, Character->GetControlRotation().Roll));
			}
		}
	}
}
