// InputSystem.cpp: implementation of the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "InputSystem.h"
#include "Render.h"
#include "Messager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


float InputSystem::DOUBLE_CLICK_TIME; // максимальное время на второй клик - если время больше - событие "аннулируется"
float InputSystem::LONG_TAP_EPS; // погрешность на случайные движения мыши во время нажатия, если движение более сильное - событие "аннулируется"
float InputSystem::LONG_TAP_TIME; // минимальное время на вызов события - если время меньше - событие "аннулируется"
float InputSystem::_timeCounter; // счетчик времени(отсчитывает время либо с последнего события нажатия или отпускания кнопки мыши
bool InputSystem::_longTap; // true - если ловим событие "Длинный Тап"
bool InputSystem::_doubleClick; // true - если ловим это событие "Двойной клик"
unsigned int InputSystem::scaleColor;
FPoint2D InputSystem::_tapPos;
std::vector<FPoint2D> InputSystem::_firstScalePoint;
FPoint2D InputSystem::_mousePos;
FPoint2D InputSystem::_scalePos;
InputSystem *InputSystem::_locked;
std::set<int> InputSystem::_pressedKeys;
bool InputSystem::_altWasPressedBeforeMouse = false;

FPoint2D InputSystem::GetMousePos()
{
	return _tapPos;
}

FPoint2D InputSystem::GetScalePos()
{
	return _scalePos;
}

InputSystem::InputSystem()
{
	_listeners.push_back(this);
	scaleColor = 0xFFFFFFFF;
}

InputSystem::~InputSystem()
{
	Listeners::iterator i;
	for (i = _listeners.begin(); i != _listeners.end() && *i != this; i++);
	if (i != _listeners.end()) {// нет в списке (уже удален)
		_listeners.erase(i);
	}
}

void InputSystem::OnMouseDown(const FPoint2D &mousePos) {}

void InputSystem::OnMouseUp() {}

void InputSystem::OnMouseMove(const FPoint2D &mousePos) {}

void InputSystem::OnLongTap(const FPoint2D &mousePos) {}

void InputSystem::OnDoubleClick(const FPoint2D &mousePos) {}

void InputSystem::OnStartScale(const FPoint2D &pos) {}

void InputSystem::OnScale(float distance, float deltaScale) {}

void InputSystem::OnEndScale() {}

void InputSystem::StartScale(const FPoint2D &pos)
{
    for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++)
    {
        if ((*i)->IsMouseOver(pos))
        {
            (*i)->OnStartScale(pos);
            _locked = *i;
            _firstScalePoint.clear();
            break;
        }
    }
}

void InputSystem::EndScale()
{
    if (_locked != NULL) {
        _locked->OnEndScale();
    }
}

void InputSystem::Scale(float distance, float scale)
{
    if (_locked != NULL)
    {
        _locked->OnScale(distance, scale);
    }
}

bool InputSystem::OnMouseWheel(int direction) {return false;}

bool InputSystem::OnKey(int key, bool bDown) {return false;}

void InputSystem::MouseDown(const FPoint2D &mousePos)
{
	_tapPos = mousePos;
	if (_altWasPressedBeforeMouse)
	{
        StartScale(_scalePos);
	}
	else
	{
		for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) 
		{
			if ((*i)->IsMouseOver(mousePos))
			{
				(*i)->OnMouseDown(mousePos);
				_locked = *i;
				break;
			}
		}
	}
}

void InputSystem::LongTap()
{
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++)
	{
		if ((*i)->IsMouseOver(_tapPos)) 
		{
			(*i)->OnLongTap(_tapPos);
			_locked = *i;
			return;
		}
	}
}

void InputSystem::DoubleClick(const FPoint2D &mousePos) {
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->IsMouseOver(mousePos)) {
			(*i)->OnDoubleClick(mousePos);
			_locked = *i;
			return;
		}
	}
}

void InputSystem::MouseUp() 
{
	if (_altWasPressedBeforeMouse)
	{
		_altWasPressedBeforeMouse = false;
        EndScale();
		_firstScalePoint.clear();
	}
	for (Listeners::iterator i = _listeners.begin(), e = _listeners.end(); i != e; i++) {
		(*i)->OnMouseUp();
	}
	_locked = NULL;
}

void InputSystem::Reset() {
	_locked = NULL;
}

void InputSystem::MouseMoveScalePos(const FPoint2D &mousePos)
{
	_mousePos = mousePos;
}

