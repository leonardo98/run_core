// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYENGINE_OBJECT_H
#define MYENGINE_OBJECT_H

#include "types.h"

#include <list>
#include <string>

//
// базовый класс, наследуемся от него если нам нужны только два 
// метода и не нужно обрабатывать ввод, иначе см. InputSystem.h
//

class Object;

typedef std::list<Object *> ObjectList;

class Object  
{
private:
	ObjectList _children;
    std::string _name;
	bool _fullscreen;
protected:
	bool _removeAfterTweens;
public:
	Object() : _removeAfterTweens(false), _fullscreen(false) {} 
	void Clear();
	void AddObject(Object *);
	virtual ~Object();
	virtual void Draw();
	virtual void Update(float);
	void SetFullscreen(bool v) { _fullscreen = v; }
	bool Fullscreen() { return _fullscreen; }
    void SetName(const std::string &name) { _name = name; }
    const std::string &GetName() { return _name; }
};

#endif//OBJECT_H
