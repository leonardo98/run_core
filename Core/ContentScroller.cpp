#include "ContentScroller.h"
#include "types.h"
#include <algorithm>
#include "Math.h"

ScrollIndicator::ScrollIndicator()
{
	_alpha = 0.f;
	_back.tex = NULL;
	_front.tex = NULL;

}

void ScrollIndicator::InitVert(GLTexture *back, GLTexture *front)
{
	assert(back);
	assert(front);

    _backUp.tex = back;
//	_backUp.color[3] = _backUp.color[2] = _backUp.color[1] = _backUp.color[0] = 0xFFFFFFFF;
	_backUp.uv[0].x = 0; _backUp.uv[0].y = 0;
	_backUp.uv[1].x = 1; _backUp.uv[1].y = 0;
	_backUp.uv[2].x = 1; _backUp.uv[2].y = 0.333f;
	_backUp.uv[3].x = 0; _backUp.uv[3].y = 0.333f;
    _back.tex = back;
//	_back.color[3] = _back.color[2] = _back.color[1] = _back.color[0] = 0xFFFFFFFF;
	_back.uv[0].x = 0; _back.uv[0].y = 0.333f;
	_back.uv[1].x = 1; _back.uv[1].y = 0.333f;
	_back.uv[2].x = 1; _back.uv[2].y = 0.666f;
	_back.uv[3].x = 0; _back.uv[3].y = 0.666f;
    _backDn.tex = back;
//	_backDn.color[3] = _backDn.color[2] = _backDn.color[1] = _backDn.color[0] = 0xFFFFFFFF;
	_backDn.uv[0].x = 0; _backDn.uv[0].y = 0.666f;
	_backDn.uv[1].x = 1; _backDn.uv[1].y = 0.666f;
	_backDn.uv[2].x = 1; _backDn.uv[2].y = 1.f;
	_backDn.uv[3].x = 0; _backDn.uv[3].y = 1.f;

	_heightBack = back->Height() * 0.666f;

    _frontUp.tex = front;
//	_frontUp.color[3] = _frontUp.color[2] = _frontUp.color[1] = _frontUp.color[0] = 0xFFFFFFFF;
	_frontUp.uv[0].x = 0; _frontUp.uv[0].y = 0;
	_frontUp.uv[1].x = 1; _frontUp.uv[1].y = 0;
	_frontUp.uv[2].x = 1; _frontUp.uv[2].y = 0.333f;
	_frontUp.uv[3].x = 0; _frontUp.uv[3].y = 0.333f;
    _front.tex = front;
//	_front.color[3] = _front.color[2] = _front.color[1] = _front.color[0] = 0xFFFFFFFF;
	_front.uv[0].x = 0; _front.uv[0].y = 0.333f;
	_front.uv[1].x = 1; _front.uv[1].y = 0.333f;
	_front.uv[2].x = 1; _front.uv[2].y = 0.666f;
	_front.uv[3].x = 0; _front.uv[3].y = 0.666f;
    _frontDn.tex = front;
//	_frontDn.color[3] = _frontDn.color[2] = _frontDn.color[1] = _frontDn.color[0] = 0xFFFFFFFF;
	_frontDn.uv[0].x = 0; _frontDn.uv[0].y = 0.666f;
	_frontDn.uv[1].x = 1; _frontDn.uv[1].y = 0.666f;
	_frontDn.uv[2].x = 1; _frontDn.uv[2].y = 1.f;
	_frontDn.uv[3].x = 0; _frontDn.uv[3].y = 1.f;

	for (unsigned int i = 0; i < 4; ++i)
	{
		_backUp.v[i].x = _backUp.uv[i].x * back->Width(); 
		_backUp.v[i].y = _backUp.uv[i].y * back->Height(); 
		_back.v[i].x = _back.uv[i].x * back->Width(); 
		_back.v[i].y = _back.uv[i].y * back->Height(); 
		_backDn.v[i].x = _backDn.uv[i].x * back->Width(); 
		_backDn.v[i].y = _backDn.uv[i].y * back->Height(); 

		_frontUp.v[i].x = _frontUp.uv[i].x * back->Width(); 
		_frontUp.v[i].y = _frontUp.uv[i].y * back->Height(); 
		_front.v[i].x = _front.uv[i].x * back->Width(); 
		_front.v[i].y = _front.uv[i].y * back->Height(); 
		_frontDn.v[i].x = _frontDn.uv[i].x * back->Width(); 
		_frontDn.v[i].y = _frontDn.uv[i].y * back->Height(); 
	}
}

