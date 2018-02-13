// Property Of Me

#include "RoboHood.h"
#include "RButtonTemplate.h"

void URButtonTemplate::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	OnSynchronizeProperties();
}

void URButtonTemplate::IniButton(UTextBlock* TextBlock, UButton* Button)
{
	isActive = false;

	//Set Text
	TextBlock->SetText(Label_Text);
	TextBlock->SetFont(Label_Font);
	TextBlock->SetColorAndOpacity(Label_Normal);

	//Set Button
	Button->SetStyle(Button_Style);
}

void URButtonTemplate::ChangeState(UTextBlock* TextBlock, ETextState CurrentState)
{
	switch (CurrentState)
	{
		case ETextState::VE_Hover:
			TextBlock->SetColorAndOpacity(Label_Hover);
		break;

		case ETextState::VE_UnHover:
			if (!isActive) { TextBlock->SetColorAndOpacity(Label_Normal); }
		break;
	}
}

void URButtonTemplate::SetSelected(UTextBlock* TextBlock, UButton* Button, bool InBool)
{
	isActive = InBool;

	if (isActive)
	{
		Button->SetStyle(ActiveButton_Style);
		TextBlock->SetColorAndOpacity(Label_Hover);
	}
	else
	{
		Button->SetStyle(Button_Style);
		TextBlock->SetColorAndOpacity(Label_Normal);
	}
}