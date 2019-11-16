#ifndef CONTENT_SCROLLER_H
#define CONTENT_SCROLLER_H

#include "../Helpers/Counter.h"
#include "Render.h"
#include "CoreTriple.h"

#include <list>

struct TimedPoint
{
	TimedPoint(float v) : time(0.f), pos(v) {}
	float time;
	float pos;
};

class ScrollIndicator
{
public:
	ScrollIndicator();
    void InitVert(GLTexture *, GLTexture *);
	void DrawVert(float x, float y, float area);
	void Update(float dt, bool);

private:
	CoreQuad _backUp;
	CoreQuad _back;
	CoreQuad _backDn;

	CoreQuad _frontUp;
	CoreQuad _front;
	CoreQuad _frontDn;
	int _heightBack;
	float _alpha;

protected:
	bool _moving;
	float _position;
	float _speed;
	float _bound;
	float _maxShift;
};

class ContentScroller : public ScrollIndicator
{
private:
	Counter _counter;
	int _direction;
	bool _mouseDown;
	float _mousePos;
	float _timerMouseDown;
	float _positionSoft;
	float _maxSoftSpeed;
	const float MOVE_ACTION_TIME;
	std::list<TimedPoint> _points;
public:
	ContentScroller();
	void Init(float maxShift, float bound);
	bool Update(float);
	float GetPosition() { return _position; }
	float GetPositionSoft() { return _positionSoft; }
	void SetMaxSoftSpeed(float speed) { _maxSoftSpeed = speed; }
	float GetSafePosition();
	void SetPosition(float v);
	float Clamp(float v);

	void SetSafePosition(float v);
	void InitCounter();
	void MouseDown(float);
	void MouseMove(float, bool force = false);
	void MouseUp();
	bool Moving() { return _moving; }
	void AddConent(float a);
};

class ScaleContentScroller
{
private:
	ContentScroller _xScroll;
	ContentScroller _yScroll;
	ContentScroller _scale;
    FPoint2D _zoomPoint;
    FPoint2D _fakeMousePos;
	int _contentWidth;
	int _contentHeight;
	int _boundWidth;
	int _boundHeight;
	int _viewWidth;
	int _viewHeight;
    bool _scaling;

public:

	float GetX() { return _xScroll.GetPosition(); }
	float GetY() { return _yScroll.GetPosition(); }

	void SetX(float x, bool checkRange = false);
	void SetY(float y, bool checkRange = false);

    FPoint2D ScreenToArea(const FPoint2D &pos);
    FPoint2D AreaToScreen(const FPoint2D &pos);
	
	void Init(int width, int boundW, int height, int boundH, int viewWidth, int viewHeight);
	
	void MouseDown(const FPoint2D &pos);
	void MouseMove(const FPoint2D &pos, bool force = false);
	void MouseUp();

	void StartScale(float);
	void Scale(float);
	void EndScale();

	bool Update(float);
	
	void SetScaleCenter(const FPoint2D &pos);

	float Value() { return (1 + _scale.GetPosition() / 100); }

    ScaleContentScroller() : _scaling(false) {}
};

class InfinitiveContentScroller
{
private:
	bool _moving;
	float _position;
	float _speed;
	int _direction;
	bool _mouseDown;
	float _mousePos;
	float _timerMouseDown;
	const float MOVE_ACTION_TIME;
	std::list<TimedPoint> _points;
public:
	InfinitiveContentScroller();
	bool Update(float);
	float GetPosition() { return _position; }
	void SetPosition(float v);

	void MouseDown(float);
	void MouseMove(float, bool force = false);
	void MouseUp();
	bool Moving() { return _moving; }
};

#endif//CONTENT_SCROLLER_H