void ScrollIndicator::DrawVert(float x, float y, float area)
{
	if (_alpha < 1e-3f)
	{
		return;
	}
	Render::PushMatrix();
	Render::MatrixMove(x, y);
	Render::DrawQuad(_backUp);
	_backDn.v[1].y = _backDn.v[0].y = _back.v[3].y = _back.v[2].y = area + _backUp.v[2].y - _heightBack;
	Render::DrawQuad(_back);
	_backDn.v[3].y = _backDn.v[2].y = area;
	Render::DrawQuad(_backDn);
	Render::PopMatrix();

	float innerArea = area * area / (area - _maxShift);
	Render::PushMatrix();
	float f = (_position / _maxShift) * (area - innerArea);
	if (f < 0)
	{
		Render::MatrixMove(x, y);
		innerArea += f;
	}
	else if (f + innerArea > area)
	{
		Render::MatrixMove(x, y + f);
		innerArea = area - f;
	}
	else
	{
		Render::MatrixMove(x, y + f);
	}
	Render::DrawQuad(_frontUp);
	_frontDn.v[1].y = _frontDn.v[0].y = _front.v[3].y = _front.v[2].y = innerArea + _frontUp.v[2].y - _heightBack;
	Render::DrawQuad(_front);
	_frontDn.v[3].y = _frontDn.v[2].y = innerArea;
	Render::DrawQuad(_frontDn);
	Render::PopMatrix();
}

void ScrollIndicator::Update(float dt, bool changes)
{
	if (changes || _moving)
	{
		if (_alpha < 1)
		{
			_alpha += dt / 0.15f;
			if (_alpha > 1)
			{
				_alpha = 1.f;
			}
		}
	}
	else if (_alpha > 0)
	{
		_alpha -= dt / 0.4f;
	}
}

ContentScroller::ContentScroller()
	: MOVE_ACTION_TIME(0.2f)
{
	Init(0, 0);
}

void ContentScroller::Init(float maxShift, float bound)
{
	_position = 0;
	_bound = bound;
    _maxShift = std::min( - maxShift, 0.f);
	_mouseDown = false;
	_mousePos = 0;
	_timerMouseDown = 0.f;
	_moving = false;
	_speed = 0.f;
	_maxSoftSpeed = 999999.f;
	_positionSoft = 0.f;
	_counter.Clear();
}

bool ContentScroller::Update(float dt)
{
	bool changes = false;
	if (_mouseDown)
	{
		if (dt > 0.07f) {
			_timerMouseDown += 0.07f;
		} else {
			_timerMouseDown += dt;
		}
		if (_timerMouseDown > MOVE_ACTION_TIME)
		{
			MouseMove(_mousePos);
		}

		for (std::list<TimedPoint>::iterator i = _points.begin(); i != _points.end(); )
		{
			(*i).time += dt;
			if (i->time > 0.1f)// учитываем точки за это время
			{
				i = _points.erase(i);
			}
			else
			{
				++i;
			}
		}
	}
	if (_speed > 0)
	{
		float acc = 1500.f;
		if (_direction == 1)
		{
			if (_position > 0)
			{
				acc = 10000;
			}
			else if (-_position < _bound)
			{
				acc = Math::lerp(3000.f, acc, -_position / _bound);
			}
		}
		else
		{
			if (_position < _maxShift)
			{
				acc = 10000;
			}
			else if (_position < _maxShift + _bound)
			{
				acc = Math::lerp(3000.f, acc, (_position - _maxShift) / _bound);
			}
		}
		if (_mouseDown)
		{
			acc *= 5; 
		}
		float s = acc * dt;
		if (_speed < s)
		{
			_speed = 0;
			InitCounter();
		}
		else
		{
			_speed -= s;
		}	
		_position += _direction * _speed * dt;
		if (_position > _bound || _position < _maxShift - _bound) 
		{
			_speed = 0.f;
			_position = std::min(_bound, std::max(_maxShift - _bound, _position));
			InitCounter();
		}
		changes = true;
	}
	else if (_counter.Action()) {
		_counter.Update(dt);
		if (_counter.Done()) {
			if (_position > 0.f) {
				_position = 0.f;
			} else if (_position < _maxShift) {
				_position = _maxShift;
			}
			ScrollIndicator::Update(dt, true);
			return true;
		}
		float oldValue = _position;
		_position = _counter.GetUserValue() + (1.f - sinf(_counter.Progress() * static_cast<float>(M_PI_2))) * (_position - _counter.GetUserValue());
		changes |= fabs(oldValue - _position) > 0.01f;
		if (!changes)
		{
			_counter.Clear();
			if (_position > 0.f) {
				_position = 0.f;
			} else if (_position < _maxShift) {
				_position = _maxShift;
			}
			ScrollIndicator::Update(dt, true);
			return true;
		}
	}
	ScrollIndicator::Update(dt, changes);
	if (fabs(_positionSoft - _position) <= _maxSoftSpeed * dt)
	{
		_positionSoft = _position;
	}
	else
	{
		_positionSoft = _positionSoft + (_position - _positionSoft) / fabs(_position - _positionSoft) * _maxSoftSpeed * dt;
	}
	return changes;
}

