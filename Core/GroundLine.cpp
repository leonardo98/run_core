#include "GroundLine.h"
#include "../Core/Math.h"
#include "../Core/Core.h"
#include "../Core/Messager.h"

GroundBase::~GroundBase() 
{
	_linked = NULL;
}

GroundLine::~GroundLine() {
}

GroundLine::GroundLine(const GroundLine &c) 
: GroundBase(c)
{
	_dots = c._dots;
}

GroundLine::GroundLine(rapidxml::xml_node<> *xe)
: GroundBase(xe)
{
	rapidxml::xml_node<> *dot = xe->first_node("dot");
	while (dot != NULL) {
		_dots.push_back(FPoint2D(static_cast<float>(atof(dot->first_attribute("x")->value())), static_cast<float>(atof(dot->first_attribute("y")->value()))));
		dot = dot->next_sibling();
	}
}

GroundLine::GroundLine(const std::string &igt, const FPoint2D &pos1,  const FPoint2D &pos2)
	: GroundBase(igt, pos1, pos2)
{
	_dots.resize(2);
	if (pos1.x < pos2.x)
	{
		_dots[0] = FPoint2D(0, 0);
		_dots[1] = pos2 - pos1;
	}
	else
	{
		_dots[0] = pos2 - pos1;
		_dots[1] = FPoint2D(0, 0);
	}
}

void GroundLine::DebugDraw() {
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);

	for (unsigned int i = 0; i < _dots.size(); ++i) {
		Render::Line(_dots[i].x, _dots[i].y, _dots[(i + 1) % _dots.size()].x, _dots[(i + 1) % _dots.size()].y, 0xFFFFFFFF);
	}

	Render::PopMatrix();
}

std::string GroundLine::Type() const { 
	return "GroundLine"; 
}

FPoint2D GroundLine::GetStart() {
	if (_dots.size() == 2 && _dots[1].x < _dots[0].x) {
		return _dots[1] + _pos;
	} else {
		return _dots[0] + _pos;
	}
}

FPoint2D GroundLine::GetFinish() {
	if (_dots.size() == 2 && _dots[1].x > _dots[0].x) {
		return _dots[1] + _pos;
	} else {
		return _dots[0] + _pos;
	}
}

void GroundLine::CreateDots(DotsList &dots) {
	dots = _dots;
	for (unsigned int i = 0; i < dots.size(); ++i) {
		dots[i] = (dots[i] + _pos) * (1.f / SCALE_BOX2D);
	}
}

float GroundLine::NearestGradient(const FPoint2D &p, float &gradient)
{
	gradient = 0.f;
	return Math::Distance(_dots[0], _dots[1], p * SCALE_BOX2D - _pos);
}