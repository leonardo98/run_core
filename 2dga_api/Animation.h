/*
** 2D Game Animation HGE api 
** by Pakholkov Leonid am98pln@gmail.com
**
** 
** ����� ��� ����������������� ��������������� �������� � ����
**
** ��� ������������ ����� ��������:
** hgeVector - ��� ����� ����� � ��������� ������������
** HTEXTURE  - ��������� �� ������������ ��������
**
*/

#ifndef ANIMATION_H
#define ANIMATION_H

#include "../Core/rapidxml/rapidxml.hpp"
#include <string>
#include <map>
#include "../Core/Matrix.h"
#include "../Core/CoreTriple.h"
#include "MovingPart.h"

namespace My {

class Animation
{
public:

	std::string fileName;
	const std::string animationName;

	//
	// ����������� - ������������ � ��������� �������� ��������
	// ��������� �������� �������� �� XML - ����� ������������ ����� ������,
	// ������� ����� ���������� ���� � ������ ��������,
	// ����� ����������� TinyXml
	//
    Animation(const std::string &id, rapidxml::xml_node<> *xe, GLTexture *hTexture, float width, float height);
    void ReloadTexture(GLTexture *hTexture);
	void DetachAll();

private:
	//
	// ���������� - ������������ � ��������� �������� ��������
	//
	virtual ~Animation();

public:
	//
	// ���������� ��������� �������� �� ������, 
	// "pos" - ��������� �������� �� ������, 
	// ����� ���������� ��������� - ������ "mirror" � "true".
	// ����� ����� ����� �������� ����� ������ ������� Draw()
	// ���������� ����� ����, ���� ����� �������� �� ������ ����� ����
	//
	//void SetPos(const FPoint2D &pos, bool mirror);

	//
	// ���������� �������� �� ������ 
	// "position" - �������� ������� ��� �������� �� ��������� [0, 1]
	//
	void Draw(float position);

	const Matrix &GetSubPosition() { return _subPosition; }

	// 
	// ���������� "��������" � ������ ���������
	//
	void DrawMixed(float position, const Animation &anim, float animPosition, float mix);

	//
	// ����� ������� ����� �������� 
	// ��� ������� ����� ������������ � Update ��� ������� 
	// �������� "position" ������ Draw
	// ��������, ��� ���������� �������� ����� ������� ���:
	// progress += deltaTime / anim->Time();
	// if(progress >= 1.f) progress = 0.f;
	//
	float Time() const;

	bool PixelCheck(const FPoint2D &pos);

	MovingPart * GetMovingPart(const std::string &);
private:
	bool _simple;
    void AddBone(MovingPart *bone);
	float _time;
	FPoint2D _pivotPos;
	My::List _bones;
	My::List _renderList;
	My::List _secondRenderList;
	Matrix _subPosition;
	VertexBuffer _vb;
	
	// ���� ��� ����� ������, ������� ����� ��������� ���� ����� ������
    friend class MovingPart;
    friend class AnimationState;
	friend class AnimationManager;
};

class AnimationState {
private:
	std::vector<PartPosition> _partPositions;
	CoreQuad _quad;
	PartPosition _mixed;
public:
	// ������ "������" �������� ��� ����� ������������ �������
	bool InitWithAnimation(Animation *anim, float position, Animation *target);
	//void InitWithAnimations(Animation *animOne, float positionOne, Animation *animTwo, float positionTwo, float mix);
	// ��������� � ����� ��������� ��� ����� �������, 
	// ��������� ������ � ������ ������������ �������
	void DrawMixed(Animation *anim, float position, float mix);
	// ��������� � ����� ��������� ��� ����� �������, �� ������ ��������
	// ������ �� ����� �������
	bool MixWith(Animation *anim, float position, float mix, Animation *);

	void DrawMixed(Animation *one, Animation *two, float oneTime, float twoTime, float mix);
};

//
// ������ ������������ ��������� ��������
//
// � ���������� �������� ���������� ������������ ��� ��������� 
// ��� ����������� � ����� ������� ������ � ���������
//
class AnimationManager {
	typedef std::map<std::string, Animation *> AnimationMap;
	static AnimationMap _animations;
	static bool Loaded(const std::string &fileName);
	static bool enableThread;
public:

	static void EnableThread(bool v);
	static bool EnableThread() { return enableThread; }
    static void ParseAnimationDescription(rapidxml::xml_node<> *root, const std::string &fileName, GLTexture *tex, int width, int height);
	static void FinishThread();
	//
	// ��������� ���� ��������
	// � ������ ������ - ������ "true"
	// ����� ��������� ��������� ������ - �������� "���������������" ��������
	//
    static void Load(std::string fileName, bool threadSafe);
    static void LoadInner(std::string fileName);
	static void Unload(const std::string &fileName);
	
    static void ReplaceTexture(std::string fileName, GLTexture *spr);
	static void ReloadTextures();

    static void UploadTexture(const std::string &fileName);
    //
	// ��������� ��� �������� � �������� �� ������
	//
	static void UnloadAll();
	//
	// �������� ��������� �� �������� - animationId - ��� ��� �������� � ��������� 
	// (����� ������ �������� delete ��� ���!)
	//
	static Animation *getAnimation(const std::string &animationId);
};

};

#endif//ANIMATION_H

