// Property Of Me

#pragma once

#include "UObject/NoExportTypes.h"
#include "RSettingsManager.generated.h"

USTRUCT(Blueprintable)
struct FSettingOption
{
	GENERATED_USTRUCT_BODY()

	// Display label
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
	FText Label;

	// Internal value that the console variable will be set to - If blank will use the array index of the element
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
	FString Value;

	FSettingOption()
	{
		Label = FText::FromString("Label");
		Value = FString("Value");
	}
};

UENUM(BlueprintType)
enum class ESoundSetting : uint8
{
	VE_Master 	UMETA(DisplayName = "Master"),
	VE_Game 	UMETA(DisplayName = "Game"),
	VE_Music	UMETA(DisplayName = "Music")
};

UCLASS()
class ROBOHOOD_API URSettingsManager : public UObject
{
	GENERATED_BODY()

public:

	URSettingsManager();
	
private: //File Variables

	static FString IniName;
	static FString IniFilename;

public: //Main Functions

	static FString GetConfigValue(FName Key, FString Section);

	static void SetConfigValue(FName Key, FString Value, FString Section);

	static FString GetSelectedLabel(TArray<FSettingOption> Options, FString Value);
	static FString GetSelectedValue(TArray<FSettingOption> Options, FText Label);

	static TArray<FString> GetSettings(FString Section);

	static float GetAudio(FName ConVar, FString Section);

public: //Helper Functions

	static FConfigSection* GetSection(FString Section);

	static FString CreateCommand(FName Key, FString Value);

public: //Resoltuion String Utils

	static FString CombineResolution(FIntPoint Pixels);

	static FString GetMode(FString ResolutionString);

	static FString GetResolution(FString ResolutionString);


	
	
};
