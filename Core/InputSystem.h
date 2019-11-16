// InputSystem.h: interface for the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYENGINE_INPUTSYSTEM_H
#define MYENGINE_INPUTSYSTEM_H

#include "types.h"
#include "Object.h"

#include "FPoint2D.h"

#include <set>
#include <vector>

//
// надстройка над системой ввода,
// если нужно обрабатывать событи€ мыши - наследуемс€ от этого класса
// если нет - от Object.h
//

class InputSystem 
	: virtual public Object  
{
private:
	static float DOUBLE_CLICK_TIME; // максимальное врем€ на второй клик - если врем€ больше - событие "аннулируетс€"
	static float LONG_TAP_EPS; // погрешность на случайные движени€ мыши во врем€ нажати€, если движение более сильное - событие "аннулируетс€"
	static float LONG_TAP_TIME; // минимальное врем€ на вызов событи€ - если врем€ меньше - событие "аннулируетс€"
	static float _timeCounter; // счетчик времени(отсчитывает врем€ либо с последнего событи€ нажати€ или отпускани€ кнопки мыши
	static bool _longTap; // true - если ловим событие "ƒлинный “ап"
	static bool _doubleClick; // true - если ловим это событие "ƒвойной клик"
	static FPoint2D _mousePos; // дл€ отладки масштабировани€ только 
	static std::vector<FPoint2D> _firstScalePoint;
	static FPoint2D _tapPos;
	static FPoint2D _scalePos;
	static InputSystem *_locked;// пишу того кто вз€лс€ обрабатывать клик(нажатие/отпускание) - остальным не делаю MouseMove
	static std::set<int> _pressedKeys;
	static bool _altWasPressedBeforeMouse;
public:
    static unsigned int scaleColor; // дл€ отладки масштабировани€ только

	InputSystem();
	static FPoint2D GetMousePos();
	static FPoint2D GetScalePos();
	virtual ~InputSystem();
	// эти методы класса переопредел€ем 
	// если хотим обрабатывать соотвествующее событие

	// клик по элементу(нажатие левой кнопки мыши)
	virtual void OnMouseDown(const FPoint2D &mousePos);
	// отпустили кнопку мыши(неважно где)
	virtual void OnMouseUp();
	// движение мыши
	virtual void OnMouseMove(const FPoint2D &mousePos);
	// колесико мыши
	virtual bool OnMouseWheel(int direction);
	
	virtual bool OnKey(int key, bool bDown);

	virtual void OnStartScale(const FPoint2D &pos);

    virtual void OnScale(float distance, float scale);

    virtual void OnEndScale();

	// длительное нажатие и удержание
	virtual void OnLongTap(const FPoint2D &mousePos);
	// двойной клик
	virtual void OnDoubleClick(const FPoint2D &mousePos);

	// ќЅя«ј≈Ћ№Ќќ нужно определить дл€ корректной работы OnMouseDown() и прочих функций
	virtual bool IsMouseOver(const FPoint2D &mousePos) = 0;
	
protected:
	// дл€ сложных(вложенных) элементов ввода - может понадобитьс€, переписать общий способ
	void RemoveFromList(InputSystem *listener); 

private:
	typedef std::list<InputSystem *> Listeners;
	static Listeners _listeners;

	static void LongTap();
	static void DoubleClick(const FPoint2D &mousePos);

public:
	static void MouseWheel(int direction);
	static void MouseDown(const FPoint2D &mousePos);
	static void MouseUp();
	static void MouseMove(const FPoint2D &mousePos);
	static void MouseMoveScalePos(const FPoint2D &mousePos);

	static void Reset();
	static void Init();
	static void ResetLock();
	static bool CheckForEvent(float dt);

	static void OnKeyDown(int key);
	static void OnKeyUp(int key);
	static bool IsPressed(int key);

    static bool IsScaling() { return _altWasPressedBeforeMouse; } // win only
    
    static void StartScale(const FPoint2D &pos);
    static void Scale(float distance, float scale);
    static void EndScale();
};

#endif//MYENGINE_INPUTSYSTEM_H
