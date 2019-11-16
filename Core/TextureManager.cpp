// TextureManager.cpp: implementation of the TextureManager class.

#include "TextureManager.h"
#include "Messager.h"
#include "Math.h"
#include "Core.h"
#include "../2dga_api/Animation.h"
#include <utility>

bool TextureManager::enableThreads = false;
TextureManager::TextureMap TextureManager::_texturesMap;

std::vector<std::pair<GLTexture *, std::string> > atlasPull;
pthread_t threadAtlas;
pthread_mutex_t mutexAtlas;
pthread_mutex_t mutexTexture;

static void *thread_atlas(void *)
{
    GLTexture *tex;
    std::string fileName;
    while (TextureManager::EnableThreads())
    {
        tex = 0;
        pthread_mutex_lock(&mutexAtlas);
        if (atlasPull.size())
        {
            tex = atlasPull.back().first;
            fileName = atlasPull.back().second;
            atlasPull.pop_back();
        }
        pthread_mutex_unlock(&mutexAtlas);
        if (tex)
            TextureManager::LoadAtlasDescription(tex, fileName);
        else
            usleep(1);
    }
    return NULL;
}

void TextureManager::ReadDescriptions(std::string fileName)
{
    std::vector<char> buffer;
    Core::ReadFileToBuffer(fileName.c_str(), buffer);

    if (buffer.size() == 0)
    { //    -  XML
		log::write("can not load file %s" , fileName.c_str());
		exit(-6);
	}

    rapidxml::xml_document<> doc;
    doc.parse<0>(&buffer[0]);
 
	rapidxml::xml_node<>* root = doc.first_node( "Resources" );
	assert( root );
	rapidxml::xml_node<>* element = root->first_node();
	while ( element ) {
		std::string name = element->name();
		if (name ==  "group") {
			std::string groupName = element->first_attribute( "name" )->value();
			rapidxml::xml_node<>* item = element->first_node( "texture" );
			while ( item ) {
				TextureState ts;
				ts.atlasPart = false;
				ts.group = groupName;
				ts.path = item->first_attribute( "path" )->value();
				ts.texture = NULL;
				std::string name = item->first_attribute( "id" )->value();

				Math::Replace(name);

				if (isTexture(name)) {
					log::write("resource already loaded : %s", name.c_str());
					assert(false);
				}
                
                if (TextureManager::EnableThreads())
                {
                    pthread_mutex_lock(&mutexTexture);
                    _texturesMap[name] = ts;
                    pthread_mutex_unlock(&mutexTexture);
                }
                else
                    _texturesMap[name] = ts;

                item = item->next_sibling();
			}
		} else if (name == "texture") {
			TextureState ts;
			ts.atlasPart = false;
			ts.group = "";
			ts.path = element->first_attribute( "path" )->value();
            ts.texture = new GLTexture2D(ts.path.c_str());
			std::string name = element->first_attribute( "id" )->value();

			Math::Replace(name);

			if (isTexture(name)) {
                log::write("resource already loaded : %s", name.c_str());
				assert(false);
			}
            
            if (TextureManager::EnableThreads())
            {
                pthread_mutex_lock(&mutexTexture);
                _texturesMap[name] = ts;
                pthread_mutex_unlock(&mutexTexture);
            }
            else
                _texturesMap[name] = ts;
		}
		element = element->next_sibling();
	}
}

void TextureManager::LoadGroup(std::string groupId)
{
    if (TextureManager::EnableThreads())
    {
        pthread_mutex_lock(&mutexTexture);
        for (TextureMap::iterator i = _texturesMap.begin(), e = _texturesMap.end(); i != e; i++) {
            if (i->second.group == groupId && i->second.texture == NULL) {
                i->second.texture = new GLTexture2D(i->second.path.c_str());
            }
        }
        pthread_mutex_unlock(&mutexTexture);
    }
    else
    {
        for (TextureMap::iterator i = _texturesMap.begin(), e = _texturesMap.end(); i != e; i++) {
            if (i->second.group == groupId && i->second.texture == NULL) {
                i->second.texture = new GLTexture2D(i->second.path.c_str());
            }
        }
    }
}

void TextureManager::UnloadGroup(std::string groupId)
{
    if (TextureManager::EnableThreads())
    {
        pthread_mutex_lock(&mutexTexture);
        for (TextureMap::iterator i = _texturesMap.begin(), e = _texturesMap.end(); i != e; i++) {
            if (i->first == groupId && i->second.texture != NULL) {
                delete i->second.texture;
                i->second.texture = NULL;
            }
        }
        pthread_mutex_unlock(&mutexTexture);
    }
    else
    {
        for (TextureMap::iterator i = _texturesMap.begin(), e = _texturesMap.end(); i != e; i++) {
            if (i->first == groupId && i->second.texture != NULL) {
                delete i->second.texture;
                i->second.texture = NULL;
            }
        }
    }
}

bool TextureManager::isTexture(const std::string &textureId)
{
    if (TextureManager::EnableThreads())
    {
        pthread_mutex_lock(&mutexTexture);
        bool r = (_texturesMap.find(textureId) != _texturesMap.end());
        pthread_mutex_unlock(&mutexTexture);
        return r;
    }
	return (_texturesMap.find(textureId) != _texturesMap.end());
}

bool TextureManager::isAtlasPart(const std::string &textureId)
{
    if (TextureManager::EnableThreads())
    {
        pthread_mutex_lock(&mutexTexture);
		TextureMap::iterator iter = _texturesMap.find(textureId);
        bool r = (iter != _texturesMap.end() && iter->second.atlasPart);
        pthread_mutex_unlock(&mutexTexture);
        return r;
    }
	TextureMap::iterator iter = _texturesMap.find(textureId);
    return (iter != _texturesMap.end() && iter->second.atlasPart);
}

