#include "LevelSet.h"
#include "../Core/Math.h"
#include "../Core/Core.h"
#include "Beauty.h"
#include "BeautyText.h"
#include "ClickArea.h"
#include "AnimationArea.h"
#include "SolidGroundLine.h"
#include "GroundLine.h"
#include "TexturedMesh.h"
#include "LinkToComplex.h"
#include <pthread.h>
//#include <unistd.h>

bool SortByNames(const BeautyBase *one, const BeautyBase *two)
{
	return one->UserString() < two->UserString();
}

void LevelSet::Clear()
{
	_visible = true;
	for (unsigned int i = 0; i < beauties.size(); ++i) {
		delete beauties[i];
	}
	beauties.clear();
	_namedBeauties.clear();
}

void LevelSet::LoadFromXml(rapidxml::xml_node<> *xe) {
	Clear();
	assert(xe != NULL);
	id = xe->first_attribute("id")->value();
	// level loading
	rapidxml::xml_node<> *beautyList = xe->first_node("Beauties");
	if (beautyList) {
		rapidxml::xml_node<> *elem = beautyList->first_node();
		std::string typeName;
		BeautyBase *beauty;	
		while (elem != NULL) {
			typeName = elem->name();
			if (typeName == "ClickArea") {
				beauty = new ClickArea(elem);
			} else if (typeName == "Beauty") {
				beauty = new Beauty(elem);
			} else if (typeName == "BeautyText") {
				beauty = new BeautyText(elem);
			} else if (typeName == "Animation") {
				beauty = new AnimationArea(elem);
			} else if (typeName == "LinkToComplex") {
				LinkToComplex *link = new LinkToComplex(elem);
				beauty = link;
			} else if (typeName == "GroundSpline") {
				beauty = new SolidGroundLine(elem);
			} else if (typeName == "SolidGroundLine") {
				beauty = new SolidGroundLine(elem);
			} else if (typeName == "GroundLine") {
				beauty = new GroundLine(elem);
			} else {
				beauty = new TexturedMesh(elem);
			}
			if (beauty->m_fail)
            {
                delete beauty;
            }
            else
            {
				beauties.push_back(beauty);
				if (!beauty->UserString().empty())
				{
					_namedBeauties.push_back(beauty);
				}
			}
			elem = elem->next_sibling();
		}
	}
	std::sort(_namedBeauties.begin(), _namedBeauties.end(), SortByNames);
	BuildLinks();
}

LevelSet::LevelSet(const LevelSet &l) 
	: id(l.id)
	, _visible(l._visible)
{
	for (unsigned int i = 0; i < l.beauties.size(); ++i) 
	{
		beauties.push_back(MakeCopy(l.beauties[i]));
		if (!beauties.back()->UserString().empty())
		{
			_namedBeauties.push_back(beauties.back());
		}
	}
	std::sort(_namedBeauties.begin(), _namedBeauties.end(), SortByNames);
}

void LevelSet::GetBeautyList(BeautyList &list) const 
{
	for (unsigned int i = 0; i < beauties.size(); ++i) {
		beauties[i]->GetBeautyList(list);
	}
}

LevelSet::~LevelSet()
{
	Clear();
}

const LevelSet &LevelSet::operator=(const LevelSet &l) 
{
	Clear();
	id = l.id;
	_visible = l._visible;
	for (unsigned int i = 0; i < l.beauties.size(); ++i) {
		beauties.push_back(MakeCopy(l.beauties[i]));
		if (!beauties.back()->UserString().empty())
		{
			_namedBeauties.push_back(beauties.back());
		}
	}
	std::sort(_namedBeauties.begin(), _namedBeauties.end(), SortByNames);
	BuildLinks();
	return *this;
}

void LevelSet::SelectByUserString(const std::string &keyWorld, BeautyBase *&beauty) const {
	beauty = NULL;
	for (unsigned int i = 0; i < beauties.size(); ++i) {
		if (beauties[i]->UserString().find(keyWorld) == 0) {
			beauty = beauties[i];
			return;
		}
	}
}

void LevelSet::SelectByUserString(const std::string &keyWorld, BeautyList &list, bool appendAtEnd) const {
	if (!appendAtEnd) {
		list.clear();
	}
	for (unsigned int i = 0; i < beauties.size(); ++i) {
		if (beauties[i]->UserString().find(keyWorld) == 0) {
			list.push_back(beauties[i]);
		}
	}
}

