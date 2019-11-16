// Core.h: interface for the Lexems class.

#ifndef MYENGINE_CORE_H
#define MYENGINE_CORE_H

#include "types.h"
#include "TextureManager.h"
#include "Object.h"
#include "../2dga_api/Animation.h"
#include "CoreWindow.h"
#include "CoreFont.h"

#include <memory>
#include <functional>

class Core
	: public TextureManager
{
public:
	static bool IsLoading() { return _element != NULL; }
    static void Init(const std::function<Object *(rapidxml::xml_node<> *)> &customCreate);
	static void Load(const char *fileName);
	static void Unload();
	static void Draw();
	static void Update(float deltaTime);
	static void Release();
	static My::Animation *getAnimation(const std::string &animationId);
	//static void DrawBar(float x, float y, float width, float height, unsigned int color);// Debug only
	static void OnMessage(const std::string &message);
	
	typedef std::list<Object *> Objects;
	typedef std::map<std::string, My::Animation *> AnimationMap;
    
    typedef std::vector<std::pair<std::string, std::string> > ListenerList;
    
	static CoreWindow *CreateWindowOnTop(const std::string &name, const std::string &libId = "", bool animate = false, std::string processMessage = "CreateWindowOnTop");
	static Object *PushObjectOnTop(Object *object);
	static void RemoveObject(Object *, float t = 0.f);
	static void RemoveObjectOnTop();
    static Object *GetObjectByName(const std::string &name);
    static Object *OnTop() {  return (_objects.size() ? _objects.back() : NULL); }
    static void AddListener(const std::string &event, const std::string &listener);
    static void RemoveListener(const std::string &event, const std::string &listener);
    static int IniGetInt(const char *filename, const char *section, const char *param, int  defaultValue = 0);
    static float IniGetFloat(const char *filename, const char *section, const char *param, float defaultValue = 1.f);
    static std::string IniGetString(const char *filename, const char *section, const char *param, const char *defaultValue = "");
    static bool ReadFileToBuffer(const char *filename, std::vector<char> &buffer);
    static std::shared_ptr<CoreFont> CreateFont(const char *filename);
    static std::string GetLocalizedString(const char *textId) { return textId; }
    static void SetDataDir(const std::string &dataDir) { _dataDir = dataDir; }
    static std::string GetDataDir() { return _dataDir; }

protected:
    static void ProcessEvent(const std::string &event);
	static void LoadOneLineResource(rapidxml::xml_node<> *&element);
	static void FinishLoading();
	static void Draw(Objects &);
    
    static std::vector<char> _buffer;
	static rapidxml::xml_node<> *_element;
    static rapidxml::xml_document<> _doc;
    static std::string _dataDir;
    static std::map<std::string, std::pair<std::vector<char> *, rapidxml::xml_document<> *> > _iniXmls;
    static rapidxml::xml_node<> *LoadIniXml(const char *fileName);

	static std::vector<std::pair<Object *, float> > _objectsToRemove;
	static Objects _objects;
	static Objects _objectsBack;
    static ListenerList _listeners;
	static void MoveObjectToBack();
	static void ClearBack();
	static std::list<std::string> _messages;
	static float _fadeEffect;
    static unsigned int _fadeColor;
    typedef std::map<std::string, std::weak_ptr<CoreFont> > FontsMap;
    static FontsMap _fonts;
    static std::function<Object *(rapidxml::xml_node<> *)> _customCreate;
};

#endif//MYENGINE_CORE_H
