// Property Of Me

#pragma once
#include "Blueprint/UserWidget.h"
#include "UI/RSettingsManager.h" //Custom Function Library
#include "RSettingMaster.generated.h"

UCLASS()
class ROBOHOOD_API URSettingMaster : public UUserWidget
{
	GENERATED_BODY()
	
public: //Variables

	UPROPERTY(EditAnywhere, Category = "Settings")
	FName ConVar;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FString Section;

	UPROPERTY(EditAnywhere, Category = "Visual Settings")
	FText Setting_Text;

	UPROPERTY(EditAnywhere, Category = "Visual Settings")
	FLinearColor Background_Colour;

	UFUNCTION(BlueprintCallable)
	void InitializeVisuals(UTextBlock* Label, UImage* Background);

	//UFUNCTION(BlueprintCallable)
	//virtual void InitializeWidget(UComboBoxString* ComboBox){}

	//UFUNCTION(BlueprintCallable)
	//virtual void ApplySetting(FText Selected_Label, FString& Command){}

public: //Sync Function

	UFUNCTION(BlueprintNativeEvent, Category = "User Interface")
	void OnSynchronizeProperties();
	void OnSynchronizeProperties_Implementation() {}

	virtual void SynchronizeProperties() override;
	
	
};
