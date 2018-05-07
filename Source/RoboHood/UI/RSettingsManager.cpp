// Property Of Me

#include "RoboHood.h"
#include "RSettingsManager.h"


FString URSettingsManager::IniName = FString("Settings");

FString URSettingsManager::IniFilename = FString("GraphicSettings");

//Load inifile
URSettingsManager::URSettingsManager()
{
	FConfigCacheIni::LoadGlobalIniFile(IniFilename, IniName.GetCharArray().GetData());
}

//Get config value from file
FString URSettingsManager::GetConfigValue(FName Key, FString Section)
{
	if (GetSection(Section))
	{
		FConfigValue* ConfigValue = GetSection(Section)->Find(Key);
		if (ConfigValue) { return ConfigValue->GetValue(); }
	}
	return FString();
}

//Set config value in file
void URSettingsManager::SetConfigValue(FName Key, FString Value, FString Section)
{
	if (!Key.IsNone() && !Value.IsEmpty())
	{
		GConfig->SetString(Section.GetCharArray().GetData(), Key.ToString().GetCharArray().GetData(), Value.GetCharArray().GetData(), IniFilename);
		GConfig->Flush(false, IniFilename);
	}
}

FString URSettingsManager::GetSelectedLabel(TArray<FSettingOption> Options, FString Value)
{
	for (auto& Element : Options)
	{
		if (Element.Value == Value)
			return Element.Label.ToString();	
	}
	return FString();
}

FString URSettingsManager::GetSelectedValue(TArray<FSettingOption> Options, FText Label)
{
	for (auto& Element : Options)
	{
		if (Element.Label.EqualTo(Label))
			return Element.Value;
	}
	return FString();
}

TArray<FString> URSettingsManager::GetSettings(FString Section)
{
	if (GetSection(Section))
	{
		TArray<FString> Settings;
		TArray<FName> Keys;

		GetSection(Section)->GetKeys(Keys);

		for (FName Key : Keys)
		{
			FString Value = GetConfigValue(Key, Section);
			Settings.Add(CreateCommand(Key, Value));
		}
		return Settings;
	}
	return TArray<FString>();
}

float URSettingsManager::GetAudio(FName ConVar, FString Section)
{
	if (GetSection(Section))
	{
		FString Value = GetConfigValue(ConVar, Section);
		return FCString::Atof(*Value);
	}
	return 0;
}

void URSettingsManager::ApplyDefaultSettings()
{
	SetConfigValue(FName("r.setres"), "1280x720w", "Settings");
	SetConfigValue(FName("sg.ResolutionQuality"), "100", "Settings");
	SetConfigValue(FName("sg.ViewDistanceQuality"), "1", "Settings");
	SetConfigValue(FName("sg.AntiAliasingQuality"), "1", "Settings");
	SetConfigValue(FName("sg.ShadowQuality"), "1", "Settings");
	SetConfigValue(FName("sg.TextureQuality"), "1", "Settings");
	SetConfigValue(FName("sg.EffectsQuality"), "1", "Settings");
	SetConfigValue(FName("sg.PostProcessQuality"), "1", "Settings");
	SetConfigValue(FName("t.MaxFPS"), "60", "Settings");

	SetConfigValue(FName("master"), "1", "Audio");
	SetConfigValue(FName("game"), "1", "Audio");
	SetConfigValue(FName("music"), "1", "Audio");
}

FConfigSection* URSettingsManager::GetSection(FString Section)
{
	return GConfig->GetSectionPrivate(Section.GetCharArray().GetData(), false, true, IniFilename);
}

FString URSettingsManager::CreateCommand(FName Key, FString Value)
{
	return Key.ToString() + FString(" ") + Value;
}

FString URSettingsManager::CombineResolution(FIntPoint Pixels)
{
	return FString::FromInt(Pixels.X) + FString("x") + FString::FromInt(Pixels.Y);
}

//This function takes the full resoltuion string and removes the resolution e.g 1920x1080wf -> wf
FString URSettingsManager::GetMode(FString ResolutionString)
{
	int32 Index;
	if (ResolutionString.FindChar(TCHAR('w'), Index) || ResolutionString.FindChar(TCHAR('f'), Index))
	{
		ResolutionString.RemoveAt(0, Index);
		return ResolutionString;
	}
	return FString();
}

//This function takes the full resolution string and removes the mode e.g 1920x1080wf -> 1920x1080.
FString URSettingsManager::GetResolution(FString ResolutionString)
{
	int32 Index;
	if (ResolutionString.FindChar(TCHAR('w'), Index) || ResolutionString.FindChar(TCHAR('f'), Index))
	{
		ResolutionString.RemoveAt(Index, ResolutionString.Len()-Index);
		return ResolutionString;
	}
	return FString();
}