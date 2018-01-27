// Property Of Me

#pragma once
#include "UI/Settings/RSettingMaster.h"
#include "RSliderAudio.generated.h"

UCLASS()
class ROBOHOOD_API URSliderAudio : public URSettingMaster
{
	GENERATED_BODY()	

public:

	UPROPERTY(EditAnywhere, Category = "Settings")
	ESoundSetting Sound_Channel;

public:
	
	UFUNCTION(BlueprintCallable)
	void InitializeWidget(USlider* Slider, UTextBlock* CurrValue_Text);

	UFUNCTION(BlueprintCallable)
	void ApplySetting(float Value, UTextBlock* CurrValue_Text, float& Volume_Value, ESoundSetting& SoundChannel);
	
};
