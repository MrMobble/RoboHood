// Property Of Me

#include "RoboHood.h"
#include "RAnimNotifyState.h"

#include "RCharacter.h"

void URAnimNotifyState::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	UE_LOG(LogTemp, Warning, TEXT("BEGIN"));
	ARCharacter* Player = Cast<ARCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		Player->Jump();
		Player->bPendingJump = false;
	}
	
}

void URAnimNotifyState::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	UE_LOG(LogTemp, Warning, TEXT("END"));
}
