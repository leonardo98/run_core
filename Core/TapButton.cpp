#include "TapButton.h"
#include "Messager.h"
#include "Core.h"
#include "Sound.h"

void TapButton::Init(const LevelSet &levelSet, const char *id, bool invisible)
{
	BeautyList list;
	levelSet.SelectByUserStringHard(id, list);
	_touchArea = NULL;
	for(BeautyList::iterator i = list.begin(); i != list.end(); ++i)
	{
		if ((*i)->Type() == "ClickArea")
		{
			assert(_touchArea == NULL);
			_touchArea = *i;
		}
		else
		{
            (*i)->TweenCheck();
			_btns.push_back(*i);
			_poses.push_back(Poses());
			_poses.back().InitWithBeauty(*i);
		}
	}

	assert((invisible || _btns.size() > 0) && _touchArea != NULL);

	_isTouch = false;
}

TapButton::TapButton(const LevelSet &levelSet, const char *id, const char *recever, const char *message, bool invisible)
	: _visible(true)
	, _enable(true)
	, _message(message)
	, _eventReceiver(NULL)
	, _checked(true)
{
	Init(levelSet, id, invisible);

	_receiver = recever;
}

TapButton::TapButton(const LevelSet &levelSet, const char *id, EventListener *recever, const char *message, bool invisible)
	: _visible(true)
	, _enable(true)
	, _message(message)
	, _eventReceiver(recever)
	, _checked(true)
{
	Init(levelSet, id, invisible);
}

void TapButton::SetVisible(bool visible)
{
	_visible = visible;
	for (BeautyList::iterator b = _btns.begin(), e = _btns.end(); b != e; ++b)
	{
		(*b)->Visible() = _visible;
	}
}

void TapButton::SetVisibleByTag(const std::string &searchTag, const std::string &keyTag)
{
    for (auto &a: _btns)
    {
        if (a->HasTag(searchTag))
        {
            a->Visible() = a->HasTag(keyTag);
        }
    }
}

void TapButton::SetChecked(bool checked)
{
	_checked = checked;
	assert(_btns.size() > 1);
	
	if (!_checked)
		_btns.back()->SetAlpha(0.f);
	else
		_poses.back().Reset();
}

void TapButton::Command(const std::string &cmd)
{
	for (BeautyList::iterator b = _btns.begin(), e = _btns.end(); b != e; ++b)
	{
		(*b)->Command(cmd);
	}
}

bool TapButton::Check(const FPoint2D &pos)
{
	if (!_visible || !_enable)
	{
		return false;
	}
	bool oldValueIsTouch = _isTouch;
	_isTouch = _touchArea->PixelCheck(pos);
	if (!oldValueIsTouch && _isTouch)
	{
		static float MaxScale = Core::IniGetFloat("data/interface.xml", "interface", "button_scale", 1.05f);
		for (unsigned int i = 0; i < _btns.size(); ++i)
		{
			_btns[i]->AddTween(new TweenScale(_poses[i].scale * MaxScale), 0.1f);
		}
	}
	else if (oldValueIsTouch && !_isTouch)
	{
		for (unsigned int i = 0; i < _btns.size(); ++i)
		{
			_btns[i]->AddTween(new TweenScale(_poses[i].scale), 0.1f);
		}
	}
	return _isTouch;
}

bool TapButton::UnderPos(const FPoint2D &pos)
{
	return _enable && _visible && _touchArea->PixelCheck(pos);
}

void TapButton::Action() const
{
    SoundEffect("button");

	if (_eventReceiver == NULL)
		Messager::SendMessage(_receiver, _message);
	else 
		_eventReceiver->Event(_message);
}

void TapButton::EndTouch()
{
	if (_enable && _isTouch)
	{
        Action();
		CancelTouch();
	}
}

void TapButton::CancelTouch()
{
	if (_isTouch)
	{
		for (unsigned int i = 0; i < _btns.size(); ++i)
		{
			_btns[i]->ClearTweens();
			_btns[i]->AddTween(new TweenScale(_poses[i].scale), 0.075f);
		}
		_isTouch = false;
	}
}

void TapButton::Reset()
{
	_enable = true;
	_isTouch = false;
    for (BeautyList::iterator b = _btns.begin(), e = _btns.end(); b != e; ++b)
	{
		(*b)->ClearTweens();
	}
	for (unsigned int i = 0, e = _poses.size(); i != e; ++i)
	{
		_poses[i].Reset();
	}
}

void TapButton::SetEnable(bool v) 
{
	_enable = v; 
}


void TapButton::UpdatePos(BeautyBase *b)
{
	for (unsigned int i = 0, e = _poses.size(); i != e; ++i)
	{
		if (_poses[i].beauty == b)
		{
			_poses[i].InitWithBeauty(b);
			break;
		}
	}
}

void TapButton::TweenEvent()
{
	SetVisible(false);
}

void TapButton::HideTo(float x, float y, float time)
{
	Reset();
	_enable = false;
	FPoint2D shift(x, y);
    for (BeautyList::iterator b = _btns.begin(), e = _btns.end(); b != e; ++b)
	{
		(*b)->AddTween(new TweenPosition((*b)->GetPos() + shift), time);
		(*b)->AddTween(new TweenAlpha(0.f), time)->SetListener(this);
	}
}

