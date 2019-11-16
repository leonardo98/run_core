#ifndef TAP_BUTTON_H
#define TAP_BUTTON_H

#include "LevelSet.h"
#include "Beauty.h"
#include "LinkToComplex.h"
#include "BeautyText.h"
#include "AnimationArea.h"
#include "InputSystem.h"

struct Poses
{
	FPoint2D pos;
	FPoint2D scale;
	float angle;
	float alpha;
    unsigned int color;
	BeautyBase *beauty;
	Poses() : beauty(NULL) {}
	void InitWithBeauty(BeautyBase *b)
	{
		beauty = b;
		pos = b->GetPos();
		scale = b->GetScale();
		angle = b->GetAngle();
		alpha = b->GetAlpha();
		color = b->GetColor();
	}
	void Reset() 
	{
		beauty->SetPos(pos);
		beauty->SetScale(scale);
		beauty->SetAngle(angle);
		beauty->SetAlpha(alpha);
		beauty->SetColor(color);
	}
};

class EventListener
{
public:
	virtual int Event(const std::string &msg) { return -1; };
};

class TapButton
	: TweenEventListener
{
private:
	BeautyList _btns;
	BeautyBase *_touchArea;

	std::string _receiver;
	EventListener *_eventReceiver;
	std::string _message;

	std::vector<Poses> _poses;
	bool _isTouch;
	bool _checked;
	bool _visible;
	bool _enable;
	void Init(const LevelSet &levelSet, const char *id, bool invisible);
	TapButton(const LevelSet &levelSet, const char *id, const char *recever, const char *message, bool invisible = false);
	TapButton(const LevelSet &levelSet, const char *id, EventListener *recever, const char *message, bool invisible = false);
	bool Check(const FPoint2D &pos);
	void EndTouch();
	void CancelTouch();
	void Reset();
	bool UnderPos(const FPoint2D &pos);
    void Action() const;
	friend class TapButtonInterface;
    
public:
	void SetMessage(const std::string &s) { _message = s; }
	void UpdatePos(BeautyBase *b);
	void SetEnable(bool v);
	virtual void TweenEvent();
	void SetVisible(bool visible);
    void SetVisibleByTag(const std::string &searchTag, const std::string &keyTag);
	void SetChecked(bool checked);
    bool IsVisible() { return _visible; }
    bool IsChecked() { return _checked; }
	void HideTo(float x, float y, float time);
	void ShowFrom(float x, float y, float time);
	Beauty* GetBeauty();
	LinkToComplex* GetLinkToComplex();
	BeautyText *GetBeautyText(float userFloat = 0.f);
	void MoveTo(float x, float y);
	AnimationArea *GetAnimationArea();
	void Command(const std::string &cmd);
    
    void PopAttach(BeautyBase *);
    void PushAttach(BeautyBase *);

    bool HasTag(const std::string &tag) const;
};

typedef std::list<TapButton *> TapButtonList;

class TapButtonInterface
{
private:
	TapButtonList _list;
	enum TouchState
	{
		touch_none,
		touch_moving,
		touch_canceled
	};
	TouchState _state;
public:
	void Clear();
	TapButtonInterface() { Clear(); }
	virtual ~TapButtonInterface();
	TapButton * AddButton(const LevelSet &levelSet, const char *id, const char *recever, const char *message, bool invisible = false);
	TapButton * AddButton(const LevelSet &levelSet, const char *id, EventListener *receiver, const char *message, bool invisible = false);
	bool OnTouchMove(const FPoint2D &);
	void OnTouchEnd();
	void TouchCancel();
	void Reset();
	bool UnderPos(const FPoint2D &pos);
    bool OnKey(int key);
};

#endif//TAP_BUTTON_H