void ContentScroller::MouseDown(float mousePos)
{
	_moving = false;
	_mouseDown = true;
	_timerMouseDown = 0.f;

	_mousePos = mousePos;
	_counter.Init(0.f);

	_points.clear();
	_points.push_back(TimedPoint(_position));
}

void ContentScroller::MouseMove(float mousePos, bool force)
{
	if (force 
		|| (_mouseDown && (_moving || (!_moving && fabs(mousePos - _mousePos) > 15.f) || _timerMouseDown > MOVE_ACTION_TIME))) 
	{
		_moving = true;
		float step = (mousePos - _mousePos);
		float scale = 1;
		if (step < 0 && _position < _maxShift - _bound / 2)
		{
			scale = std::max(0.f, (_position - (_maxShift - _bound)) / (_bound / 2));
		}
		else if (step > 0 && _position > _bound / 2)
		{
			scale = std::max(0.f, (_bound - _position) / (_bound / 2));
		}
		step *= scale;//(scale * scale);
		_position += step;
		_position = std::min(_bound, std::max(_maxShift - _bound, _position));
		_mousePos = mousePos;
		_points.push_back(TimedPoint(_position));

		_speed = 0.f;
		_counter.Clear();
	}
}

void ContentScroller::MouseUp()
{
	if (_mouseDown && !_moving) {
		_mouseDown = false;
	} else {
		_moving = false;
		_mouseDown = false;
		if (_points.size() >= 2)
		{
			_speed = fabs(_points.back().pos - _points.front().pos) / ((_points.front().time - _points.back().time));
			_direction = Math::sign((_points.back().pos - _points.front().pos));
			_points.clear();
			if (_speed < 100)
			{
				_speed = 0.f;
			}
		}
	}
	InitCounter();
}

void ContentScroller::InitCounter()
{
	if (_position > 0.f)
	{
		_counter.Init(1.5f);
		_counter.SetUserValue(0.f);
	}
	else if (_position < _maxShift)
	{
		_counter.Init(1.5f);
		_counter.SetUserValue(_maxShift);
	}
}

float ContentScroller::Clamp(float v)
{
	return std::min(0.f, std::max(_maxShift, v));
}

void ContentScroller::SetPosition(float v) {
	_position = v; 
	_position = std::min(_bound, std::max(_maxShift - _bound, _position));
	_positionSoft = _position;
}

void ContentScroller::SetSafePosition(float v) {
	_position = v; 
	float safeBound = _maxShift > 0 ? _maxShift / 2 : 0;
	_position = std::min(safeBound, std::max(_maxShift - safeBound, _position));
	_positionSoft = _position;
}

void ContentScroller::AddConent(float a) { 
	_maxShift -= a; 
	InitCounter();
} 

float ContentScroller::GetSafePosition() {
	if (_maxShift > 0) 
	{
		return 0.f;
	} 
	return std::min(0.f, std::max(_maxShift, _position));
}

void ScaleContentScroller::SetX(float x, bool checkRange) 
{
	if (checkRange)
	{
		x = _xScroll.Clamp(x);
	}
	_xScroll.SetPosition(x);
}

void ScaleContentScroller::SetY(float y, bool checkRange) 
{ 
	if (checkRange)
	{
		y = _yScroll.Clamp(y);
	}
	_yScroll.SetPosition(y);
}

void ScaleContentScroller::Init(int width, int boundW, int height, int boundH, int viewWidth, int viewHeight)
{
	float scaleMin = Render::ScreenWidth() / 2048.f;
	_scale.Init((1 - scaleMin) * 100, 8);
	_xScroll.Init(width - viewWidth, boundW);
	_yScroll.Init(height - viewHeight, boundH);

	_contentWidth = width;
	_contentHeight = height;
	_boundWidth = boundW;
	_boundHeight = boundH;
	_viewWidth = viewWidth;
	_viewHeight = viewHeight;
}

bool ScaleContentScroller::Update(float dt)
{
	_xScroll.Update(dt);
	_yScroll.Update(dt);

	if (_scale.Update(dt) &&  Value() > 1.f)
	{
		_xScroll.SetPosition(_zoomPoint.x / Value() - _fakeMousePos.x);
		_yScroll.SetPosition(_zoomPoint.y / Value() - _fakeMousePos.y);
		return true;
	}
	return false;
}

void ScaleContentScroller::MouseDown(const FPoint2D &pos)
{
	_xScroll.MouseDown(pos.x / Value());
	_yScroll.MouseDown(pos.y / Value());
}

