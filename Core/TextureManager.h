// TextureManager.h: interface for the TextureManager class.

#ifndef MYENGINE_TEXTUREMANAGER_H
#define MYENGINE_TEXTUREMANAGER_H

#include "types.h"
#include "rapidxml/rapidxml.hpp"
#include "Render.h"

#include <pthread.h>
#include <map>

class TextureManager  
{
public:
	struct TextureState
	{
		std::string path;
		std::string group;
        GLTexture *texture;
		bool atlasPart;
	};
	typedef std::map<std::string, TextureState> TextureMap;
    static GLTexture *getTexture(std::string textureId);
	static void unloadTexture(const std::string &textureId);
	static bool isTexture(const std::string &textureId);
	static bool isAtlasPart(const std::string &textureId);
	static void LoadGroup(std::string groupId);
	static void UnloadGroup(std::string groupId);
	static void ReleaseAllTextures();
    static void CreateSprite(GLTexture *tex, rapidxml::xml_node<> *xe);
	static void ReadDescriptions(std::string fileName);

    static bool EnableThreads() { return enableThreads; }
    static void EnableThread(bool v);
    
    static void LoadAtlas(const std::string &fileName);
    static void LoadAtlasDescription(GLTexture *tex, const std::string &fileName);
    
private:
	static TextureMap _texturesMap;
    static bool enableThreads;
    static void FinishThread();
};

#endif//MYENGINE_TEXTUREMANAGER_H
