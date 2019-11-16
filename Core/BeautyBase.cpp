#include "BeautyBase.h"
#include "../Core/Render.h"
#include "../Core/Core.h"
#include "../Core/Math.h"

unsigned int __beauty_counter = 0;

BeautyBase::~BeautyBase() {
	for (std::list<PhisicLink *>::iterator i = _physicLinks.begin(), e = _physicLinks.end(); i != e; ++i)
	{
		delete (*i);
	}
	--__beauty_counter;
}

BeautyBase::BeautyBase(const BeautyBase &b) 
	: _linked(NULL)
    , m_fail(false)
	, _b2Body(NULL)
{
	++__beauty_counter;
	_parentBeauty = NULL;
	_visible = true;
	_pos = b._pos;
	_pointForAnything = b._pointForAnything;
	
	_angle = b._angle;
	_scalar = b._scalar;
	_color = b._color;

	_scale = b._scale;

	_width = b._width;
	_height = b._height;

	_userString = b._userString;
	_uid = b._uid;
	_tags = b._tags;
}

BeautyBase::BeautyBase(const std::string userString, const FPoint2D &pos1, const FPoint2D &pos2) 
	: _linked(NULL)
    , m_fail(false)
	, _b2Body(NULL)
{
	++__beauty_counter;
	_parentBeauty = NULL;
	_visible = true;
	_pos = pos1;
	_pointForAnything.x = 0;
	_pointForAnything.y = 0;
	_color = 0xFFFFFFFF;
	_angle = 0;
	_scalar = 0;

	_scale.x = 1;
	_scale.y = 1;

	_width = fabs(pos2.x - pos1.x);
	_height = fabs(pos2.y - pos1.y);

	_userString = userString;
	_uid = "";
}

BeautyBase::BeautyBase()
	: _linked(NULL)
	, _b2Body(NULL)
{
	++__beauty_counter;
	_parentBeauty = NULL;
	_visible = true;
	_pos.x = _pos.y = 0;
	_pointForAnything.x = 0;
	_pointForAnything.y = 0;
	_color = 0xFFFFFFFF;
	_angle = 0;
	_scalar = 0;

	_scale.x = 1;
	_scale.y = 1;

	_width = 1;
	_height = 1;

	_userString = "";
	_uid = "";
}

BeautyBase::BeautyBase(const std::string userString, const FPoint2D &pos, int w, int h) 
	: _linked(NULL)
	, _b2Body(NULL)
{
	++__beauty_counter;
	_parentBeauty = NULL;
	_visible = true;
	_pos = pos;
	_pointForAnything.x = 0;
	_pointForAnything.y = 0;
	_color = 0xFFFFFFFF;
	_angle = 0;
	_scalar = 0;

	_scale.x = 1;
	_scale.y = 1;

	_width = w;
	_height = h;

	_userString = userString;
	_uid = "";
}

BeautyBase::BeautyBase(rapidxml::xml_node<> *xe) 
	: _linked(NULL)
    , m_fail(false)
	, _b2Body(NULL)
{
	++__beauty_counter;
	_parentBeauty = NULL;
	_visible = true;
    rapidxml::xml_attribute<> *attr = xe->first_attribute("param");
	rapidxml::xml_attribute<> *tmp;
    if (attr)
    {
        int r;// used in editor
        int s;// used in editor
        sscanf(attr->value(), "%f,%f,%f,%f,%f,%f,%f,%f,%i,%i,%f,%f,%x"
                , &_scalar, &(_pos.x), &(_pos.y)
                , &(_pointForAnything.x), &(_pointForAnything.y)
                , &_angle, &(_scale.x), &(_scale.y)
                , &r, &s
				, &_width, &_height
                , &_color);
    }
    else
    {
        _pos.x = atof(xe->first_attribute("x")->value());
        _pos.y = atof(xe->first_attribute("y")->value());
        _pointForAnything.x = Math::Read(xe, "pax", 0.f);
        _pointForAnything.y = Math::Read(xe, "pay", 0.f);
		_width = static_cast<float>(atof(xe->first_attribute("width")->value()));
		_height = static_cast<float>(atof(xe->first_attribute("height")->value()));
        tmp = xe->first_attribute("scalar");
        _scalar = (tmp != NULL) ? atof(tmp->value()) : 0.f;

        tmp = xe->first_attribute("color");
        _color = tmp ? Math::ReadColor(tmp->value()) : 0xFFFFFFFF;

        _angle = Math::Read(xe, "angle", 0.f);

        _scale.x = Math::Read(xe, "sx", 1.f);
        _scale.y = Math::Read(xe, "sy", 1.f);
    }

    _color = (0xFF00FF00 & _color)
                | ((0x00FF0000 & _color) >> 16)
                | ((0x000000FF & _color) << 16);

    tmp = xe->first_attribute("inGameType");
	if (!tmp) {
		tmp = xe->first_attribute("name");
	}
	if (tmp) {
        _userString = tmp->value();
	}
	tmp = xe->first_attribute("uid");
	if (tmp) {
		_uid = tmp->value();
	}
	tmp = xe->first_attribute("tags");
	if (tmp) {
        std::string value(tmp->value());
        std::string::size_type b = 0;
        std::string::size_type e = value.find(";");
        while (true)
        {
            if (e == std::string::npos)
            {
                if (b < value.size())
                {
                    _tags.insert(value.substr(b));
                }
                break;
            }
            else
            {
                _tags.insert(value.substr(b, e - b));
                b = e + 1;
                e = value.find(";", b);
            }
        }
	}
}

void BeautyBase::SetPos(float x, float y) { 
	_pos.x = x; 
	_pos.y = y; 
}

void BeautyBase::SetAlpha(float alpha) {
    assert(0 <= alpha && alpha <= 1.f);
    _color = Math::round(alpha * 0xFF) << 24 | (_color & 0xFFFFFF);
}

float BeautyBase::GetAlpha() const
{
	return static_cast<float>((_color & 0xFF000000) >> 24) / 0xFF;
}

void BeautyBase::SetScale(float s) { 
	SetScale(s, s);
}

void BeautyBase::SetScale(float sx, float sy) { 
	_scale.x = sx;
	_scale.y = sy;
}

void BeautyBase::ShiftTo(float dx, float dy) { 
	_pos.x += dx; 
	_pos.y += dy; 
}

const FPoint2D &BeautyBase::GetPos() const { 
	return _pos; 
}

void BeautyBase::Draw() 
{
}

void BeautyBase::Update(float dt) 
{
	TweenPlayer::Update(this, dt); 
}

bool BeautyBase::HasTag(const std::string &tag) const
{
    return _tags.find(tag) != _tags.end();
}
