#include "ClickArea.h"
#include "../Core/Math.h"
#include "../Core/Core.h"
#include "../Core/Messager.h"

ClickArea::~ClickArea() {
}

ClickArea::ClickArea(const ClickArea &c) 
: BeautyBase(c)
{
	_dots = c._dots;
	_mouseDown = false;
}

ClickArea::ClickArea(rapidxml::xml_node<> *xe)
: BeautyBase(xe)
{
	rapidxml::xml_node<> *dot = xe->first_node("dot");
	while (dot != NULL) {
		_dots.push_back(FPoint2D(static_cast<float>(atof(dot->first_attribute("x")->value())), static_cast<float>(atof(dot->first_attribute("y")->value()))));
		dot = dot->next_sibling();
	}
	_mouseDown = false;
}

void ClickArea::Draw() {

	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);

	_screenDots = _dots;
	for (unsigned int i = 0; i < _dots.size(); ++i) 
	{
		Render::GetCurrentMatrix().Mul(_screenDots[i]);	
	}
	//for (unsigned int i = 0; i < _dots.size(); ++i) {
	//	Render::Line(_dots[i].x, _dots[i].y, _dots[(i + 1) % _dots.size()].x, _dots[(i + 1) % _dots.size()].y, 0xFFFFFFFF);
	//}

	Render::PopMatrix();
}

bool ClickArea::IsOnScreen()
{
	for (unsigned int i = 0; i < _screenDots.size(); ++i) {
		if (_screenDots[i].x > 0 
			&& _screenDots[i].x < Render::ScreenWidth() 
			&& _screenDots[i].y > 0
			&& _screenDots[i].y < Render::ScreenHeight())
		{
			return true;
		}
	}
	return false;
}

bool ClickArea::PixelCheck(const FPoint2D &point) { 
	if (Math::Inside(point, _screenDots)) {
		return true;
	}
	return false;
}

std::string ClickArea::Type() const { 
	return "ClickArea"; 
}

FPoint2D ClickArea::CalcCenter() {
	FPoint2D pos(0.f, 0.f);
	for (std::vector<FPoint2D>::iterator i = _dots.begin(), e = _dots.end(); i != e; ++i) {
		pos += (*i);
	}
	return _pos + pos * (1.f / _dots.size());
}

bool ClickArea::WorldPointCheck(const FPoint2D &point) const {
	return Math::Inside(point - _pos, _dots);
}
