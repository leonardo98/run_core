#include "Sound.h"

#include "SoundManager.h"

#include <string>
#include <assert.h>

Sound *SoundEffect(const char *id)
{
    SoundManager::PlaySound(id);
    return nullptr;//SoundEffect(id); todo fix it
}

void SoundEffectRnd(const char *id, int var, int base)
{
	std::string soundName(id);
	int r = rand() % var + base;
	static char digits[11] = "0123456789";
	soundName += digits[r];
	SoundEffect(soundName.c_str());
}

void SoundEffectVar(const char *id)
{
    //assert(false);// todo fix it
//	SoundEffect(id, SNDTYPE_SOUND, Math::random(0.5f, 1.f), 0.f, 0.7f + static_cast<float>(rand() % 5) / 4.f * 0.6f);
}

void SoundStop(Sound *sound)
{

}

void musicPlay(const char *id)
{
    SoundManager::PlayMusic(id);
}
