// Property Of Me

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "RAnimNotifyState.generated.h"

UCLASS()
class ROBOHOOD_API URAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:

	void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;
	
	
};
