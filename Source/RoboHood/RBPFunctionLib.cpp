// Property Of Me

#include "RoboHood.h"
#include "RBPFunctionLib.h"

#include "Public/AudioDevice.h"

UAudioComponent* URBPFunctionLib::CreateAudioComponent(USoundBase* Sound)
{


	UAudioComponent* WorldMusicComponent = FAudioDevice::CreateComponent(Sound, nullptr, nullptr, false, false);

	if (WorldMusicComponent)
	{
		WorldMusicComponent->bAllowSpatialization = false;
		WorldMusicComponent->bIsUISound = true;
		WorldMusicComponent->bIgnoreForFlushing = true;
		//WorldMusicComponent->Play();
	}

	return WorldMusicComponent ? WorldMusicComponent : NULL;
}


