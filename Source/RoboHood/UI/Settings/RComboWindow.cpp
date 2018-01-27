// Property Of Me

#include "RoboHood.h"
#include "RComboWindow.h"

void URComboWindow::InitializeWidget(UComboBoxString* ComboBox)
{
	for (auto& Elem : Options)
	{
		ComboBox->AddOption(Elem.Label.ToString());
	}

	FString ConfigValue = URSettingsManager::GetConfigValue(ConVar, Section);

	ComboBox->SetSelectedOption(URSettingsManager::GetSelectedLabel(Options, URSettingsManager::GetMode(ConfigValue)));
}

void URComboWindow::ApplySetting(FText Selected_Label, FString& Command)
{
	//Gets values using the selected settings label.
	FString SelectedValue = URSettingsManager::GetSelectedValue(Options, Selected_Label);
	FString ConfigValue = URSettingsManager::GetConfigValue(ConVar, Section);
	FString CombinedValue = URSettingsManager::GetResolution(ConfigValue) + SelectedValue;

	//Saves new setting value to file.
	URSettingsManager::SetConfigValue(ConVar, CombinedValue, Section);

	//Return the console command in FString value.
	Command = URSettingsManager::CreateCommand(ConVar, CombinedValue);
}


