// Core.cpp: implementation of the Lexems class.

#include "types.h"
#include "Core.h"
#include "InputSystem.h"
#include "Render.h"
#include "Messager.h"
#include "LevelSet.h"
#include "GameData.h"

#ifdef WIN32
#include "FileAgregator.h"
#else
#include "../runner.h"
#endif//WIN32

Core::Objects Core::_objects;
std::vector<std::pair<Object *, float> > Core::_objectsToRemove;
std::list<std::string> Core::_messages;
Core::Objects Core::_objectsBack;
Core::ListenerList Core::_listeners;
std::string Core::_dataDir;

std::vector<char> Core::_buffer;
rapidxml::xml_document<> Core::_doc;
rapidxml::xml_node<> *Core::_element;

std::map<std::string, std::pair<std::vector<char> *, rapidxml::xml_document<> *> > Core::_iniXmls;

std::vector<std::pair<GLTexture *, std::string> > animationsPull;

pthread_t threadAnimation;
pthread_mutex_t mutexAnimation;

float Core::_fadeEffect = 0.f;
unsigned int Core::_fadeColor = 0xFFFFFF;

Core::FontsMap Core::_fonts;

std::function<Object *(rapidxml::xml_node<> *)> Core::_customCreate;

void Core::Init(const std::function<Object *(rapidxml::xml_node<> *)> &customCreate)
{
    _customCreate = customCreate;
    _fadeColor = Math::ReadColor(Core::IniGetString("game.xml", "system", "fade_color", "0xFFFFFFFF"));
    GLTexture::globalLoadingCounter = GameData::int_exist("loading_number") ? GameData::int_var("loading_number") : 6;
    Messager::SendMessage("Core", "post:load xml start.xml");
}

void Core::FinishLoading()
{
    GLTexture::globalLoadingCounter = 0;
    _buffer.clear();

	if (_objectsBack.size() > 0 && _objects.size() > 0)
    {
		_fadeEffect = 1.f;        
    }
	else
		_fadeEffect = 0.f;
}

void Core::LoadOneLineResource(rapidxml::xml_node<> *&element)
{
	if (element != NULL) 
	{
		std::string name = element->name();
		if (name == "Resources") {
			ReadDescriptions(element->first_attribute("fileName")->value());
		} else if (name == "Atlas") {
			LoadAtlas(element->first_attribute("fileName")->value());
		} else if (name == "LevelSet") {
			LevelSetManager::Load(element->first_attribute("fileName")->value());
		} else if (name == "MyAnimations") {
            rapidxml::xml_attribute<> *threadSaveXml = element->first_attribute("threadSafe");
            My::AnimationManager::Load(element->first_attribute("fileName")->value(), threadSaveXml ? strcmp(threadSaveXml->value(), "1") == 0 : false);
        } else if (name == "StopThreads") {
            LevelSetManager::EnableThread(false);
            TextureManager::EnableThread(false);
			My::AnimationManager::EnableThread(false);

            if (!GameData::int_exist("loading_number") || GameData::int_var("loading_number") < GLTexture::globalLoadingCounter)
            {
                GameData::int_create("loading_number", GLTexture::globalLoadingCounter);
            }
        } else if (name == "CopyResources") {
#ifdef WIN32
            FILE *file = fopen("copy_release_data", "r");
			if (file)
			{
                char str[100];
                fscanf(file, "%s", str);
                fclose(file);
				::Init();
                ::CopyResources(".", strcmp(str, "png") == 0);
			}
#endif//WIN32

        } else if (_customCreate) {
            _objects.push_back(_customCreate(element));
        } else {
            log::write("ERROR: unknown name %s", name.c_str());
        }
		element = element->next_sibling();
		if (element == NULL)
		{
            log::write("FinishLoading - start");
			FinishLoading();
            log::write("FinishLoading - end");
		}
	}
}

void Core::Load(const char *fileName)
{
    TextureManager::EnableThread(false);
    LevelSetManager::EnableThread(false);
	My::AnimationManager::EnableThread(false);

    log::write(fileName);
    assert(_buffer.empty());
	assert(_element == NULL);

    //std::ifstream myfile(fileName);
    Core::ReadFileToBuffer(fileName, _buffer);

    if (_buffer.size())
    {
        _buffer.push_back('\0');
        _doc.parse<0>(_buffer.data());

		rapidxml::xml_node<> *root = _doc.first_node();
		_element = root->first_node();
        LoadOneLineResource(_element);
	}
	else
	{
        log::write("Core %s - file not found", fileName);
	}
}

