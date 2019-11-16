#ifndef BEAUTY_H
#define BEAUTY_H

#include "BeautyBase.h"

class Beauty : public BeautyBase
{
public:

	virtual ~Beauty();
	Beauty(const Beauty &b);
    Beauty(rapidxml::xml_node<> *xe);
    Beauty(GLTexture *);

	virtual void Draw();
	virtual void GetBeautyList(BeautyList &list) const;

	virtual std::string Type() const;

	virtual int Width();
	virtual int Height();

	const std::string &FileName() const {return _fileName;}
	virtual void SetPos(float x, float y);
	virtual void ShiftTo(float dx, float dy);
	virtual void SetScale(float sx, float sy);
	virtual void SetAngle(float angle);

	virtual void ReloadTextures(bool touch = false);
    virtual GLTexture *GetTexture() { return _sprite; }
    virtual void SetTexture(GLTexture *tex) { _sprite = tex; }

	virtual void GetLocalMatrix(Matrix &m) { m = _localPos; }

private:
	std::string _fileName;
    GLTexture *_sprite;
	Matrix _localPos;
};


#endif//BEAUTY_H
