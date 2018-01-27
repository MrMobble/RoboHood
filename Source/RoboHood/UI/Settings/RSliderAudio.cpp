// Property Of Me

#include "RoboHood.h"
#include "RSliderAudio.h"

void URSliderAudio::InitializeWidget(USlider* Slider, UTextBlock* CurrValue_Text)
{
	FString ConfigValue = URSettingsManager::GetConfigValue(ConVar, Section);
	float RawValue = FCString::Atof(*ConfigValue);

	Slider->SetValue(RawValue);

	CurrValue_Text->SetText(FText::FromString(FString::FromInt((int32)(RawValue * 100))));
}

void URSliderAudio::ApplySetting(float Value, UTextBlock* CurrValue_Text, float& Volume_Value, ESoundSetting& SoundChannel)
{
	CurrValue_Text->SetText(FText::FromString(FString::FromInt((int32)(Value * 100))));

	URSettingsManager::SetConfigValue(ConVar, FString::SanitizeFloat(Value), Section);

	Volume_Value = Value;
	SoundChannel = Sound_Channel;
}


