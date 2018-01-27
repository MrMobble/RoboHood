// Property Of Me

#include "RoboHood.h"
#include "RSettingMaster.h"

void URSettingMaster::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	OnSynchronizeProperties();
}

void URSettingMaster::InitializeVisuals(UTextBlock* Label, UImage* Background)
{
	Label->SetText(Setting_Text);
	Background->SetColorAndOpacity(Background_Colour);
}
