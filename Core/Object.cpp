// Object.cpp: implementation of the Object class.
//
//////////////////////////////////////////////////////////////////////

#include "Object.h"

//////////////////////////////////////////////////////////////////////

Object::~Object()
{
	Clear();
}

void Object::Draw() 
{
	for (ObjectList::iterator o = _children.begin(), e = _children.end(); o != e; ++o)
	{
		(*o)->Draw();
	}
}

void Object::Update(float dt) 
{
	for (ObjectList::iterator o = _children.begin(); o != _children.end(); )
	{
		(*o)->Update(dt);
		if ((*o)->_removeAfterTweens)
		{
			delete (*o);
			o = _children.erase(o);
		}
		else
			++o;
	}
}

void Object::AddObject(Object *o)
{ 
	_children.push_back(o); 
}

void Object::Clear()
{
	for (ObjectList::iterator o = _children.begin(), e = _children.end(); o != e; ++o)
	{
		delete (*o);
	}
    _children.clear();
}