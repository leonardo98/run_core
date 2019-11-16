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
// ���������� ��� �������� �����,
// ���� ����� ������������ ������� ���� - ����������� �� ����� ������
// ���� ��� - �� Object.h
//

class InputSystem 
	: virtual public Object  
{
private:
	static float DOUBLE_CLICK_TIME; // ������������ ����� �� ������ ���� - ���� ����� ������ - ������� "������������"
	static float LONG_TAP_EPS; // ����������� �� ��������� �������� ���� �� ����� �������, ���� �������� ����� ������� - ������� "������������"
	static float LONG_TAP_TIME; // ����������� ����� �� ����� ������� - ���� ����� ������ - ������� "������������"
	static float _timeCounter; // ������� �������(����������� ����� ���� � ���������� ������� ������� ��� ���������� ������ ����
	static bool _longTap; // true - ���� ����� ������� "������� ���"
	static bool _doubleClick; // true - ���� ����� ��� ������� "������� ����"
	static FPoint2D _mousePos; // ��� ������� ��������������� ������ 
	static std::vector<FPoint2D> _firstScalePoint;
	static FPoint2D _tapPos;
	static FPoint2D _scalePos;
	static InputSystem *_locked;// ���� ���� ��� ������ ������������ ����(�������/����������) - ��������� �� ����� MouseMove
	static std::set<int> _pressedKeys;
	static bool _altWasPressedBeforeMouse;
public:
    static unsigned int scaleColor; // ��� ������� ��������������� ������

	InputSystem();
	static FPoint2D GetMousePos();
	static FPoint2D GetScalePos();
	virtual ~InputSystem();
	// ��� ������ ������ �������������� 
	// ���� ����� ������������ �������������� �������

	// ���� �� ��������(������� ����� ������ ����)
	virtual void OnMouseDown(const FPoint2D &mousePos);
	// ��������� ������ ����(������� ���)
	virtual void OnMouseUp();
	// �������� ����
	virtual void OnMouseMove(const FPoint2D &mousePos);
	// �������� ����
	virtual bool OnMouseWheel(int direction);
	
	virtual bool OnKey(int key, bool bDown);

	virtual void OnStartScale(const FPoint2D &pos);

    virtual void OnScale(float distance, float scale);

    virtual void OnEndScale();

	// ���������� ������� � ���������
	virtual void OnLongTap(const FPoint2D &mousePos);
	// ������� ����
	virtual void OnDoubleClick(const FPoint2D &mousePos);

	// ���������� ����� ���������� ��� ���������� ������ OnMouseDown() � ������ �������
	virtual bool IsMouseOver(const FPoint2D &mousePos) = 0;
	
protected:
	// ��� �������(���������) ��������� ����� - ����� ������������, ���������� ����� ������
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
