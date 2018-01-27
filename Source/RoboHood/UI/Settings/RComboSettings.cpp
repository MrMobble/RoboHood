// Property Of Me

#include "RoboHood.h"
#include "RComboSettings.h"

void URComboSettings::InitializeWidget(UComboBoxString* ComboBox)
{
	for (auto& Elem : Options)
	{
		ComboBox->AddOption(Elem.Label.ToString());
	}

	FString ConfigValue = URSettingsManager::GetConfigValue(ConVar, Section);

	ComboBox->SetSelectedOption(URSettingsManager::GetSelectedLabel(Options, ConfigValue));
}

void URComboSettings::ApplySetting(FText Selected_Label, FString& Command)
{
	//Gets values using the selected settings label.
	FString ConfigValue = URSettingsManager::GetSelectedValue(Options, Selected_Label);

	//Saves new setting value to file.
	URSettingsManager::SetConfigValue(ConVar, ConfigValue, Section);

	//Return the console command in FString value.
	Command = URSettingsManager::CreateCommand(ConVar, ConfigValue);
}