void TapButton::ShowFrom(float x, float y, float time)
{
	SetVisible(true);
	Reset();
	_enable = true;
	FPoint2D shift(x, y);
    for (BeautyList::iterator b = _btns.begin(), e = _btns.end(); b != e; ++b)
	{
		FPoint2D pos((*b)->GetPos());
		(*b)->SetPos(pos + shift);
		(*b)->AddTween(new TweenPosition(pos), time, MotionType_JumpOut);

		float alpha = (*b)->GetAlpha();
		(*b)->SetAlpha(0.f);
		(*b)->AddTween(new TweenAlpha(alpha), time);		
	}
}

Beauty *TapButton::GetBeauty()
{
    for (BeautyList::iterator b = _btns.begin(), e = _btns.end(); b != e; ++b)
	{
		if ((*b)->Type() == "Beauty")
		{
			return static_cast<Beauty *>(*b);
		}
	}
	return NULL;
}

LinkToComplex* TapButton::GetLinkToComplex()
{
    for (BeautyList::iterator b = _btns.begin(), e = _btns.end(); b != e; ++b)
	{
		if ((*b)->Type() == "LinkToComplex")
		{
			return static_cast<LinkToComplex *>(*b);
		}
	}
	return NULL;
}

void TapButton::MoveTo(float x, float y)
{
	_touchArea->SetPos(_touchArea->GetPos().x + x, _touchArea->GetPos().y + y);
    for (BeautyList::iterator b = _btns.begin(), e = _btns.end(); b != e; ++b)
	{
		(*b)->SetPos((*b)->GetPos().x + x, (*b)->GetPos().y + y);
	}
	for (unsigned int i = 0, e = _poses.size(); i != e; ++i)
	{
		_poses[i].pos.x += x;
		_poses[i].pos.y += y;
	}
}

BeautyText *TapButton::GetBeautyText(float userFloat)
{
    for (BeautyList::iterator b = _btns.begin(), e = _btns.end(); b != e; ++b)
	{
		if ((*b)->Type() == "BeautyText" && fabs((*b)->UserFloat() - userFloat) < 1e-3f)
		{
			return static_cast<BeautyText *>(*b);
		}
	}
	return NULL;
}

AnimationArea *TapButton::GetAnimationArea()
{
    for (BeautyList::iterator b = _btns.begin(), e = _btns.end(); b != e; ++b)
	{
		if ((*b)->Type() == "Animation")
		{
			return static_cast<AnimationArea *>(*b);
		}
	}
	return NULL;
}

void TapButton::PopAttach(BeautyBase *b)
{
    if (_btns.size() && _btns.back() == b)
    {
        _btns.pop_back();
        _poses.pop_back();
    }
}

void TapButton::PushAttach(BeautyBase *b)
{
    _btns.push_back(b);
    _poses.push_back(Poses());
    _poses.back().InitWithBeauty(b);
}

bool TapButton::HasTag(const std::string &tag) const
{
    return _touchArea->HasTag(tag);
}


TapButton * TapButtonInterface::AddButton(const LevelSet &levelSet, const char *id, const char *receiver, const char *message, bool invisible)
{
	TapButton *btn = new TapButton(levelSet, id, receiver, message, invisible);
	_list.push_back(btn);
	return btn;
}

TapButton * TapButtonInterface::AddButton(const LevelSet &levelSet, const char *id, EventListener *receiver, const char *message, bool invisible)
{
	TapButton *btn = new TapButton(levelSet, id, receiver, message, invisible);
	_list.push_back(btn);
	return btn;
}

bool TapButtonInterface::OnTouchMove(const FPoint2D &pos)
{
	if (_state == touch_canceled)
	{
		return false;
	}
	bool result = false;
	for (TapButtonList::iterator i = _list.begin(), e = _list.end(); i != e; ++i)
	{
		if ((*i)->Check(pos))
		{
			result = true;
		}
	}
	return result;
}

void TapButtonInterface::OnTouchEnd()
{
	_state = touch_none;
	for (TapButtonList::iterator i = _list.begin(), e = _list.end(); i != e; ++i)
	{
		(*i)->EndTouch();
	}
}

void TapButtonInterface::TouchCancel()
{
	_state = touch_canceled;
	for (TapButtonList::iterator i = _list.begin(), e = _list.end(); i != e; ++i)
	{
		(*i)->CancelTouch();
	}
}

void TapButtonInterface::Reset()
{
	_state = touch_none;
	for (TapButtonList::iterator i = _list.begin(), e = _list.end(); i != e; ++i)
	{
		(*i)->Reset();
	}
}

void TapButtonInterface::Clear()
{
	_state = touch_none;
	for (TapButtonList::iterator i = _list.begin(), e = _list.end(); i != e; ++i)
	{
		delete  (*i);
	}
	_list.clear();
}

TapButtonInterface::~TapButtonInterface()
{
	Clear();
}

bool TapButtonInterface::UnderPos(const FPoint2D &pos)
{
	for (TapButtonList::iterator i = _list.begin(), e = _list.end(); i != e; ++i)
	{
		if ((*i)->UnderPos(pos))
		{
			return true;
		}
	}
	return false;
}

bool TapButtonInterface::OnKey(int key)
{
//    if (key == GK_ESCAPE)
//    {
//	    for (TapButtonList::iterator i = _list.begin(), e = _list.end(); i != e; ++i)
//	    {
//		    if ((*i)->HasTag("escape"))
//		    {
//                (*i)->Action();
//			    return true;
//		    }
//	    }
//    }
	return false;
}
