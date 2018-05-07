// Property Of Me

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "RBPFunctionLib.generated.h"

UCLASS()
class ROBOHOOD_API URBPFunctionLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	static UAudioComponent* CreateAudioComponent(USoundBase* Sound);
	
	
};
