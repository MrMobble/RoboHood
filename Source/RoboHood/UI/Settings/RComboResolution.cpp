// Property Of Me

#include "RoboHood.h"
#include "RComboResolution.h"

void URComboResolution::InitializeWidget(UComboBoxString* ComboBox, TArray<FIntPoint> Resolutions)
{

	for (auto& Elem : Resolutions)
	{
		FSettingOption Option;
		FString Resolution = URSettingsManager::CombineResolution(Elem);
		Option.Label = FText::FromString(Resolution);
		Option.Value = Resolution;
		Options.Add(Option);
	}

	for (auto& Elem : Options)
	{
		ComboBox->AddOption(Elem.Label.ToString());
	}

	FString ConfigValue = URSettingsManager::GetConfigValue(ConVar, Section);

	ComboBox->SetSelectedOption(URSettingsManager::GetSelectedLabel(Options, URSettingsManager::GetResolution(ConfigValue)));
}

void URComboResolution::ApplySetting(FText Selected_Label, FString& Command)
{
	//Gets values using the selected settings label.
	FString SelectedValue = URSettingsManager::GetSelectedValue(Options, Selected_Label);
	FString ConfigValue = URSettingsManager::GetConfigValue(ConVar, Section);
	FString CombinedValue = SelectedValue + URSettingsManager::GetMode(ConfigValue);

	//Saves new setting value to file.
	URSettingsManager::SetConfigValue(ConVar, CombinedValue, Section);

	//Return the console command in FString value.
	Command = URSettingsManager::CreateCommand(ConVar, CombinedValue);
}


