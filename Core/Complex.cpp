#include "Complex.h"
#include "../Core/Math.h"
#include "Beauty.h"
#include "ClickArea.h"
#include "AnimationArea.h"
#include "SolidGroundLine.h"
#include "GroundLine.h"
#include "TexturedMesh.h"
#include "LinkToComplex.h"
#include "LevelSet.h"

void ComplexManager::ReloadTextures() {
	for (ComplexMap::iterator it = _complexes.begin(); it != _complexes.end(); it++) {
		(*it).second->ReloadTextures();
	}
}

//
// Выгрузить все анимации и текстуры из памяти
//
void ComplexManager::UnloadAll() {
	for (ComplexMap::iterator it = _complexes.begin(); it != _complexes.end(); it++) {
		delete (*it).second;
	}
	_complexes.clear();
}

//
// Получить указатель на анимацию по animationId - это имя анимации в редакторе 
// (ВАЖНО нельзя вызывать delete для нее!)
//
LevelSet *ComplexManager::getComplex(const std::string &complexId) 
{
	ComplexMap::iterator it_find = _complexes.find(complexId);
	if (it_find != _complexes.end()) {
		return (*it_find).second;
	}
	LevelSet *c = new LevelSet();
	if (LevelSetManager::GetLevel("", complexId, *c))
	{
		_complexes[complexId] = c;
		return c;
	}

	log::write("complex %s not found.", complexId.c_str());
	return NULL;
}

ComplexManager::ComplexMap ComplexManager::_complexes;
