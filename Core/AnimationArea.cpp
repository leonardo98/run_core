#include "AnimationArea.h"
#include "../Core/Core.h"
#include "../Core/Math.h"

AnimationArea::~AnimationArea() {
}

AnimationArea::AnimationArea(rapidxml::xml_node<> *xe) 
: BeautyBase(xe)
, _animationReplace(NULL)
{
	_lib = xe->first_attribute("file")->value();
	_id = xe->first_attribute("id")->value();
	_shiftX = atoi(xe->first_attribute("shiftX")->value());
	_shiftY = atoi(xe->first_attribute("shiftY")->value());
	_animation = My::AnimationManager::getAnimation(_id);
	const char *tmp = xe->first_attribute("start")->value();
	_time = tmp ? atof(tmp) : 0;
    assert(0.f <= _time && _time <= 1.f);
	_state = state_stop;
}

AnimationArea::AnimationArea(const AnimationArea &b)
: BeautyBase(b)
, _animationReplace(NULL)
{
	_lib = b._lib;
	_id = b._id;
	_animation = My::AnimationManager::getAnimation(_id);
	_time = b._time;
    assert(0.f <= _time && _time <= 1.f);
	_state = state_stop;
	_shiftX = b._shiftX;
	_shiftY = b._shiftY;
}

void AnimationArea::Draw() {
    FPoint2D a(- Width() / 2 * fabs(_scale.x), 0);
    FPoint2D b(Width() / 2 * fabs(_scale.x), 0);
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
    Render::GetCurrentMatrix().Mul(a);
    Render::GetCurrentMatrix().Mul(b);
    if (a.x > Render::ScreenWidth() || b.x < 0)
    {
        Render::PopMatrix();
        return;
    }
	Render::MatrixRotate(_angle);
	Render::MatrixScale(_scale.x, _scale.y);
	Render::MatrixMove(_shiftX, _shiftY);
	Render::PushColorAndMul(_color);
	if (_state != state_replace)
	{
		_animation->Draw(_time);
	}
	else
	{
		_animationReplace->Draw(_time);
	}
	Render::PopColor();
	Render::PopMatrix();
}

void AnimationArea::GetLocalMatrix(Matrix &m) 
{
	m.Unit();
	m.Move(_pos.x, _pos.y);
	m.Rotate(_angle);
	m.Scale(_scale.x, _scale.y);
	//m.Move(_shiftX, _shiftY);// (?) - не уверен что это шаг нужен
}


std::string AnimationArea::Type() const { 
	return "Animation"; 
}

void AnimationArea::Update(float dt) 
{
	BeautyBase::Update(dt);
	if (_state == state_loop
		|| _state == state_play
		|| _state == state_play_and_stop_at_last_frame
		|| _state == state_play_and_hide) {
		_time += dt / _animation->Time();
        assert(0.f <= _time);
		if (_time >= 1.f) {
			if (_state == state_play_and_stop_at_last_frame) {
				_time = 1.f;
				_state = state_last_frame;
			} else if (_state == state_play_and_hide) {
				_time = 0.f;
				_state = state_stop;
				Visible() = false;
			} else if (_state == state_play) {
				_time = 0.f;
				_state = state_stop;
			} else if (_state == state_loop) {
				while (_time >= 1.f) {
					_time -= 1.f;
				}
                assert(0.f <= _time && _time <= 1.f);
			} else {
				assert(false);
			}
		}
	}
	if (_state == state_replace) {
		_time += dt / _animationReplace->Time();
		if (_time >= 1.f) {
			_state = state_loop;
			_time = 0.f;
		}
	}
}

bool AnimationArea::Command(const std::string &cmd) {
	if (cmd == "stop") {
		_state = state_stop;
		return true;
	} else if (cmd == "play") {
		_state = state_play;
		_time = 0.f;
		return true;
	} else if (cmd == "reset") {
		_state = state_stop;
		_time = 0.f;
		return true;
	} else if (cmd == "play and stop") {
		_state = state_play_and_stop_at_last_frame;
		_time = 0.f;
		return true;
	} else if (cmd == "play and hide") {
		_state = state_play_and_hide;
		_time = 0.f;
		return true;
	} else if (cmd.find("time:") == 0) {
		float f = atof(cmd.substr(5).c_str());
		f = std::min(1.f, std::max(0.f, f));
		_time = f;
        assert(0.f <= _time && _time <= 1.f);
		return true;
	} else if (cmd == "loop") {
		_state = state_loop;
		return true;
	} else if (cmd.substr(0, 9) == "replace: ") {
		_time = 0.f;
		_state = state_replace;
		_animationReplace = My::AnimationManager::getAnimation(cmd.substr(9));
		return true;
	} else if (cmd.substr(0, 6) == "init: ") {
		_animation = My::AnimationManager::getAnimation(cmd.substr(6));
		return true;
	}
	return (cmd == "" || BeautyBase::Command(cmd));
}

void AnimationArea::ReloadTextures(bool touch)
{
    if (touch)
    {
        My::AnimationManager::UploadTexture(_id);
    }
}
