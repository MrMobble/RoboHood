// Property Of Me

#include "RoboHood.h"
#include "RCameraManager.h"
#include "RCharacter.h"

ARCameraManager::ARCameraManager()
{
	DefaultFOV = 90.0f;
	ViewPitchMin = -50.0f;
	ViewPitchMax = 50.0f;
	bAlwaysApplyModifiers = true;

	CameraStyle = FName("FreeCam");
	FreeCamDistance = 300.f;
	FreeCamOffset = FVector(0, 45, 75);
}

void ARCameraManager::UpdateCamera(float DeltaTime)
{
	ARCharacter* MyPawn = PCOwner ? Cast<ARCharacter>(PCOwner->GetPawn()) : NULL;

	Super::UpdateCamera(DeltaTime);

	

}
