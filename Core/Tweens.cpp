#include "Tweens.h"
#include "BeautyBase.h"

Tween *TweenQueue::Add(Tween *t, float time, MotionType motionType) 
{
	assert(t != NULL && !(t->queue));
	if (time <= 0)
	{
		return NULL;
	}
	_queue.push_back(PlaySubTween(t, time, motionType)); 
	return t;
}

void TweenQueue::Clear()
{ 
	for (TweenList::iterator t = _queue.begin(); t != _queue.end(); ++t)
	{
		delete t->tween;
	}
	_queue.clear();
}

unsigned int TweenQueue::Size() { return _queue.size(); }

void TweenQueue::UpdatePosition(BeautyBase *beauty, float dt)
{
	if (_queue.size() == 0)
	{
		return;
	}
	PlaySubTween &play = _queue.front();
	play.Update( dt );
	if (play.timer < 1.f)
	{
		play.tween->UpdatePosition(beauty, play.Timer());
	}
	else
	{
		float overHead = (play.timer - 1.f) * play.timeFull;
		play.tween->UpdatePosition(beauty, 1.f);
		delete play.tween;
		_queue.pop_front();
		if (overHead > 0)
		{
			UpdatePosition(beauty, overHead);
		}
	}
}

void TweenAlpha::UpdatePosition(BeautyBase *beauty, float p) 
{
	if (_alphaStart < 0)
	{
		_alphaStart = beauty->GetAlpha();
	}
    beauty->SetAlpha(Math::lerp(_alphaStart, _alpha, p));
}

void TweenColor::UpdatePosition(BeautyBase *beauty, float p) 
{
	if (_rStart < 0)
	{
		_rStart = static_cast<float>((beauty->GetColor() & 0xFF0000) >> 16);
		_gStart = static_cast<float>((beauty->GetColor() & 0xFF00) >> 8);
		_bStart = static_cast<float>((beauty->GetColor() & 0xFF));
		_aStart = static_cast<float>((beauty->GetColor() & 0xFF000000) >> 24);
	}
	beauty->SetColor(static_cast<unsigned int>(Math::lerp(_rStart, _r, p)) << 16 
					| static_cast<unsigned int>(Math::lerp(_gStart, _g, p)) << 8 
					| static_cast<unsigned int>(Math::lerp(_bStart, _b, p)) 
					| static_cast<unsigned int>(Math::lerp(_aStart, _a, p)) << 24 );
}

void TweenPosition::UpdatePosition(BeautyBase *beauty, float p) 
{
	if (_notSet)
	{
		_notSet = false;
		_posStart = beauty->GetPos();
	}
	beauty->SetPos(Math::lerp(_posStart, _pos, p));
}

void TweenAngle::UpdatePosition(BeautyBase *beauty, float p) 
{
	if (_notSet)
	{
		_notSet = false;
		_posStart = beauty->GetAngle();
	}
	beauty->SetAngle(Math::lerp(_posStart, _pos, p));
}

void TweenScale::UpdatePosition(BeautyBase *beauty, float p) 
{
	if (_notSet)
	{
		_notSet = false;
		_scaleStart = beauty->GetScale();
	}
	FPoint2D s(Math::lerp(_scaleStart, _scale, p));
	beauty->SetScale(s.x, s.y);
}

void TweenJump::UpdatePosition(BeautyBase *beauty, float p) 
{
	if (_notSet)
	{
		_notSet = false;
		_scaleStart = beauty->GetScale();
	}
	FPoint2D s;
	if (p < 0.3f)
	{
		s = Math::lerp(_scaleStart, FPoint2D(1.3f, 1.3f), p / 0.3f);
	}
	else
	{
		s = Math::lerp(FPoint2D(1.3f, 1.3f), _scaleStart, (p - 0.3f) / 0.7f);
	}
	beauty->SetScale(s.x, s.y);
}

void TweenTimedCommand::UpdatePosition(BeautyBase *beauty, float p)
{
	if (p == 1 && _cmd.size())
	{
		beauty->Command(_cmd);
		_cmd.clear();
	}
}

Tween *TweenPlayer::AddTween(Tween *t, float time, MotionType motionType)
{
	_tweens.push_back(PlaySubTween(t, time, motionType)); 
	return t;
}

void TweenPlayer::Update(BeautyBase *beauty, float dt)
{
	for (TweenList::iterator i = _tweens.begin(); i != _tweens.end(); )
	{
		PlaySubTween &play = *i;
		if (play.tween->queue)
		{
			TweenQueue *queue = dynamic_cast<TweenQueue *>(play.tween);
			if (queue == NULL || queue->Size() == 0)
			{
				_removeAfterTweens = play.tween->removeAfterTweens;
				delete play.tween;
				i = _tweens.erase(i);
			}
			else
			{
				play.tween->UpdatePosition(beauty, dt);
				++i;
			}
		}
		else
		{
			play.Update( dt );
			
			if (play.timer >= 1.f) {
				if (play.tween->GetRepeat() == 0) {
					play.timer -= 1.f;
					if (play.tween->GetPingPong()) {
						play.tween->SetReverce(!play.tween->GetReverce());
					}
				} else if (play.tween->GetPingPong() && !play.tween->GetReverce()) {
					play.tween->SetReverce(true);
					play.timer -= 1.f;
				} else if (play.tween->GetPingPong() && play.tween->GetReverce() && play.tween->GetRepeat() > 1) {
					play.tween->Repeat(play.tween->GetRepeat() - 1);
					play.tween->SetReverce(false);
					play.timer -= 1.f;
				}
			}

			if (play.timer < 1.f)
			{
				if (play.tween->GetReverce()) {
					play.tween->UpdatePosition(beauty, 1.f - play.Timer());
				}
				else {
					play.tween->UpdatePosition(beauty, play.Timer());
				}
				++i;
			}
			else
			{
				play.tween->UpdatePosition(beauty, 1);
				_removeAfterTweens = play.tween->removeAfterTweens;
				delete play.tween;
				i = _tweens.erase(i);
			}
		}

	}
}

void TweenPlayer::ClearTweens()
{
	for (TweenList::iterator i = _tweens.begin(); i != _tweens.end(); ++i)
	{
		delete i->tween;
	}
	_tweens.clear();
}

void TweenPlayer::TweenCheck()
{
    assert(_tweens.size() == 0);
}