GLTexture *TextureManager::getTexture(std::string textureId)
{
	Math::Replace(textureId);
	if (!isTexture(textureId)) {
		// если такой текстуры нет - может нам дают ее путь?
		TextureState ts;
		ts.atlasPart = false;
		ts.group = "";
		ts.path = textureId.c_str();
        ts.texture = new GLTexture2D((Core::GetDataDir() + ts.path).c_str());
        if (TextureManager::EnableThreads())
        {
            pthread_mutex_lock(&mutexTexture);
            _texturesMap[textureId] = ts;
            pthread_mutex_unlock(&mutexTexture);
        }
        else
            _texturesMap[textureId] = ts;
        
		return ts.texture;
	}
    else
    {
        if (TextureManager::EnableThreads())
        {
            pthread_mutex_lock(&mutexTexture);
            GLTexture *tex = _texturesMap[textureId].texture;
            pthread_mutex_unlock(&mutexTexture);
            return tex;
        }
        else
            return _texturesMap[textureId].texture;
	}
}

void TextureManager::unloadTexture(const std::string &textureId)
{
	if (isTexture(textureId))
	{
        if (TextureManager::EnableThreads())
        {
            pthread_mutex_lock(&mutexTexture);
            delete _texturesMap.find(textureId)->second.texture;
            _texturesMap.erase(textureId);
            pthread_mutex_unlock(&mutexTexture);
        }
        else
        {
            delete _texturesMap.find(textureId)->second.texture;
            _texturesMap.erase(textureId);
        }
	}
}

void TextureManager::ReleaseAllTextures() 
{
    if (TextureManager::EnableThreads())
    {
        pthread_mutex_lock(&mutexTexture);
        for (;_texturesMap.begin() != _texturesMap.end();) {
            if (_texturesMap.begin()->second.texture != NULL) {
                delete _texturesMap.begin()->second.texture;
            }
            _texturesMap.erase(_texturesMap.begin());
        }
        pthread_mutex_unlock(&mutexTexture);
    }
    else
    {
        for (;_texturesMap.begin() != _texturesMap.end();) {
            if (_texturesMap.begin()->second.texture != NULL) {
                delete _texturesMap.begin()->second.texture;
            }
            _texturesMap.erase(_texturesMap.begin());
        }
    }
}

void TextureManager::CreateSprite(GLTexture *tex, rapidxml::xml_node<> *xe)
{
	std::string textureId = xe->first_attribute("name")->value();
	Math::Replace(textureId);
	if (!isTexture(textureId)) {
		TextureState ts;
		ts.atlasPart = true;
		ts.group = "";
		ts.path = textureId.c_str();
		int x = atoi(xe->first_attribute("x")->value());
		int y = atoi(xe->first_attribute("y")->value());
		int w = atoi(xe->first_attribute("w")->value());
		int h = atoi(xe->first_attribute("h")->value());
		int shiftx = atoi(xe->first_attribute("shiftx")->value());
		int shifty = atoi(xe->first_attribute("shifty")->value());
        ts.texture = new Sprite(tex, x, y, w, h, shiftx, shifty);

        if (TextureManager::EnableThreads())
        {
            pthread_mutex_lock(&mutexTexture);
            _texturesMap[textureId] = ts;
            pthread_mutex_unlock(&mutexTexture);
        }
        else
            _texturesMap[textureId] = ts;
	}
	else
	{
		//log::write("this part already exist");
	}
}

void TextureManager::FinishThread()
{
    if (enableThreads)
    {   
		enableThreads = false;
        pthread_join(threadAtlas, NULL);
        pthread_mutex_destroy(&mutexAtlas);
        pthread_mutex_destroy(&mutexTexture);

        {
            GLTexture *tex;
            std::string fileName;
            while (atlasPull.size())
            {
                tex = atlasPull.back().first;
                fileName = atlasPull.back().second;
                atlasPull.pop_back();
                TextureManager::LoadAtlasDescription(tex, fileName);
            }
        }
    }
}

void TextureManager::LoadAtlasDescription(GLTexture *tex, const std::string &fileName)
{
    std::vector<char> buffer;
    Core::ReadFileToBuffer(fileName.c_str(), buffer);
    
    if (buffer.size())
    {
        rapidxml::xml_document<> doc;
        doc.parse<0>(&buffer[0]);
        
        rapidxml::xml_node<> *root = doc.first_node();
        rapidxml::xml_node<> *element = root->first_node();
        while (element != NULL) {
            CreateSprite(tex, element);
            element = element->next_sibling();
        }
    }
    else
    {
        log::write("Core %s - file not found", fileName.c_str());
    }
}

void TextureManager::EnableThread(bool v)
{
	if (!enableThreads && v)
	{
        pthread_create(&threadAtlas, NULL, thread_atlas, NULL);
        pthread_mutex_init(&mutexAtlas, NULL);
        pthread_mutex_init(&mutexTexture, NULL);
		enableThreads = v;
	}
	else if (enableThreads && !v)
	{
		FinishThread();
	}
}

void TextureManager::LoadAtlas(const std::string &fileName)
{
    log::write("loading : %s", fileName.c_str());
    GLTexture *tex = getTexture(fileName.substr(0, fileName.find(".")) + ".png");
    
    if (!enableThreads)
        LoadAtlasDescription(tex, fileName);
    else
    {
        pthread_mutex_lock(&mutexAtlas);
        assert(tex);
        atlasPull.push_back(std::make_pair(tex, fileName));
        pthread_mutex_unlock(&mutexAtlas);
    }
}


