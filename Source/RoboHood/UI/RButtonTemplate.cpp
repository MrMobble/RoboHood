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
			TextBlock->SetColorAndOpacity(Label_Normal);
		break;
	}
}