void LevelSet::SelectIndexesByUserString(const std::string &keyWorld, std::vector<unsigned int> &list, bool appendAtEnd) const {
	if (!appendAtEnd) {
		list.clear();
	}
	for (unsigned int i = 0; i < beauties.size(); ++i) {
		if (beauties[i]->UserString().find(keyWorld) == 0) {
			list.push_back(i);
		}
	}
}

void LevelSet::SelectByUserStringHard(const std::string &keyWorld, BeautyList &list, bool appendAtEnd) const {
	if (!appendAtEnd) {
		list.clear();
	}
	int index = FindInNames(keyWorld, 0, _namedBeauties.size());
	while (index > 0 && _namedBeauties[index - 1]->UserString() == keyWorld) --index;
	while (index < _namedBeauties.size() && _namedBeauties[index]->UserString() == keyWorld) 
	{
		list.push_back(_namedBeauties[index++]);
	}
}

int LevelSet::FindInNames(const std::string &name, unsigned int s, unsigned int e) const
{
	if (e <= s) return -1;
	if ((e - s) == 1)
	{
		if (_namedBeauties[s]->UserString() == name)
		{
			return (int)s;
		}
		else return -1;
	}
	unsigned int m = (s + e) / 2;
	if (_namedBeauties[m]->UserString() == name)
	{
		return (int)m;
	}
	else if (_namedBeauties[m]->UserString() > name)
	{
		return FindInNames(name, s, m);
	}
	else
	{
		return FindInNames(name, m + 1, e);
	}
}

void LevelSet::SelectByUserStringHard(const std::string &keyWorld, BeautyBase *&beauty, bool cutOrigin) 
{
	beauty = NULL;
	int index = FindInNames(keyWorld, 0, _namedBeauties.size());
	if (index >= 0)
	{
		beauty = _namedBeauties[index];
		if (cutOrigin)
		{
			_namedBeauties.erase(_namedBeauties.begin() + index);
			for (BeautyList::iterator i = beauties.begin(), e = beauties.end(); i != e; ++i) 
			{
				if ((*i) == beauty)
				{
					beauties.erase(i);
					return;
				}
			}

		}
	}
}

void LevelSet::SelectByUserStringSoft(const std::string &keyWorld, BeautyList &list, bool appendAtEnd) const {
	if (!appendAtEnd) {
		list.clear();
	}
	for (unsigned int i = 0; i < beauties.size(); ++i) {
		if (beauties[i]->UserString().find(keyWorld) != std::string::npos) {
			list.push_back(beauties[i]);
		}
	}
}

void LevelSet::SelectIndexesByUserStringSoft(const std::string &keyWorld, std::vector<unsigned int> &list, bool appendAtEnd) const {
	if (!appendAtEnd) {
		list.clear();
	}
	for (unsigned int i = 0; i < beauties.size(); ++i) {
		if (beauties[i]->UserString().find(keyWorld) != std::string::npos) {
			list.push_back(i);
		}
	}
}

void LevelSet::Draw() 
{
    TIME_TEST(30)
    if (!_visible)
	{
		return;
	}
    for (unsigned int i = 0; i < beauties.size(); ++i) {
        BeautyBase *b = beauties[i];
        if (b->Visible()) {
            b->Draw();
        }
    }
}

bool LevelSet::Command(const std::string &cmd) 
{
	bool result = false;
    for (unsigned int i = 0; i < beauties.size(); ++i) {
        if (beauties[i]->Command(cmd))
		{
			result = true;
		}
    }
	return result;
}

void LevelSet::MakeCopyByUserStringHard(const std::string &keyWorld, BeautyList &list, bool appendAtEnd) const {
	if (!appendAtEnd) {
		for (BeautyList::iterator i = list.begin(), e = list.end(); i != e; ++i) 
		{
				delete (*i);
		}
		list.clear();
	}
	for (unsigned int j = 0; j < beauties.size(); ++j) {
		if (beauties[j]->Type() == "LinkToComplex")
		{
			LinkToComplex *link = static_cast<LinkToComplex *>(beauties[j]);
			BeautyList inner;
			link->GetComplex()->MakeCopyByUserStringHard(keyWorld, inner);
			for (unsigned int i = 0; i < inner.size(); ++i) {
				inner[i]->ShiftTo(link->GetPos().x + link->GetShiftX(), link->GetPos().y + link->GetShiftY());
				if (inner[i]->ParentBeauty() == 0)
				{
					inner[i]->ParentBeauty() = link;
				}
				list.push_back(inner[i]);
			}
			if (beauties[j]->Linked() == NULL && inner.size() == 1)
			{
				beauties[j]->LinkWith(inner[0]);
			}
		}
		else if (beauties[j]->UserString() == keyWorld)
		{
			list.push_back(MakeCopy(beauties[j]));
		}
	}
}

