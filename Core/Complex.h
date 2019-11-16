#ifndef COMPLEX_H
#define COMPLEX_H

#include "../Core/Render.h"
#include "BeautyBase.h"
#include "GroundLine.h"
#include "LevelSet.h"

class ComplexManager 
{

	typedef std::map<std::string, LevelSet *> ComplexMap;
	static ComplexMap _complexes;

public:

	static void ReloadTextures();
	static void UnloadAll();
	static LevelSet *getComplex(const std::string &complexId);

};

#endif//COMPLEX_H