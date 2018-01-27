// Property Of Me

#pragma once
#include "UI/Settings/RSettingMaster.h"
#include "RSliderSettings.generated.h"

UCLASS()
class ROBOHOOD_API URSliderSettings : public URSettingMaster
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Settings")
	float MinValue;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float MaxValue;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool DisplayPercentage;

public:

	UFUNCTION(BlueprintCallable)
	void InitializeWidget(USlider* Slider, UTextBlock* CurrValue_Text);

	UFUNCTION(BlueprintCallable)
	void ApplySetting(float Value, UTextBlock* CurrValue_Text, FString& Command);

private: //Hate these functions messy

	float GetSliderValue();
	FString GetSettingValue(float Value);
	
	
};
