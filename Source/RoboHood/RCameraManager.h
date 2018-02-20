// Property Of Me

#pragma once
#include "Camera/PlayerCameraManager.h"
#include "RCameraManager.generated.h"

UCLASS()
class ROBOHOOD_API ARCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	ARCameraManager();
	
public:

	virtual void UpdateCamera(float DeltaTime) override;
	
	
};
