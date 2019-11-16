#ifndef LEVELSET_H
#define LEVELSET_H

#include "../Core/Render.h"
#include "BeautyBase.h"

#include <map>

struct LevelSet {
private:
	void BuildLinks();
	std::string id;
	bool _visible;
	BeautyList _namedBeauties;
public:
	int FindInNames(const std::string &name, unsigned int s, unsigned int e) const;
	void SetVisible(bool value) { _visible = value; }
	const std::string &Id() { return id; }
	BeautyList beauties;
	void Draw();
	bool Command(const std::string &cmd);
	void Update(float dt);
	void Clear();
	void LoadFromXml(rapidxml::xml_node<> *xe);
	const LevelSet &operator=(const LevelSet &l);
	LevelSet(const LevelSet &l);
	~LevelSet();
	LevelSet() { _visible = true; }
	// search only from begining of UserString
	void SelectByUserString(const std::string &keyWorld, BeautyBase *&beauty) const;
	void SelectByUserString(const std::string &keyWorld, BeautyList &list, bool appendAtEnd = false) const;
	void SelectIndexesByUserString(const std::string &keyWorld, std::vector<unsigned int> &list, bool appendAtEnd = false) const;
	// search anythere in UserString
	void SelectByUserStringSoft(const std::string &keyWorld, BeautyList &list, bool appendAtEnd = false) const;
	void SelectIndexesByUserStringSoft(const std::string &keyWorld, std::vector<unsigned int> &list, bool appendAtEnd = false) const;
	void SelectByUserStringHard(const std::string &keyWorld, BeautyList &list, bool appendAtEnd = false) const;
	void SelectByUserStringHard(const std::string &keyWorld, BeautyBase *&beauty, bool cutOrigin = false);

	void MakeCopyByUserStringHard(const std::string &keyWorld, BeautyList &list, bool appendAtEnd = false) const;
	void MakeCopyByUserStringHard(const std::string &keyWorld, BeautyBase *&beauty, bool hideOrigin = false) const;
	void GetBeautyList(BeautyList &list) const;
	void ReloadTextures(bool touch = false);
};

class LevelSetManager
{
protected:
	struct FileAndDoc
	{
		rapidxml::xml_document<> *doc;
		std::vector<char> buffer;
		std::map<std::string, rapidxml::xml_node<> *> names;
	};
	typedef std::map<std::string, FileAndDoc> LevelSetMap;
	static LevelSetMap _levelSets;
	static std::vector<LevelSet*> _levelSetUpdateList;
    static bool enableThread;
    static void FinishThread();
public:
    static bool EnableThread() { return enableThread; }
    static void EnableThread(bool v);
	//
	// Загрузить файл 
	// в случае успеха - вернет "true"
	// можно загружать несколько файлов - менеджер "коллекционирует" 
	//
	static bool Load(const std::string &fileName);
	
	//
	// Выгрузить все из памяти
	//
	static void UnloadAll();
	
	static bool GetLevel(const std::string &libId, const std::string &id, LevelSet &ls, bool skipMessage = false);

	static void ClearUpdateList() { _levelSetUpdateList.clear(); }
	static void PushToUpdateList(LevelSet *ls);
	static void Update(float dt) { for (std::vector<LevelSet*>::iterator i = _levelSetUpdateList.begin(), e = _levelSetUpdateList.end(); i != e; ++i) (*i)->Update(dt); }
    
    static bool LoadInner(const std::string &fileName);
    static bool IsLoaded(const std::string &fileName);
};

BeautyBase *MakeCopy(const BeautyBase *);

#endif//LEVELSET_H
