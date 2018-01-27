// Property Of Me

#pragma once

#include "Blueprint/UserWidget.h"
#include "RButtonTemplate.generated.h"

UENUM(BlueprintType)
enum class ETextState : uint8
{
	VE_Hover 	UMETA(DisplayName = "Hover"),
	VE_UnHover 	UMETA(DisplayName = "UnHover"),
};

UCLASS()
class ROBOHOOD_API URButtonTemplate : public UUserWidget
{
	GENERATED_BODY()

public:

	//Label Settings

	UPROPERTY(EditAnywhere, Category = "Label Settings")
	FText Label_Text;

	UPROPERTY(EditAnywhere, Category = "Label Settings")
	FSlateFontInfo Label_Font;

	UPROPERTY(EditAnywhere, Category = "Label Settings")
	FSlateColor Label_Normal;

	UPROPERTY(EditAnywhere, Category = "Label Settings")
	FSlateColor Label_Hover;

	//UPROPERTY(EditAnywhere, Category = "Label Settings")
	//FSlateColor Label_Pressed;

	//Button Settings

	UPROPERTY(EditAnywhere, Category = "Button Settings")
	FButtonStyle Button_Style;

	//Functions

	UFUNCTION(BlueprintCallable)
	void IniButton(UTextBlock* TextBlock, UButton* Button);

	UFUNCTION(BlueprintCallable)
	void ChangeState(UTextBlock* TextBlock, ETextState CurrentState);

	
public:

	//Sync Function

	UFUNCTION(BlueprintNativeEvent, Category = "User Interface")
	void OnSynchronizeProperties();
	void OnSynchronizeProperties_Implementation() {}

	virtual void SynchronizeProperties() override;
	
	
};
