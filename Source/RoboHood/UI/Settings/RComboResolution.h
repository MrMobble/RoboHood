// Property Of Me

#pragma once

#include "UI/Settings/RSettingMaster.h"
#include "RComboResolution.generated.h"

/**
 * 
 */
UCLASS()
class ROBOHOOD_API URComboResolution : public URSettingMaster
{
	GENERATED_BODY()
	
private: //Variables

	TArray<FSettingOption> Options;

public:

	UFUNCTION(BlueprintCallable)
	void InitializeWidget(UComboBoxString* ComboBox, TArray<FIntPoint> Resolutions);

	UFUNCTION(BlueprintCallable)
	void ApplySetting(FText Selected_Label, FString& Command);
	
	
};
