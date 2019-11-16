#ifndef CLICKAREA_H
#define CLICKAREA_H

#include "BeautyBase.h"

//
// Украшение
// Область для клика
//

class ClickArea : public BeautyBase
{
private:

	std::vector<FPoint2D> _dots; // точки для редактирования
	std::vector<FPoint2D> _screenDots; 
	bool _mouseDown;

public:

	FPoint2D CalcCenter();

	virtual ~ClickArea();
	ClickArea(rapidxml::xml_node<> *xe);
	ClickArea(const ClickArea &c);

	virtual void Draw();
	virtual bool PixelCheck(const FPoint2D &point);
	virtual bool IsOnScreen();
	virtual bool WorldPointCheck(const FPoint2D &point) const;
	virtual std::string Type() const;
	virtual void ReloadTextures(bool touch = false) {}

};


#endif//CLICKAREA_H