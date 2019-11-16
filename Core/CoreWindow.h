#ifndef CORE_WINDOW_H
#define CORE_WINDOW_H

#include "types.h"
#include "InputSystem.h" 
#include "TapButton.h"
#include "../Core/LevelSet.h"

class CoreWindow 
	: virtual public InputSystem
	, public EventListener
	, virtual public BeautyBase
{
public:
	virtual ~CoreWindow();

	virtual int Event(const std::string &msg);

	virtual bool IsMouseOver(const FPoint2D &mousePos) { return _isUnderMouse; }
	void SetDefUnderMouse(bool under) { _isUnderMouse = under; }

	// клик по элементу(нажатие левой кнопки мыши)
	virtual void OnMouseDown(const FPoint2D &mousePos);
	// отпустили кнопку мыши(неважно где)
	virtual void OnMouseUp();
	// движение мыши
	virtual void OnMouseMove(const FPoint2D &mousePos);

	bool SetText(const char *id, const std::string &text);
	bool SetTextById(const char *id, const std::string &textId);
	TapButton *SetButton(const char *id, const char *receiver, const char *message);
	TapButton *SetButton(const char *id, EventListener *receiver, const char *message);
	TapButton *SetCloseButton(const char *id);
	bool SelectPicture(const std::string &mask, const std::string &id);
	LevelSet * AddLayer(const std::string &name, const std::string &libId = "", bool animate = false);
	void Show(float delay = 0.f);
	void Hide();

	virtual void Draw();
	virtual void Update(float);

	virtual std::string Type() const { return "CoreWindow"; }
	virtual void ReloadTextures(bool touch = false) {}
	CoreWindow(const std::string &name, const std::string &libId, bool animate = false);

protected:
	
	float _delay;
	std::vector<LevelSet *> _interfaces;
	TapButtonInterface _buttons;
	bool _isUnderMouse;
	friend class Core;
};

#endif//CORE_WINDOW_H
