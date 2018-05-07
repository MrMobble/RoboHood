// Property Of Me

#include "RoboHood.h"
#include "RMatAnimNotify.h"

#include "RCharacter.h"


void URMatAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	class ARCharacter* Player = Cast<ARCharacter>(MeshComp->GetOwner() ? MeshComp->GetOwner() : NULL);
	if (Player)
	{
		TArray<FString> AnimNames = Player->Animation_Names;
		for (int32 i = 0; i < AnimNames.Num(); i++)
		{
			if (AnimNames[i] == Animation->GetName())
			{
				Player->SetNewMaterial(i);
			}
		}
	}
}

