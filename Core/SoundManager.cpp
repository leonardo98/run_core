#include "SoundManager.h"

#include "Core.h"

#include <assert.h>

SoundManager *SoundManager::_instance = nullptr;
SoundManager *Instance();

#define ERRCHECK(_result) assert(_result == FMOD_OK);//ERRCHECK_fn(_result, __FILE__, __LINE__)

SoundManager::SoundManager()
    : _system(nullptr)
    , _soundChannel(nullptr)
    , _musicChannel(nullptr)
    , _extradriverdata(nullptr)
	, _soundMap()

{
    assert(!_instance);

    _instance = this;

    FMOD_RESULT result;

    result = FMOD::System_Create(&_system);
    ERRCHECK(result);

    unsigned int version;
    result = _system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        log::write("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
    }

    result = _system->init(32, FMOD_INIT_NORMAL, _extradriverdata);
    ERRCHECK(result);
	
    LoadSounds();
}

SoundManager::~SoundManager()
{
    FMOD_RESULT result;
	for (auto &s : _soundMap)
	{
		result = s.second->release();
	    ERRCHECK(result);
	}

    result = _system->close();
    ERRCHECK(result);
    result = _system->release();
    ERRCHECK(result);

    assert(_instance == this);
    _instance = nullptr;
}

void SoundManager::PlaySound(const char *name)
{
    assert(_instance);
    if (_instance)
    {
        _instance->PlaySoundInt(name);
    }
}

void SoundManager::PlayMusic(const char *name)
{
    assert(_instance);
    if (_instance)
    {
        _instance->PlayMusicInt(name);
    }
}

void SoundManager::PlaySoundInt(const char *name)
{
	SoundMap::iterator f = _soundMap.find(name);
	if (f != _soundMap.end())
	{
	    FMOD_RESULT result;
        result = _system->playSound((*f).second, 0, false, &_soundChannel);
	    ERRCHECK(result);
	}
	else
	{
        log::write("sample not found %s", name);
	}	
}

void SoundManager::PlayMusicInt(const char *name)
{
    SoundMap::iterator f = _musicMap.find(name);
    if (f != _musicMap.end())
    {
        FMOD_RESULT result;

        if (_musicChannel)
        {
            bool isPlaying;
            result = _musicChannel->isPlaying(&isPlaying);
            ERRCHECK(result);

            if (isPlaying)
            {
                FMOD::Sound *sound;
                result = _musicChannel->getCurrentSound(&sound);
                ERRCHECK(result);
                if (sound == (*f).second)
                {
                    return;// already playing it - do nothing
                }

                result = _musicChannel->stop();
                ERRCHECK(result);
            }
        }

        result = _system->playSound((*f).second, 0, false, &_musicChannel);
        ERRCHECK(result);
    }
    else
    {
        log::write("music not found %s", name);
    }
}

void SoundManager::LoadSounds()
{
    // Load ini xml
    std::vector<char> buffer;
    Core::ReadFileToBuffer("music.xml", buffer);

    if (buffer.size() == 0)
    {
        log::write("Error! File not found: sounds.xml");
        return;
    }
    rapidxml::xml_document<> doc;
    try
    {
        doc.parse<0>(buffer.data());
    }
    catch (const std::runtime_error& e)
    {
        log::write("Error! Parse failed: sounds.xml - Runtime error was: ", e.what());
    }
    catch (const rapidxml::parse_error& e)
    {
        log::write("Error! Parse failed: sounds.xml - Parse error was: %s", e.what());
    }
    catch (const std::exception& e)
    {
        log::write("Error! Parse failed: sounds.xml - Error was: %s", e.what());
    }
    catch (...)
    {
        log::write("Error! Parse failed: sounds.xml - An unknown error occurred.");
    }

    rapidxml::xml_node<> *root = doc.first_node();
    if (root)
    {
        rapidxml::xml_node<> *music = root->first_node("Music");
        for(rapidxml::xml_node<> *sampleNode = music->first_node("sample"); sampleNode; sampleNode = sampleNode->next_sibling())
        {
            rapidxml::xml_attribute<> *pathAttribute = sampleNode->first_attribute("music");
            LoadMusic(sampleNode->first_attribute("name")->value(), pathAttribute->value());
        }

        rapidxml::xml_node<> *samples = root->first_node("Samples");
        for(rapidxml::xml_node<> *sampleNode = samples->first_node("sample"); sampleNode; sampleNode = sampleNode->next_sibling())
        {
            rapidxml::xml_attribute<> *pathAttribute = sampleNode->first_attribute("sound");
            rapidxml::xml_attribute<> *nameAttribute = sampleNode->first_attribute("name");
            LoadSound(nameAttribute->value(), pathAttribute->value());
        }
    }

}

void SoundManager::LoadSound(const char *alias, const char *filename)
{
    FMOD_RESULT result;
    FMOD::Sound *sound;
    std::string path(Core::GetDataDir());
    path.append(filename);
    result = _system->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound);//Common_MediaPath("drumloop.wav")
    ERRCHECK(result);

    result = sound->setMode(FMOD_LOOP_OFF);
    ERRCHECK(result);

    _soundMap[alias] = sound;
}

void SoundManager::LoadMusic(const char *alias, const char *filename)
{
    FMOD_RESULT result;
    FMOD::Sound *sound;
    std::string path(Core::GetDataDir());
    path.append(filename);
    result = _system->createStream(path.c_str(), FMOD_LOOP_NORMAL | FMOD_2D, 0, &sound);
    ERRCHECK(result);

    result = sound->setMode(FMOD_LOOP_NORMAL);
    ERRCHECK(result);

    _musicMap[alias] = sound;
}

void SoundManager::Update()
{
    if (!_instance)
    {
        return;
    }
    _instance->UpdateInt();
}

void SoundManager::UpdateInt()
{
    assert(_instance);
    if (!_instance)
    {
        return;
    }

    FMOD_RESULT result;
    result = _system->update();
    ERRCHECK(result);
}

void SoundManager::Create()
{
    assert(!_instance);
    new SoundManager();
}

void SoundManager::Release()
{
    assert(_instance);
    SoundManager *instance = _instance;
    delete instance;
}
