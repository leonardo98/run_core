#ifndef GROUNDLINE_H
#define GROUNDLINE_H

#include "../Helpers/LittleHero.h"
#include "BeautyBase.h"

class GroundBase : public BeautyBase
{
public:
	GroundBase(const GroundBase &g)
		: BeautyBase(g)
	{}
	GroundBase(rapidxml::xml_node<> *xe)
		: BeautyBase(xe)
	{}
	GroundBase(const std::string &igt, const FPoint2D &pos1,  const FPoint2D &pos2)
		: BeautyBase(igt, pos1, pos2)
	{}
	virtual ~GroundBase();
	virtual void CreateDots(DotsList &dots) = 0;
	virtual FPoint2D GetStart() = 0;
	virtual FPoint2D GetFinish() = 0;
	virtual float NearestGradient(const FPoint2D &p, float &gradient) = 0;
};

class GroundLine : public GroundBase
{
private:

	std::vector<FPoint2D> _dots; // точки для редактирования
public:
	void DebugDraw();
	virtual ~GroundLine();
	GroundLine(rapidxml::xml_node<> *xe);
	GroundLine(const std::string &igt, const FPoint2D &pos1,  const FPoint2D &pos2);
	GroundLine(const GroundLine &c);

	virtual std::string Type() const;

	virtual void CreateDots(DotsList &dots);
	virtual FPoint2D GetStart();
	virtual FPoint2D GetFinish();
	virtual float NearestGradient(const FPoint2D &p, float &gradient);
	virtual void ReloadTextures(bool touch = false) {}
};


#endif//GROUNDLINE_H