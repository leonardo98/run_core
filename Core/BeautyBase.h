#ifndef BEAUTY_BASE_H
#define BEAUTY_BASE_H

#include "../Core/Render.h"
#include "PhisicLink.h"
#include "Tweens.h"

#include <set>

//
// Базовый класс для украшений - для статичной картинки или анимации или цветного многоугольника
//

typedef std::vector<BeautyBase *> BeautyList;

class BeautyBase : virtual public TweenPlayer
{
protected:
	// эти переменные общие для украшений
	std::string _userString;
	std::string _uid;
    std::set<std::string> _tags;
	FPoint2D _pos;
	float _angle;
	float _scalar;
    unsigned int _color;
	FPoint2D _scale;
	bool _visible;
	float _width;
	float _height;
	FPoint2D _pointForAnything;
	BeautyBase *_parentBeauty;
	// чтобы не дублировать в наследниках
	Matrix parent;
	typedef std::list<PhisicLink *> PhisicLinkList;
	PhisicLinkList _physicLinks;
	BeautyBase *_linked;
	b2Body *_b2Body;

public:
	std::string &UID() { return _uid; }
    bool m_fail;
	void LinkWith(BeautyBase *beauty) { _linked = beauty; }
	BeautyBase *Linked() { return _linked; }
	void LinkWithBody(b2Body *body) { _b2Body = body; }
	b2Body *LinkedBody() { return _b2Body; }
	void AddPhysicBody(PhisicLink *pl) { _physicLinks.push_back(pl); }
	PhisicLinkList GetPhysicLinks() { return _physicLinks; }
    void SetColor(unsigned int color) { _color = color; }
    unsigned int GetColor() const { return _color; }
	BeautyBase *&ParentBeauty() { return _parentBeauty; }
	inline bool &Visible() { return _visible; }
	virtual ~BeautyBase();
	BeautyBase(const BeautyBase &b);
	BeautyBase(rapidxml::xml_node<> *xe);
	BeautyBase(const std::string userString, const FPoint2D &, const FPoint2D &);
	BeautyBase(const std::string userString, const FPoint2D &, int w = 0, int h = 0);
	BeautyBase();

	virtual void Update(float dt);
	virtual void Draw();
	virtual bool PixelCheck(const FPoint2D &point) { return false; }
	virtual int Width() { return static_cast<int>(_width); }
	virtual int Height() { return static_cast<int>(_height); }
	virtual bool Command(const std::string &cmd) { return false; }
	virtual void GetBeautyList(BeautyList &list) const {}
	virtual void GetLocalMatrix(Matrix &m) { m.Unit(); }
	virtual void ReloadTextures(bool touch = false) = 0;
	virtual void ShiftU(float u) {}

	virtual std::string Type() const = 0;
	const std::string &UserString() const { return _userString; }
	void SetUserString(const std::string &str) { _userString = str; }
	virtual void UpdateScreenRect() {}

	virtual void SetPos(float x, float y);
	virtual void SetPos(const FPoint2D &pos) { SetPos(pos.x, pos.y); }
	virtual const FPoint2D &GetPos() const;

	virtual void SetAngle(float angle) { _angle = angle; }
	virtual float GetAngle() const { return _angle; }

	virtual void SetScale(float s);
	virtual void SetScale(float sx, float sy);
	virtual void SetScale(const FPoint2D &scale) { SetScale(scale.x, scale.y); }
	FPoint2D GetScale() const { return _scale; }
	
	virtual void SetAlpha(float alpha);
	virtual float GetAlpha() const;

	virtual void ShiftTo(float dx, float dy);// для перемещения на разных разрешениях экрана

	float &UserFloat() { return _scalar; }
	FPoint2D &UserPoint() { return _pointForAnything; }

    virtual GLTexture *GetTexture() { return NULL; }

    bool HasTag(const std::string &tag) const;
};

#endif//BEAUTY_BASE_H
