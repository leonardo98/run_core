#ifndef ANIMATION_AREA_H
#define ANIMATION_AREA_H

#include "../2dga_api/Animation.h"
#include "BeautyBase.h"

class AnimationArea : public BeautyBase
{
public:

	virtual ~AnimationArea();
	AnimationArea(const AnimationArea &b);
	AnimationArea(rapidxml::xml_node<> *xe);

	virtual void Draw();
	virtual void Update(float dt);
	virtual bool Command(const std::string &cmd = "");
	virtual void GetLocalMatrix(Matrix &m);

	virtual std::string Type() const;

	const std::string &Name() const { return _id; }

	float ShiftX() const { return static_cast<float>(_shiftX); }
	float ShiftY() const { return static_cast<float>(_shiftY); }
	
	enum State {
		state_stop,
		state_last_frame,
		state_play,
		state_play_and_stop_at_last_frame,
		state_loop,
		state_play_and_hide,
		state_replace
	};

private:
	//Matrix _drawMatrix;
	std::string _id;
	std::string _lib;
	My::Animation *_animation;
	My::Animation *_animationReplace;
	State _state;
	float _time;

protected:
	int _shiftX;
	int _shiftY;
public:
	State GetState() { return _state; }
	My::Animation *GetAnimation() { return _animation; }
	const std::string &GetAnimationName() { return _id; }
	float GetAnimationPosition() { return _time; }
    virtual void ReloadTextures(bool touch = false);
};


#endif//BEAUTY_H