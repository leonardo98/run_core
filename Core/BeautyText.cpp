#include "BeautyText.h"

#include "../Core/Core.h"

BeautyText::BeautyText(const char *font, int xAlign /*= 0*/, int yAlign /*= 0*/)
{
    _font = Core::CreateFont(font);
	_xalign = xAlign;
	_yalign = yAlign;
	_hInterval = 0;
	_vSpacing = 0;
    _textFitScale = 1.f;

    _instances.push_back(this);
}

BeautyText::BeautyText(const BeautyText &b)
	: BeautyBase(b)
{
//		_text = b._text;
    _stdText = b._stdText;
	_xalign = b._xalign;
	_yalign = b._yalign;
	_hInterval = b._hInterval;
	_vSpacing = b._vSpacing;
    _textFitScale = b._textFitScale;
    _font = b._font;

    _instances.push_back(this);
}

void BeautyText::ClearStrings()
{
    _stdText.clear();
}

BeautyText::~BeautyText()
{
	ClearStrings();

    _instances.remove(this);
}

BeautyText::BeautyText(rapidxml::xml_node<> *xe)
	: BeautyBase(xe)
{
    _textFitScale = 1.f;
    const char *fileName = xe->first_attribute("font")->value();
    _font = Core::CreateFont(fileName);
	_xalign = atoi(xe->first_attribute("align")->value());
	_yalign = 0;
	_hInterval = (float)atof(xe->first_attribute("hInterval")->value());
	_vSpacing = (float)atof(xe->first_attribute("vSpacing")->value());

    std::string origin = xe->first_attribute("text")->value();

//    _stdText.push_back(origin);

    UpdateText(&origin);
    if (_stdText.empty() && _userString.size())
    {
        int i = 0;
        UpdateText(&origin);
    }

    _instances.push_back(this);
}

void BeautyText::UpdateText(std::string *origin)
{
    //std::string str;
	if (_userString.size() >= 2 && UserString()[0] == '#' && UserString()[1] != '#')
	{
    //	str = Core::GetLocalizedString(UserString().c_str());
	}
    if (origin)
    {
    //    if (str.IsEmpty())
	    {
		    if ((*origin).empty())
            {
                SetText("STRING IS NOT DEFINED");
            }
            else
            {
			    SetText(*origin);
            }
	    }
//		    else
//		    {
//			    SetTextById(UserString());
//		    }
    }
    //else if (!str.IsEmpty())
//    {
//		SetTextById(UserString());
//    }
}

BeautyText::BeautyText(const FPoint2D &pos, const std::string fontPath, const std::string &text, int xalign /*= 0*/, int yalign /*= 0*/)
	: _xalign(xalign)
	, _yalign(yalign)
	, BeautyBase("", pos)
{
    _textFitScale = 1.f;
    _font = Core::CreateFont(fontPath.c_str());
	SetText(text);
	_vSpacing = 0.f;
}

void BeautyText::Draw()
{
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	Render::MatrixRotate(_angle);
	Render::MatrixScale(_scale.x * _textFitScale, _scale.y);

//		Render::MatrixMove(0.f, static_cast<float>(Math::round(- _font->getLineSpacing() * 0.5f * (_text.size() + _stdText.size() - 1) * _hInterval)));

	if (_xalign == -1)
		Render::MatrixMove(static_cast<float>(Math::round(- Width() / 2.f)), 0.f);
	else if (_xalign == 1)
		Render::MatrixMove(static_cast<float>(Math::round( Width() / 2.f)), 0.f);

	Render::PushColorAndMul(_color);
	float y = 0;
    //_font->SetSpacing(_vSpacing);
//    for (std::vector<std::string>::iterator i = _stdText.begin(); i != _stdText.end(); ++i)
//    {
//        Render::PrintStringScaled(0, y, 1.f, 1.f, _font, (*i), _xalign, 0);
//        y += _font->GetSpacing() * _hInterval;
//    }
//    y = 0;
//    for (unsigned int i = 0; i < _stdText.size(); ++i)
//    {
//        Render::PrintStringScaled(0, y, 1.f, 1.f, _font, _stdText[i].c_str(), _xalign, 0);
//        y += _font->GetSpacing() * _hInterval;
//    }

    {
        if (_font && _stdText.size())
        {
            _font->SetSpacing(_hInterval);
            _font->SetTracking(_vSpacing);
            _font->Render(0, 0, _xalign, _stdText[0].c_str());
        }
    }
    //_font->SetSpacing(0.f);
	Render::PopColor();
	Render::PopMatrix();
}

void BeautyText::SetText(std::string str)
{
	ClearStrings();
	std::string::size_type index = str.find("#");
	std::string::size_type start = 0;
	unsigned int lineCounter = 0;
	while (index != std::string::npos) 
	{
		if (lineCounter >= _stdText.size())
		{
			_stdText.push_back(str.substr(start, index - start));
		}
		else
		{
			_stdText[lineCounter] = str.substr(start, index - start);
		}
		start = index + 1;
		index = str.find("#", start);
		lineCounter++;
	}
	if (_stdText.size() > lineCounter)
	{
		_stdText.resize(lineCounter);
	}
	_stdText.push_back(str.substr(start, index));
}