void Core::OnMessage(const std::string &message)
{
	_messages.push_back(message);
}

void Core::Draw(Objects &objects)
{
	Objects::iterator start = objects.begin();
	for (Objects::iterator i = objects.begin(), e = objects.end(); i != e; i++) 
	{
		if ((*i)->Fullscreen()) start = i;
	}
	for (Objects::iterator i = start, e = objects.end(); i != e; i++) {
		(*i)->Draw();
	}
}

void Core::Draw()
{
    TIME_TEST(30)
	if (_fadeEffect > 0.f)
	{
		if (_fadeEffect > 0.5f)
			Draw(_objectsBack);
		else
			Draw(_objects);
        unsigned int alpha = 0xFF * sin(M_PI * _fadeEffect);
		Render::DrawBar(0, 0, Render::ScreenWidth(), Render::ScreenHeight(), _fadeColor | (alpha << 24));
		return;
	}

	if (_objectsBack.size())
	{
		Draw(_objectsBack);
		return;
	}

	Draw(_objects);

#ifdef CHEATS
	if (InputSystem::IsScaling())
	{
		Render::Circle(InputSystem::GetScalePos().x, InputSystem::GetScalePos().y, 15, InputSystem::scaleColor);
	}
#endif
}

void Core::Update(float deltaTime)
{
#ifdef CHEATS
	if (InputSystem::IsPressed(VK_SHIFT))
	{
		deltaTime *= 0.1f;
	}
#endif
    TIME_TEST(30)
    assert(deltaTime >= 0.f);
	if (_element)
	{
		LoadOneLineResource(_element);
		return;
	}
	if (_fadeEffect > 0.f)
	{
		float old(_fadeEffect);
		_fadeEffect -= deltaTime / 0.6f;
		if (old > 0.5f && _fadeEffect <= 0.5f)
		{
			ClearBack();
		}
	}

	Messager::CoreSendMessages(deltaTime);

	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) 
	{
		(*i)->Update(deltaTime);
	}

	while (_messages.begin() != _messages.end()) 
	{
		std::string message = *_messages.begin();
		_messages.pop_front();
		if (Messager::CanCut(message, "loadGroup ", message)) {
			LoadGroup(message);
		} else if (Messager::CanCut(message, "UnloadGroup ", message)) {
			UnloadGroup(message);
		} else if (Messager::CanCut(message, "load xml ", message)) {
			InputSystem::Reset();
			MoveObjectToBack();
			Load(message.c_str());
		} else if (Messager::CanCut(message, "post:", message)) {
			Messager::SendMessage("Core", message);
		} else {
			assert(false);
		}
	}

	for (unsigned int i = 0; i < _objectsToRemove.size(); )
	{
		_objectsToRemove[i].second -= deltaTime;
		if (_objectsToRemove[i].second <= 0.f)
		{
			for (Objects::iterator k = _objects.begin(); k != _objects.end(); )
			{
				if ((*k) == _objectsToRemove[i].first)
				{
					InputSystem::ResetLock();
					delete (*k);
					k = _objects.erase(k);
					break;
				}
				else
					++k;
			}

			if (i + 1 < _objectsToRemove.size())
				_objectsToRemove[i] = _objectsToRemove.back();

			_objectsToRemove.pop_back();
		}
		else
		{
			++i;
		}
	}
}

void Core::Release()
{
	ClearBack();
	Unload();
	ReleaseAllTextures();
    for (auto &a : _iniXmls)
    {
        delete a.second.first;
        delete a.second.second;
    }
}

void Core::ClearBack()
{
	for (Objects::iterator i = _objectsBack.begin(), e = _objectsBack.end(); i != e; i++)
	{
		delete (*i);
		(*i) = NULL;
	}
	_objectsBack.clear();
}

void Core::Unload()
{
	InputSystem::ResetLock();
	for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) 
	{
		delete (*i);
		(*i) = NULL;
	}
	_objects.clear();
}

void Core::MoveObjectToBack()
{
	InputSystem::ResetLock();
	_objectsBack = _objects;
	_objects.clear();
}

#ifndef WIN32
#include <sys/time.h>

unsigned long GetTickCount()
{
    struct timeval tv;
    if( gettimeofday(&tv, NULL) != 0 )
        return 0;
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000.f);
}
#endif

CoreWindow *Core::CreateWindowOnTop(const std::string &name, const std::string &libId, bool animate, std::string processMessage)
{
	if (processMessage.size())
		ProcessEvent(processMessage);

	CoreWindow *w = new CoreWindow(name, libId, animate);
    w->SetName(name);
	PushObjectOnTop(w);
	return w;
}

