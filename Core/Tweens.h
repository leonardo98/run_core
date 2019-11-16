#ifndef TWEENS_H
#define TWEENS_H

#include "../Core/Math.h"
#include "../Core/Object.h"

#include <list>

class BeautyBase;

class TweenEventListener
{
public:
	virtual void TweenEvent() {}
};

class Tween
{
private:
	TweenEventListener *listener;
	bool removeAfterTweens;
	friend class TweenPlayer;
	bool _pingPong;
	int _repeat;
	bool _reverce;
public:
	void RemoveAtEnd(bool b) { removeAfterTweens = b; }
	void SetListener(TweenEventListener *l) { listener = l; }
	const bool queue;
	Tween(bool Queue = false) : queue(Queue) , _repeat(1), _reverce(false), _pingPong(false), removeAfterTweens(false), listener(NULL) {}
	virtual ~Tween() { if (listener) listener->TweenEvent(); }
	virtual void Init() {}
	virtual void UpdatePosition(BeautyBase *beauty, float ) {}
	Tween * PingPong(bool b) { _pingPong = b; return this; }
	Tween * Repeat(int r) { _repeat = r; return this; }
	int GetRepeat() { return _repeat; }
	bool GetPingPong() {  return _pingPong; }
	bool GetReverce() { return _reverce; }
	void SetReverce(bool b) { _reverce = b; }
};

enum MotionType
{
	MotionType_Linear,
	MotionType_Fast,
	MotionType_JumpOut,
	MotionType_Slow
};

struct PlaySubTween
{
	Tween *tween;
	float timeFull;
	float timer;
	MotionType motion;
	PlaySubTween(Tween *t, float time, MotionType motionType)
		: tween(t)
		, timeFull(time)
		, timer(0.f)
		, motion(motionType)
	{}
	void Update(float dt) 
	{
		timer = timer + dt / timeFull;
	}
	float Timer()
	{
		if (motion == MotionType_Linear)
		{
			return timer;
		}
		if (motion == MotionType_Slow)
		{
			return sin(M_PI / 2 * timer);
		}
		if (motion == MotionType_Fast)
		{
			return 1 - cos(M_PI / 2 * timer);
		}
		if (motion == MotionType_JumpOut)
		{
			if (timer < 0.5f)
				return 1.15f * sin(M_PI * timer);
			else
				return 1.f + 0.15f * sin(M_PI * timer);
		}
		assert(false);
		return -1.f;
	}
};

typedef std::list<PlaySubTween> TweenList;

class TweenQueue : public Tween
{
private:
	TweenList _queue;
public:
	TweenQueue() : Tween(true) {}
	Tween *Add(Tween *t, float time, MotionType motionType = MotionType_Linear);
	void Clear();
	unsigned int Size();
	virtual void UpdatePosition(BeautyBase *beauty, float dt);
};

class TweenAlpha : public Tween
{
private:
	float _alpha;
	float _alphaStart;
public:
	TweenAlpha(float alpha)
		: _alpha(alpha)
		, _alphaStart(-1)
	{}
	virtual void UpdatePosition(BeautyBase *beauty, float p);
};

class TweenColor : public Tween
{
private:
	float _r;
	float _g;
	float _b;
	float _a;
	float _rStart;
	float _gStart;
	float _bStart;
	float _aStart;
public:
    TweenColor(unsigned int color)
		: _r(static_cast<float>((color & 0xFF0000) >> 16))
		, _g(static_cast<float>((color & 0xFF00) >> 8))
		, _b(static_cast<float>((color & 0xFF)))
		, _a(static_cast<float>((color & 0xFF000000) >> 24))
		, _rStart(-1.f)
	{}
	virtual void UpdatePosition(BeautyBase *beauty, float p);
};

class TweenEmpty : public Tween
{
public:
	TweenEmpty() {}
	virtual void UpdatePosition(BeautyBase *beauty, float p) {}
};

class TweenPosition : public Tween
{
private:
	FPoint2D _pos;
	FPoint2D _posStart;
	bool _notSet;
public:
	TweenPosition(const FPoint2D &pos)
		: _pos(pos)
		, _notSet(true)
	{}
	virtual void UpdatePosition(BeautyBase *beauty, float p);
};

class TweenAngle : public Tween
{
private:
	float _pos;
	float _posStart;
	bool _notSet;
public:
	TweenAngle(float pos)
		: _pos(pos)
		, _notSet(true)
	{}
	virtual void UpdatePosition(BeautyBase *beauty, float p);
};

class TweenTimedCommand : public Tween
{
private:
	std::string _cmd;
public:
	TweenTimedCommand(const std::string &cmd)
		: _cmd(cmd)
	{}
	virtual void UpdatePosition(BeautyBase *beauty, float p);
};

class TweenScale : public Tween
{
private:
	FPoint2D _scale;
	FPoint2D _scaleStart;
	bool _notSet;
public:
	TweenScale(const FPoint2D &scale)
		: _scale(scale)
		, _notSet(true)
	{}
	TweenScale(float scale)
		: _scale(scale, scale)
		, _notSet(true)
	{}
	virtual void UpdatePosition(BeautyBase *beauty, float p);
};

class TweenJump : public Tween
{
private:
	FPoint2D _scaleStart;
	bool _notSet;
public:
	TweenJump()
		: _notSet(true)
	{}
	virtual void UpdatePosition(BeautyBase *beauty, float p);
};

class TweenPlayer : virtual public Object
{
private:
	TweenList _tweens;
public:
	virtual ~TweenPlayer() { ClearTweens(); }
	TweenPlayer() {}
	Tween * AddTween(Tween *t, float time = 0, MotionType motionType = MotionType_Linear);
	void Update(BeautyBase *beauty, float dt);
	void ClearTweens();
    void TweenCheck();
	bool Action() { return _tweens.size() > 0; }
};

#endif//TWEENS_H
