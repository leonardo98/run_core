#include "LinkToComplex.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include "Complex.h"

LinkToComplex::~LinkToComplex() {
}

LinkToComplex::LinkToComplex(const std::string &id)
	: BeautyBase(id, FPoint2D(0, 0), 1, 1)
{
	_id = id;
	_complex = ComplexManager::getComplex(_id);
	LevelSetManager::PushToUpdateList(_complex);
	_shiftX = 0.f;//static_cast<float>(atof(xe->first_attribute("shiftX")->value()));
	_shiftY = 0.f;//static_cast<float>(atof(xe->first_attribute("shiftY")->value()));
}

LinkToComplex::LinkToComplex(rapidxml::xml_node<> *xe) 
: BeautyBase(xe)
{
	_id = xe->first_attribute("complex")->value();
	_complex = ComplexManager::getComplex(_id);
	_shiftX = static_cast<float>(atof(xe->first_attribute("shiftX")->value()));
	_shiftY = static_cast<float>(atof(xe->first_attribute("shiftY")->value()));
}

LinkToComplex::LinkToComplex(const LinkToComplex &b)
: BeautyBase(b)
{
	_id = b._id;
	_shiftX = b._shiftX;
	_shiftY = b._shiftY;
	_complex = ComplexManager::getComplex(_id);
}

void LinkToComplex::Draw() {
    FPoint2D a(- Width() / 2.f, 0);
    FPoint2D b(Width() / 2.f, 0);
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
    Render::GetCurrentMatrix().Mul(a);
    Render::GetCurrentMatrix().Mul(b);
    if (a.x > Render::ScreenWidth() || b.x < 0)
    {
        Render::PopMatrix();
        return;
    }
	Render::MatrixRotate(_angle);
	Render::MatrixScale(_scale.x, _scale.y);
	Render::MatrixMove(_shiftX, _shiftY);
	//_drawMatrix = Render::GetCurrentMatrix();
	Render::PushColorAndMul(_color);
	_complex->Draw();
	Render::PopColor();
	Render::PopMatrix();
}

std::string LinkToComplex::Type() const { 
	return "LinkToComplex"; 
}

void LinkToComplex::GetBeautyList(BeautyList &list) const {
	BeautyList ls;
	_complex->GetBeautyList(ls);

	FPoint2D tmp;
	for (BeautyList::iterator i = ls.begin(), e = ls.end(); i != e; ++i) {
		tmp = (*i)->GetPos();
		tmp.x  += _shiftX;
		tmp.y  += _shiftY;
		(*i)->SetPos(*(tmp.Rotate(_angle)) + _pos);
		(*i)->SetAngle((*i)->GetAngle() + _angle);
		list.push_back(*i);
	}
}

void LinkToComplex::ReloadTextures(bool touch)
{
	_complex->ReloadTextures(touch);
}
