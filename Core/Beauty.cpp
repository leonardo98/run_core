#include "Beauty.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include "LevelSet.h"

Beauty::~Beauty() {
}

Beauty::Beauty(rapidxml::xml_node<> *xe) 
: BeautyBase(xe)
{
	_fileName = xe->first_attribute("texture")->value();
	_sprite = Core::getTexture(_fileName);
    m_fail = (_sprite == NULL);
	_localPos.Move(_pos.x, _pos.y);
	_localPos.Rotate(_angle);
	_localPos.Scale(_scale.x, _scale.y);
	_localPos.Move(Math::round(- Width() / 2), Math::round(- Height() / 2));
}

Beauty::Beauty(GLTexture *spr)
{
    _sprite = spr;
    m_fail = false;
}

void Beauty::SetPos(float x, float y) {
	BeautyBase::SetPos(x, y);
	_localPos.Unit();
	_localPos.Move(_pos.x, _pos.y);
	_localPos.Rotate(_angle);
	_localPos.Scale(_scale.x, _scale.y);
	_localPos.Move(Math::round(- Width() / 2), Math::round(- Height() / 2));
}

void Beauty::ShiftTo(float dx, float dy) { 
	BeautyBase::ShiftTo(dx, dy);
	_localPos.Unit();
	_localPos.Move(_pos.x, _pos.y);
	_localPos.Rotate(_angle);
	_localPos.Scale(_scale.x, _scale.y);
	_localPos.Move(Math::round(- Width() / 2), Math::round(- Height() / 2));
}

void Beauty::SetScale(float sx, float sy) { 
	BeautyBase::SetScale(sx, sy);
	_localPos.Unit();
	_localPos.Move(_pos.x, _pos.y);
	_localPos.Rotate(_angle);
	_localPos.Scale(_scale.x, _scale.y);
	_localPos.Move(Math::round(- Width() / 2), Math::round(- Height() / 2));
}

void Beauty::SetAngle(float angle)
{
	BeautyBase::SetAngle(angle);
	_localPos.Unit();
	_localPos.Move(_pos.x, _pos.y);
	_localPos.Rotate(_angle);
	_localPos.Scale(_scale.x, _scale.y);
	_localPos.Move(Math::round(- Width() / 2), Math::round(- Height() / 2));
}


void Beauty::Draw() 
{
	Render::PushColorAndMul(_color);
	_sprite->Render(_localPos);
	Render::PopColor();
}

std::string Beauty::Type() const { 
	return "Beauty"; 
}

int Beauty::Width() {
	return _sprite->Width();
}

int Beauty::Height() {
	return _sprite->Height();
}

Beauty::Beauty(const Beauty &b)
: BeautyBase(b)
{
	_fileName = b._fileName;
	_sprite = b._sprite;
	_localPos = b._localPos;
}

void Beauty::GetBeautyList(BeautyList &list) const {
	BeautyBase *s = MakeCopy(this);
	list.push_back(s);
}

void Beauty::ReloadTextures(bool touch)
{
    assert(false); // todo fix it
//    if (touch)
//        _sprite->GetTexture()->Touch();
//    else
//        _sprite = Core::getTexture(_fileName);
}