void LevelSet::MakeCopyByUserStringHard(const std::string &keyWorld, BeautyBase *&beauty, bool hideOrigin) const
{
	assert(beauty == NULL);
	beauty = NULL;
	for (unsigned int j = 0; j < beauties.size(); ++j) {
		if (beauties[j]->Type() == "LinkToComplex")
		{
			LinkToComplex *link = static_cast<LinkToComplex *>(beauties[j]);
			link->GetComplex()->MakeCopyByUserStringHard(keyWorld, beauty, hideOrigin);
			if (beauty)
			{
				beauty->ShiftTo(link->GetPos().x + link->GetShiftX(), link->GetPos().y + link->GetShiftY());
				if (beauty->ParentBeauty() == 0)
				{
					beauty->ParentBeauty() = link;
				}
				return;
			}
		}
		else if (beauties[j]->UserString() == keyWorld)
		{
			if (hideOrigin)
				beauties[j]->Visible() = false;
			beauty = MakeCopy(beauties[j]);
			return;
		}
	}
}

void LevelSet::BuildLinks()
{
	BeautyList linked;
	for (BeautyList::iterator i = beauties.begin(), e = beauties.end(); i != e; ++i) {
		if ((*i)->UID().size() && (*i)->UID()[0] == ':')
		{
			linked.push_back(*i);
		}
	}
	std::string uid;
	for (BeautyList::iterator i = linked.begin(), e = linked.end(); i != e; ++i) {
		uid = (*i)->UID().substr(1);
		for (BeautyList::iterator j = beauties.begin(), k = beauties.end(); j != k; ++j) {
			if ((*j)->UID() == uid)
			{
				(*i)->LinkWith(*j);
				break;
			}
		}
	}
}

LevelSetManager::LevelSetMap LevelSetManager::_levelSets;
std::vector<std::string> levelsPull;
pthread_t threadLevelSet;
pthread_mutex_t mutexLoad;
pthread_mutex_t mutexLevel;
bool LevelSetManager::enableThread = false;

static void *thread_levels(void *)
{
    std::string fileName;
    while (LevelSetManager::EnableThread())
    {
        pthread_mutex_lock(&mutexLoad);
        fileName.clear();
        if (levelsPull.size())
        {
            fileName = levelsPull.back();
            levelsPull.pop_back();
        }
        pthread_mutex_unlock(&mutexLoad);
        if (fileName.size())
            LevelSetManager::LoadInner(fileName);
        else
            usleep(1);
    }
    return NULL;
}

void LevelSetManager::FinishThread()
{
    if (enableThread)
    {
		enableThread = false;
        pthread_join(threadLevelSet, NULL);
        pthread_mutex_destroy(&mutexLevel);
        pthread_mutex_destroy(&mutexLoad);
        
        while (levelsPull.size())
        {
            LevelSetManager::LoadInner(levelsPull.back());
            levelsPull.pop_back();
        }
    }
}

bool LevelSetManager::LoadInner(const std::string &fileName)
{
    std::vector<char> buffer;
    Core::ReadFileToBuffer(fileName.c_str(), buffer);
    
    if (buffer.size())
    {
        rapidxml::xml_document<> *doc = new rapidxml::xml_document<>();
        
        if (enableThread)
        {
            pthread_mutex_lock(&mutexLevel);
            _levelSets[fileName].buffer = buffer;
            doc->parse<0>(&_levelSets[fileName].buffer[0]);
            _levelSets[fileName].doc = doc;
            pthread_mutex_unlock(&mutexLevel);
        }
        else
        {
            _levelSets[fileName].buffer = buffer;
            doc->parse<0>(&_levelSets[fileName].buffer[0]);
            _levelSets[fileName].doc = doc;
        }
        return true;
    } 
    return false;
}

bool LevelSetManager::IsLoaded(const std::string &fileName)
{
    if (enableThread)
    {
        pthread_mutex_lock(&mutexLevel);
        bool r = _levelSets.find(fileName) != _levelSets.end();
        pthread_mutex_unlock(&mutexLevel);
        return r;
    }
    else
    {
        return _levelSets.find(fileName) != _levelSets.end();
    }
}

void LevelSetManager::EnableThread(bool v)
{
	if (!enableThread && v)
	{
        pthread_create(&threadLevelSet, NULL, thread_levels, NULL);
        pthread_mutex_init(&mutexLevel, NULL);
        pthread_mutex_init(&mutexLoad, NULL);
		enableThread = v;
	}
	else if (enableThread && !v)
	{
		FinishThread();
	}
}
	

