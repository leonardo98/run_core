#ifndef BEAUTY_TEXT_H
#define BEAUTY_TEXT_H

#include "BeautyBase.h"
#include "Tweens.h"
#include "../Core/CoreFont.h"

class BeautyText : public BeautyBase 
{
private:
	std::vector<std::string> _stdText;
    std::shared_ptr<CoreFont> _font;
	int _xalign;
	int _yalign;
	float _hInterval;
	float _vSpacing;
    float _textFitScale;
    static std::list<BeautyText *> _instances;

public:
	void SetSize(float width, float height)
	{
		_width = width;
		_height = height;
	}
	void SetHorInterval(float v)
	{
		_hInterval = v;
	}
	BeautyText(const char *font, int xAlign = 0, int yAlign = 0);
	BeautyText(const BeautyText &b);
	void ClearStrings();
	virtual ~BeautyText();
	BeautyText(rapidxml::xml_node<> *xe);

    void UpdateText(std::string *origin);

    static void UpdateLocalization()
    {
        for (auto &a:_instances)
        {
            a->UpdateText(nullptr);
        }
    }

	void SetTextById(const std::string &strId)
	{
        _stdText.clear();
        ClearStrings();
//		std::list<std::vector<RSCHAR> > WordList;
//		cTextDividerBase::FillWordList(Core::GetLocalizedString(strId.c_str()), WordList, NULL);
//		int foo;
//		cTextDividerBase::BuildDivision(WordList, _font, 1.f, (float)Width(), &_text, foo);
//		WordList.clear();
	}
	void SetText(std::string str);
//	void SetText(const std::string &str)
//	{
//		ClearStrings();
//		_stdText.clear();
//		RSCHAR *copy = new RSCHAR[str.Length() + 1];
//		memset(copy, 0, (str.Length() + 1) * sizeof(RSCHAR));
//		memcpy(copy, str.operator const RSCHAR *(), str.Length() * sizeof(RSCHAR));
//		_text.push_back(copy);
//    }
    void ScaleText()
    {
//        float length = static_cast<float>(grGetLength(_font, _text.back()));
//        if (length > Width())
//        {
//            _textFitScale = Width() / length * 1.2f;
//        }
//        else
//            _textFitScale = 1.f;
    }
	BeautyText(const FPoint2D &pos, const std::string fontPath, const std::string &text, int xalign = 0, int yalign = 0);
	virtual void Draw() override;
	//void SetText(const std::string &text) { _text = text; }
	virtual void ReloadTextures(bool touch = false) {}
	virtual std::string Type() const { return "BeautyText"; } 
};

class TweenCounter : public Tween
{
private:
	int _num;
	std::string _mask;
public:
	TweenCounter(int num, const char *mask)
		: _num(num)
		, _mask(mask)
	{}
	virtual void UpdatePosition(BeautyBase *beauty, float p) 
	{
		BeautyText *text = dynamic_cast<BeautyText *>(beauty);
		if (text)
		{
			static char buff[100];
			sprintf(buff, _mask.c_str(), Math::lerp(0, _num, p));
			text->SetText(buff);
		}
	}
};

#endif//BEAUTY_TEXT_H
