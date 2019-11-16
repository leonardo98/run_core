#ifndef SOLID_GROUND_LINE_H
#define SOLID_GROUND_LINE_H

#include "../Core/SplinePath.h"
#include "../Helpers/LittleHero.h"
#include "../Core/Render.h"
#include "GroundLine.h"

class Besier {
public:
	std::vector<FPoint2D> origin;
	void Recalc(std::vector<FPoint2D> &screen, int sectorNumber) const;
	float NearestGradient(const FPoint2D &p, float &gradient);
	void Path(float f, FPoint2D &pos);
	float Length() const;
	void RecalcWithNumber(std::vector<FPoint2D> &screen, int dotNumber) const;
};


class SolidGroundLine : public GroundBase
{

protected:	
	
	Besier _besier;

public:

	// только для отладки
	//virtual void Draw();

	SolidGroundLine(rapidxml::xml_node<> *xe);

	virtual std::string Type() const;

	virtual void CreateDots(DotsList &dots);
	void ExportToLines(std::vector<FPoint2D> &lineDots, int divisionBy = 6);

	virtual FPoint2D GetStart();
	virtual FPoint2D GetFinish();

	const Besier &GetBesier() { return _besier; }

	virtual float NearestGradient(const FPoint2D &p, float &gradient);
	virtual void ReloadTextures(bool touch = false) {}

};

#endif//SOLID_GROUND_LINE_H