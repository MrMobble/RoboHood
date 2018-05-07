// Property Of Me

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "RMatAnimNotify.generated.h"

UCLASS()
class ROBOHOOD_API URMatAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
	
	
	
	
};
