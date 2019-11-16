/*
** 2D Game Animation HGE api 
** by Pakholkov Leonid am98pln@gmail.com
**
** 
** Класс для непосредственного воспроизведения анимации в игре
**
** При портировании нужно заменить:
** hgeVector - это класс точки в двумерном пространстве
** HTEXTURE  - указатель на используемую текстуру
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
	// Конструктор - используется в менеджере загрузки анимации
	// параметры анимации читаются из XML - можно использовать любой парсер,
	// который умеет перебирать ноды и читать атрибуты,
	// здесь успользован TinyXml
	//
    Animation(const std::string &id, rapidxml::xml_node<> *xe, GLTexture *hTexture, float width, float height);
    void ReloadTexture(GLTexture *hTexture);
	void DetachAll();

private:
	//
	// Деструктор - используется в менеджере загрузки анимации
	//
	virtual ~Animation();

public:
	//
	// Настройить положение анимации на экране, 
	// "pos" - положение анимации на экране, 
	// чтобы нарисовать зеркально - ставим "mirror" в "true".
	// Метод лучше всего вызывать перед каждым вызывом Draw()
	// исключение может быть, если такая анимация на экране всего одна
	//
	//void SetPos(const FPoint2D &pos, bool mirror);

	//
	// Нарисовать анимацию на экране 
	// "position" - значение времени для анимации из диапазона [0, 1]
	//
	void Draw(float position);

	const Matrix &GetSubPosition() { return _subPosition; }

	// 
	// Нарисовать "смешаную" с другой анимацией
	//
	void DrawMixed(float position, const Animation &anim, float animPosition, float mix);

	//
	// Время полного цикла анимации 
	// эту функцию нужно использовать в Update для расчета 
	// значения "position" метода Draw
	// например, для зацикленой анимации можно считать так:
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
	
	// этот тот самый вектор, который будет выполнять роль стека матриц
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
	// делаем "слепок" анимации без учета установленой матрицы
	bool InitWithAnimation(Animation *anim, float position, Animation *target);
	//void InitWithAnimations(Animation *animOne, float positionOne, Animation *animTwo, float positionTwo, float mix);
	// смешиваем с новой анимацией без учета матрицы, 
	// результат рисуем с учетом установленой матрицы
	void DrawMixed(Animation *anim, float position, float mix);
	// смешиваем с новой анимацией без учета матрицы, но вместо рисовния
	// делаем ее новым слепком
	bool MixWith(Animation *anim, float position, float mix, Animation *);

	void DrawMixed(Animation *one, Animation *two, float oneTime, float twoTime, float mix);
};

//
// Пример примитивного менеджера анимаций
//
// К конкретных проектах рекомендую использовать его исходники 
// для встраивания в общую систему работы с ресурсами
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
	// Загрузить файл анимации
	// в случае успеха - вернет "true"
	// можно загружать несколько файлов - менеджер "коллекционирует" анимации
	//
    static void Load(std::string fileName, bool threadSafe);
    static void LoadInner(std::string fileName);
	static void Unload(const std::string &fileName);
	
    static void ReplaceTexture(std::string fileName, GLTexture *spr);
	static void ReloadTextures();

    static void UploadTexture(const std::string &fileName);
    //
	// Выгрузить все анимации и текстуры из памяти
	//
	static void UnloadAll();
	//
	// Получить указатель на анимацию - animationId - это имя анимации в редакторе 
	// (ВАЖНО нельзя вызывать delete для нее!)
	//
	static Animation *getAnimation(const std::string &animationId);
};

};

#endif//ANIMATION_H

