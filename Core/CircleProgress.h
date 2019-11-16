#ifndef CIRCLE_PROGRESS_H
#define CIRCLE_PROGRESS_H

#include "types.h"
#include "CoreTriple.h"
#include "Render.h"
#include "../Core/Beauty.h"

class CircleProgress 
	: public BeautyBase
{
public:
	virtual void ReloadTextures(bool touch = false) {}
	virtual std::string Type() const
	{
		return "CircleProgress";
	}
	CircleProgress(Beauty *, bool revers = false);
	void SetProgress(float p) { _value = p; }
	virtual void Draw();
	void Draw(float p)
	{
		_value = p;
		Draw();
	}
private:
	float _value;
    void Init(GLTexture *);
	std::vector<CoreTriple> _cover;
    FPoint2D _center;
	int _filled;
    GLTexture *_origin;
	bool _revers;
};

#endif//CIRCLE_PROGRESS_H