Object *Core::PushObjectOnTop(Object *window)
{
	_objects.push_back(window);
	return window;
}

void Core::RemoveObject(Object *o, float delay)
{
    ProcessEvent("RemoveObject");
	_objectsToRemove.push_back(std::make_pair(o, delay));
}

void Core::RemoveObjectOnTop()
{
	assert(_objects.size());
	CoreWindow *window = dynamic_cast<CoreWindow *>(_objects.back());
	if (window) 
	{
		window->Hide();
	}
	else
	{
		RemoveObject(_objects.back());
	}
}

Object *Core::GetObjectByName(const std::string &name)
{
    for (auto o : _objects)
    {
        if (o->GetName() == name)
        {
            return o;
        }
    }
    return NULL;
}

void Core::AddListener(const std::string &event, const std::string &listener)
{
    _listeners.push_back(std::make_pair(event, listener));
}

void Core::ProcessEvent(const std::string &event)
{
    for (auto i : _listeners)
    {
        if (i.first == event)
        {
            Messager::SendMessage(i.second, event);
        }
    }
}

bool Core::ReadFileToBuffer(const char *filename, std::vector<char> &buffer)
{
    std::string path(GetDataDir());
    path.append(filename);
    FILE *file = fopen(path.c_str(), "rb");
    if (!file)
    {
        return false;
    }

    buffer.clear();
    size_t size = 0;
    const size_t readingStep = 100;
    while (!feof(file))
    {
        buffer.resize(size + readingStep);
        size_t r = fread(&(buffer[size]), 1, readingStep, file);
        buffer.resize(size + r);
        size += r;
    }
    if (buffer.size())
    {
        buffer.push_back(0);
    }
    fclose(file);
    return true;
}

std::shared_ptr<CoreFont> Core::CreateFont(const char *fileName)
{
    FontsMap::iterator f = _fonts.find(fileName);
    if (f == _fonts.end() || (*f).second.lock() == nullptr)
    {
        std::shared_ptr<CoreFont> font(new CoreFont(fileName));
        _fonts[fileName] = font;
        return font;
    }
    return (*f).second.lock();
}

rapidxml::xml_node<> *Core::LoadIniXml(const char *fileName)
{
    auto f = _iniXmls.find(fileName);
    if (f != _iniXmls.end())
    {
        return (*f).second.second->first_node();
    }

    // Load ini xml
    std::vector<char> *buffer = new std::vector<char>();
    Core::ReadFileToBuffer(fileName, *buffer);

    if (buffer->size() == 0)
    {
        log::write("Error! Ini file not found! %s", fileName);
        return nullptr;
    }
    rapidxml::xml_document<> *doc = new rapidxml::xml_document<>();
    try
    {
        doc->parse<0>(buffer->data());
    }
    catch (...)
    {
        return nullptr;
    }

    _iniXmls[fileName] = std::make_pair(buffer, doc);

    return doc->first_node();
}

int Core::IniGetInt(const char *filename, const char *section, const char *param, int  defaultValue /* = 0 */)
{
    rapidxml::xml_node<> *root = LoadIniXml(filename);
    if (root)
    {
        rapidxml::xml_node<> *sectionNode = root->first_node(section);
        if (sectionNode)
        {
            rapidxml::xml_attribute<> *paramAttr = sectionNode->first_attribute(param);
            if (paramAttr)
            {
                return atoi(paramAttr->value());
            }
        }
    }
    return defaultValue;
}

float Core::IniGetFloat(const char *filename, const char *section, const char *param, float defaultValue /* = 1.f */)
{
    rapidxml::xml_node<> *root = LoadIniXml(filename);
    if (root)
    {
        rapidxml::xml_node<> *sectionNode = root->first_node(section);
        if (sectionNode)
        {
            rapidxml::xml_attribute<> *paramAttr = sectionNode->first_attribute(param);
            if (paramAttr)
            {
                return (float)atof(paramAttr->value());
            }
        }
    }
    return defaultValue;
}

std::string Core::IniGetString(const char *filename, const char *section, const char *param, const char *defaultValue /* = "" */)
{
    rapidxml::xml_node<> *root = LoadIniXml(filename);
    if (root)
    {
        rapidxml::xml_node<> *sectionNode = root->first_node(section);
        if (sectionNode)
        {
            rapidxml::xml_attribute<> *paramAttr = sectionNode->first_attribute(param);
            if (paramAttr)
            {
                return paramAttr->value();
            }
        }
    }
    return defaultValue;
}
