#ifndef SOUND_H
#define SOUND_H

class Sound
{
public:
	Sound() {}
    void Play();
    void Stop();
};


Sound *SoundEffect(const char *id);
void SoundEffectRnd(const char *id, int var, int base = 1);
void SoundEffectVar(const char *id);
void SoundStop(Sound *sound);

void musicPlay(const char *id);

#endif//SOUND_H
