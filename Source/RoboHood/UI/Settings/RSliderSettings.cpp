// Property Of Me

#include "RoboHood.h"
#include "RSliderSettings.h"

void URSliderSettings::InitializeWidget(USlider* Slider, UTextBlock* CurrValue_Text)
{
	float Value = GetSliderValue();

	Slider->SetValue(Value);

	if (DisplayPercentage) { CurrValue_Text->SetText(FText::FromString(GetSettingValue(Value) + FString("%"))); }
	else { CurrValue_Text->SetText(FText::FromString(GetSettingValue(Value))); }
}

void URSliderSettings::ApplySetting(float Value, UTextBlock* CurrValue_Text, FString& Command)
{
	FString StringValue = GetSettingValue(Value);

	if (DisplayPercentage) { CurrValue_Text->SetText(FText::FromString(StringValue + FString("%"))); }
	else { CurrValue_Text->SetText(FText::FromString(StringValue)); }

	URSettingsManager::SetConfigValue(ConVar, StringValue, Section);

	Command = URSettingsManager::CreateCommand(ConVar, StringValue);
}







float URSliderSettings::GetSliderValue()
{
	FString ConfigValue = URSettingsManager::GetConfigValue(ConVar, Section);
	float RawValue = FCString::Atof(*ConfigValue);
	float NormalizedValue = FMath::GetMappedRangeValueUnclamped(FVector2D(MinValue, MaxValue), FVector2D(0.0f, 1.0f), RawValue);

	return NormalizedValue;
}

FString URSliderSettings::GetSettingValue(float Value)
{
	float NormalizedValue = FMath::GetMappedRangeValueUnclamped(FVector2D(0.0f, 1.0f), FVector2D(MinValue, MaxValue), Value);
	int32 IntValue = (int32)NormalizedValue;
	FString StringValue = FString::FromInt(IntValue);

	return StringValue;
}