bool LevelSetManager::Load(const std::string &fileName)
{
    log::write("loading : %s", fileName.c_str());
    assert(!IsLoaded(fileName));

    if (!enableThread)
        return LoadInner(fileName);
    else
    {
        pthread_mutex_lock(&mutexLoad);
        levelsPull.push_back(fileName);
        pthread_mutex_unlock(&mutexLoad);
    }
    return false;
}

void LevelSetManager::UnloadAll()
{
    if (enableThread)
    {
        pthread_mutex_lock(&mutexLevel);
        for (LevelSetManager::LevelSetMap::iterator i = _levelSets.begin(), e = _levelSets.end(); i != e; ++i)
        {
            delete (i->second.doc);
        }
        _levelSets.clear();
        pthread_mutex_unlock(&mutexLevel);
    }
    else
    {
        for (LevelSetManager::LevelSetMap::iterator i = _levelSets.begin(), e = _levelSets.end(); i != e; ++i)
        {
            delete (i->second.doc);
        }
        _levelSets.clear();
    }
}

bool LevelSetManager::GetLevel(const std::string &libId, const std::string &id, LevelSet &level, bool skipMessage)
{
    assert(!enableThread);
	if (libId.size() == 0)
	{
		for (LevelSetManager::LevelSetMap::iterator i = _levelSets.begin(), e = _levelSets.end(); i != e; ++i)
		{
			if (GetLevel(i->first, id, level, true))
			{
				return true;
			}
		}
		return false;
	}
	assert(IsLoaded(libId));

	level.Clear();

    {
		FileAndDoc &fileAndDoc = _levelSets[libId];
		rapidxml::xml_document<> *doc = fileAndDoc.doc;
		if (fileAndDoc.names.size() == 0)
		{
			rapidxml::xml_node<> *xe = doc->first_node()->first_node();
			while (xe != NULL) 
			{
                assert(xe->first_attribute("id"));
                assert(std::string(xe->first_attribute("id")->value()) != "");
				fileAndDoc.names[xe->first_attribute("id")->value()] = xe;
				xe = xe->next_sibling();
			}
		}

		std::map<std::string, rapidxml::xml_node<> *>::iterator xe = fileAndDoc.names.find(id);

		if (xe != fileAndDoc.names.end())
		{
			level.LoadFromXml(xe->second);
		}
		else if (!skipMessage)
		{
            log::write("Level not found: %s", id.c_str());
		}
		return xe != fileAndDoc.names.end();
	}
}

void LevelSet::ReloadTextures(bool touch) {
	for (unsigned int i = 0; i < beauties.size(); ++i) {
		beauties[i]->ReloadTextures(touch);
	}
}

BeautyBase *MakeCopy(const BeautyBase *origin)
{
	std::string typeName;
	BeautyBase *beauty;
	typeName = origin->Type();
	if (typeName == "ClickArea") {
		beauty = new ClickArea(*(ClickArea *)(origin));
	} else if (typeName == "Beauty") {
		beauty = new Beauty(*(Beauty *)(origin));
	} else if (typeName == "BeautyText") {
		beauty = new BeautyText(*(BeautyText *)(origin));
	} else if (typeName == "Animation") {
		beauty = new AnimationArea(*(AnimationArea *)(origin));
	} else if (typeName == "LinkToComplex") {
		beauty = new LinkToComplex(*(LinkToComplex *)(origin));
	} else if (typeName == "SolidGroundLine") {
		beauty = new SolidGroundLine(*(SolidGroundLine *)(origin));
	} else if (typeName == "GroundLine") {
		beauty = new GroundLine(*(GroundLine *)(origin));
	} else {
		beauty = new TexturedMesh(*(TexturedMesh *)(origin));
	}
	return beauty;
}

void LevelSet::Update(float dt) 
{
	for (BeautyList::iterator i = beauties.begin(); i != beauties.end(); ++i ) 
	{
		(*i)->Update(dt);
	}
}

void LevelSetManager::PushToUpdateList(LevelSet *ls)
{
	for (std::vector<LevelSet*>::iterator i = _levelSetUpdateList.begin(), e = _levelSetUpdateList.end(); i != e; ++i) 
	{
		if ((*i) == ls)
		{
			return;
		}
	}
	_levelSetUpdateList.push_back(ls);
}

std::vector<LevelSet*> LevelSetManager::_levelSetUpdateList;
