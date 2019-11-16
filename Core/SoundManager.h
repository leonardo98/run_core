#include "fmod.hpp"

#include <string>
#include <map>

class SoundManager
{
public:

    static void PlaySound(const char *name);
    static void PlayMusic(const char *name);
    static void Create();
    static void Release();
    static void Update();

private:

    static SoundManager *_instance;

    FMOD::System *_system;
    FMOD::Channel *_soundChannel;
    FMOD::Channel *_musicChannel;
    void *_extradriverdata;
    typedef std::map<std::string, FMOD::Sound *> SoundMap;
    SoundMap _soundMap;
    SoundMap _musicMap;

    void LoadSounds();
    void LoadSound(const char *alias, const char *filename);
    void LoadMusic(const char *alias, const char *filename);
    void PlaySoundInt(const char *name);
    void PlayMusicInt(const char *name);
    void UpdateInt();

    SoundManager();
    virtual ~SoundManager();
};