void InputSystem::MouseMove(const FPoint2D &mousePos)
{
	if (_locked != NULL)
	{
		if (_altWasPressedBeforeMouse)
		{
			if (_firstScalePoint.size() == 0)
				_firstScalePoint.push_back(mousePos);

			float f = (mousePos - _scalePos).Length() / (_firstScalePoint[0] - _scalePos).Length();
			
			if (f < 1.f)
				scaleColor = 0xFFFF0000;
			else if (f > 1.f)
				scaleColor = 0xFF00FFFF;
			else
				scaleColor = 0xFFFFFFFF;

			_locked->Scale((_firstScalePoint[0] - _scalePos).Length(), f); 
		}
		else
		{
			_locked->OnMouseMove(mousePos);
		}
	}
	else
	{
		for (Listeners::iterator i = _listeners.begin(), e = _listeners.end(); i != e; i++) {
			(*i)->OnMouseMove(mousePos);
		}
	}
	_tapPos = mousePos;
}

InputSystem::Listeners InputSystem::_listeners;

void InputSystem::RemoveFromList(InputSystem *listener) {
	Listeners::iterator i;
	for (i = _listeners.begin(); i != _listeners.end() && *i != listener; i++);
	if (i == _listeners.end()) {// нет в списке (уже удален)
        log::write("удален уже Listener или удаляемый не унаследован от InputSystem");
		exit(-4);
	}
	_listeners.erase(i);
}

void OnLongTap(int direction) {
}

void OnDoubleClick(int direction) {
}

void InputSystem::MouseWheel(int direction) {
	if (_locked != NULL) {
		_locked->OnMouseWheel(direction);
		return;
	}
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->OnMouseWheel(direction)) {
			return;
		}
	}
}

bool InputSystem::CheckForEvent(float dt) {
	_timeCounter += dt;
	_doubleClick &= (_timeCounter < DOUBLE_CLICK_TIME);
	//hgeInputEvent event;
	//while (Render::GetDC()->Input_GetEvent(&event)) {
	//	if (event.type == INPUT_MBUTTONDOWN && event.key == HGEK_LBUTTON) {
	//		_longTapPos = FPoint2D(event.x, event.y);
	//		MouseDown(_longTapPos);
	//		if (_doubleClick) {
	//			DoubleClick(_longTapPos);
	//			_doubleClick = false;
	//		}
	//		_timeCounter = dt / 2.f;
	//		_longTap = true;
	//		_doubleClick = true;
	//	} else if (event.type == INPUT_MBUTTONUP && event.key == HGEK_LBUTTON) {
	//		MouseUp();
	//		_longTap = false;
	//	} else if (event.type == INPUT_MOUSEMOVE) {
	//		FPoint2D pos = FPoint2D(event.x, event.y);
	//		MouseMove(pos);
	//		_longTap &= (_longTapPos - pos).Length() > LONG_TAP_EPS;
	//	} else if (event.type == INPUT_MOUSEWHEEL) {
	//		MouseWheel(event.wheel);
	//	} else if (event.type == INPUT_KEYDOWN /*&& event.key == HGEK_ESCAPE*/) {
	//		for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
	//			if ((*i)->OnKey(event.key)) {
	//				break;
	//			}
	//		}
	//	//	return true;
	//	}
	//}
	if (_longTap) {
		LongTap();
		_longTap = false;
	}
	return false;
}

void InputSystem::Init() {
	LONG_TAP_EPS = 10.f;
	LONG_TAP_TIME = 0.2f;
	DOUBLE_CLICK_TIME = 0.2f;
	_timeCounter = 0.f;
	_longTap = false;
	_doubleClick = false; 
	_locked = NULL;
}

void InputSystem::ResetLock()
{
	_locked = NULL;
}

void InputSystem::OnKeyDown(int key)
{
	_pressedKeys.insert(key);
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->OnKey(key, true)) {
			break;
		}
	}
//	if (key == GK_MENU)
//	{
//		if (!_altWasPressedBeforeMouse)
//		{
//			_altWasPressedBeforeMouse = true;
//			_scalePos = _mousePos;
//		}
//	}
}

void InputSystem::OnKeyUp(int key)
{
	if (_pressedKeys.find(key) != _pressedKeys.end())
	{
		_pressedKeys.erase(key);
	}
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		(*i)->OnKey(key, false);
	}
//	if (key == GK_MENU && !IsPressed(GK_LBUTTON))
//	{
//		_altWasPressedBeforeMouse = false;
//	}
}

bool InputSystem::IsPressed(int key) 
{
	return (_pressedKeys.find(key) != _pressedKeys.end());
}
