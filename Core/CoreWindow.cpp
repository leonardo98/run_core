#include "CoreWindow.h"
#include "Core.h"
#include "BeautyText.h"

CoreWindow::~CoreWindow()
{
	for (unsigned int i = 0; i < _interfaces.size(); ++i)
	{
		delete _interfaces[i];
	}
}

int CoreWindow::Event(const std::string &msg)
{
	if (msg == "close")
	{
		Hide();
		return true;
	}
	return false;
}

void CoreWindow::OnMouseDown(const FPoint2D &mousePos)
{
	if (_delay > 0.f || TweenPlayer::Action())
	{
		return;
	}
	_buttons.OnTouchMove(mousePos);
}

void CoreWindow::OnMouseUp()
{
	if (_delay > 0.f || TweenPlayer::Action())
	{
		return;
	}
	_buttons.OnTouchEnd();
}

void CoreWindow::OnMouseMove(const FPoint2D &mousePos)
{
	if (_delay > 0.f || TweenPlayer::Action())
	{
		return;
	}
	_buttons.OnTouchMove(mousePos);
}

void CoreWindow::Draw()
{
	if (_delay > 0.f)
	{
		return;
	}
	Render::PushMatrix();
	Render::PushColor();
	Render::MatrixMove(Render::ScreenWidth() / 2, Render::ScreenHeight() / 2);
	Render::MatrixMove(_pos);
	Render::MatrixRotate(_angle);
	Render::MatrixScale(_scale.x, _scale.y);
	Render::MulColor(_color);
	Render::MatrixMove(-Render::ScreenWidth() / 2, -Render::ScreenHeight() / 2);
	for (unsigned int i = 0; i < _interfaces.size(); ++i)
	{
		_interfaces[i]->Draw();
	}
	Render::PopColor();
	Render::PopMatrix();
}

void CoreWindow::Update(float dt)
{
	if (_delay > 0.f)
	{
		_delay -= dt;
	}
	else
	{
		TweenPlayer::Update(this, dt);
	}

	for (unsigned int i = 0; i < _interfaces.size(); ++i)
	{
		_interfaces[i]->Update(dt);
	}
}

bool CoreWindow::SetText(const char *id, const std::string &text)
{
	BeautyBase *tb;
	_interfaces.back()->SelectByUserStringHard(id, tb);
	if (tb)
	{
		BeautyText *beautyText = dynamic_cast<BeautyText *>(tb);
		assert(beautyText != NULL);
		beautyText->SetText(text);
		return true;
	}
	return false;
}

bool CoreWindow::SetTextById(const char *id, const std::string &textId)
{
	BeautyBase *tb;
	_interfaces.back()->SelectByUserStringHard(id, tb);
	if (tb)
	{
		BeautyText *beautyText = dynamic_cast<BeautyText *>(tb);
		assert(beautyText != NULL);
		beautyText->SetTextById(textId);
		return true;
	}
	log::write("not found beauty: %s", id);
	return false;
}

TapButton *CoreWindow::SetButton(const char *id, const char *receiver, const char *message)
{
	return _buttons.AddButton(*_interfaces.back(), id, receiver, message);
}

TapButton *CoreWindow::SetButton(const char *id, EventListener *receiver, const char *message)
{
	return _buttons.AddButton(*_interfaces.back(), id, receiver, message);
}

TapButton *CoreWindow::SetCloseButton(const char *id)
{
	return _buttons.AddButton(*_interfaces.back(), id, this, "close");
}

void CoreWindow::Show(float delay)
{
	_delay = delay;
	SetAlpha(0.f);
	AddTween(new TweenAlpha(1.f), 0.1f);
	AddTween(new TweenJump(), 0.25f);
}

void CoreWindow::Hide()
{
	AddTween(new TweenAlpha(0.f), 0.2f);
	AddTween(new TweenScale(1.3f), 0.2f);
	Core::RemoveObject(this, 0.2f);
}

LevelSet * CoreWindow::AddLayer(const std::string &name, const std::string &libId, bool animate)
{
	LevelSet *ls = new LevelSet();
	LevelSetManager::GetLevel(libId, name, *ls);
	_interfaces.push_back(ls);

	// сдвигаем в центр для экранов не 960х640
	if (Render::ScreenWidth() != 960)
    {
		FPoint2D shift(Render::ScreenWidth() - 960.f, Render::ScreenHeight() - 640.f);
		for (BeautyList::iterator i = _interfaces.back()->beauties.begin(), e = _interfaces.back()->beauties.end(); i != e; ++i)
		{
			(*i)->ShiftTo(shift.x * ((*i)->UserPoint().x + 1.f) / 2.f, shift.y * ((*i)->UserPoint().y + 1.f) / 2.f);
		}
	}
    if (animate)
    {
        for (BeautyList::iterator i = _interfaces.back()->beauties.begin(), e = _interfaces.back()->beauties.end(); i != e; ++i)
        {
            if ((*i)->Type() == "Animation")
            {
                (*i)->Command("loop");
            }
        }
    }
	return ls;
}

CoreWindow::CoreWindow(const std::string &name, const std::string &libId, bool animate)
	: _delay(0.f)
	, _isUnderMouse(true)
{
	AddLayer(name, libId, animate);
}

bool CoreWindow::SelectPicture(const std::string &mask, const std::string &id)
{
	BeautyList beauties;
	_interfaces.back()->SelectByUserString(mask, beauties);
	
	for (BeautyList::iterator i = beauties.begin(), e = beauties.end(); i != e; ++i)
	{
		(*i)->Visible() = ((*i)->UserString() == id);
	}
	return true;
}
