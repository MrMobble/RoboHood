// Property Of Me

#pragma once
#include "UI/Settings/RSettingMaster.h"
#include "RComboWindow.generated.h"

UCLASS()
class ROBOHOOD_API URComboWindow : public URSettingMaster
{
	GENERATED_BODY()
	
public: //Variables

	UPROPERTY(EditAnywhere, Category = "Settings")
	TArray<FSettingOption> Options;

public:

	UFUNCTION(BlueprintCallable)
	void InitializeWidget(UComboBoxString* ComboBox);

	UFUNCTION(BlueprintCallable)
	void ApplySetting(FText Selected_Label, FString& Command);
	
	
};