void ScaleContentScroller::MouseMove(const FPoint2D &pos, bool force)
{
	_xScroll.MouseMove(pos.x / Value(), force);
	_yScroll.MouseMove(pos.y / Value(), force);
}

void ScaleContentScroller::MouseUp()
{
	_xScroll.MouseUp();
	_yScroll.MouseUp();
}

FPoint2D ScaleContentScroller::ScreenToArea(const FPoint2D &pos)
{
	FPoint2D r(pos);
	r = r / Value();
	r.x -= _xScroll.GetPosition();
	r.y -= _yScroll.GetPosition();
	return r;
}

FPoint2D ScaleContentScroller::AreaToScreen(const FPoint2D &pos)
{
	FPoint2D r(pos);
	r.x += _xScroll.GetPosition();
	r.y += _yScroll.GetPosition();
	r = r * Value();
	return r;
}

void ScaleContentScroller::SetScaleCenter(const FPoint2D &pos) 
{ 
	_zoomPoint = pos;
}

void ScaleContentScroller::Scale(float v)
{
	_scale.MouseMove((v - 1) * 100, true);

	_xScroll.SetPosition(_zoomPoint.x / Value() - _fakeMousePos.x);
	_yScroll.SetPosition(_zoomPoint.y / Value() - _fakeMousePos.y);
}

void ScaleContentScroller::StartScale(float v)
{
	_scale.MouseDown((v - 1) * 100);
	_fakeMousePos = ScreenToArea(_zoomPoint);

	_xScroll.AddConent(10000);
	_yScroll.AddConent(10000);
    
    _scaling = true;
}

void ScaleContentScroller::EndScale()
{
	_scale.MouseUp();

	if (_scaling)
	{
		float value = (1 + _scale.GetSafePosition() / 100);
		float tmp = _xScroll.GetPosition();
		_xScroll.Init(std::max(1.f, _contentWidth /** Value() */- _viewWidth / value), _boundWidth);
		_xScroll.SetPosition(tmp);
		_xScroll.InitCounter();

		tmp = _yScroll.GetPosition();
		_yScroll.Init(_contentHeight /** Value() */- _viewHeight / value, _boundHeight);
		_yScroll.SetPosition(tmp);
		_yScroll.InitCounter();

		if (Value() > value)
		{
			// todo: slide back a little
		}
        _scaling = false;
	}
}

InfinitiveContentScroller::InfinitiveContentScroller()
	: MOVE_ACTION_TIME(0.2f)
{
	_position = 0;
	_mouseDown = false;
	_mousePos = 0;
	_timerMouseDown = 0.f;
	_moving = false;
	_speed = 0.f;
}

bool InfinitiveContentScroller::Update(float dt)
{
	bool changes = false;
	if (_mouseDown)
	{
		_timerMouseDown += dt;
		if (_timerMouseDown > MOVE_ACTION_TIME)
		{
			MouseMove(_mousePos);
		}

		for (std::list<TimedPoint>::iterator i = _points.begin(); i != _points.end(); )
		{
			(*i).time += dt;
			if (i->time > 0.1f)// учитываем точки за это время
			{
				i = _points.erase(i);
			}
			else
			{
				++i;
			}
		}
	}
	if (_speed > 0)
	{
		_speed *= dt;
		if (_speed > 1.f)
		{
			_position += _direction * _speed * dt;
			changes = true;
		}
		else
			_speed = 0.f;
	}
	return changes;
}

void InfinitiveContentScroller::MouseDown(float mousePos)
{
	_moving = false;
	_mouseDown = true;
	_timerMouseDown = 0.f;

	_mousePos = mousePos;

	_points.clear();
	_points.push_back(TimedPoint(_position));
}

void InfinitiveContentScroller::MouseMove(float mousePos, bool force)
{
	if (force 
		|| (_mouseDown && (_moving || (!_moving && fabs(mousePos - _mousePos) > 15.f) || _timerMouseDown > MOVE_ACTION_TIME))) 
	{
		_moving = true;
		float step = (mousePos - _mousePos);
		_position += step;
		_mousePos = mousePos;
		_points.push_back(TimedPoint(_position));

		_speed = 0.f;
	}
}

void InfinitiveContentScroller::MouseUp()
{
	if (_mouseDown && !_moving) {
		_mouseDown = false;
	} else {
		_moving = false;
		_mouseDown = false;
		if (_points.size() >= 2)
		{
			_speed = fabs(_points.back().pos - _points.front().pos) / ((_points.front().time - _points.back().time));
			_direction = Math::sign((_points.back().pos - _points.front().pos));
			_points.clear();
			if (_speed < 100)
			{
				_speed = 0.f;
			}
		}
	}
}

void InfinitiveContentScroller::SetPosition(float v) {
	_position = v; 
}
