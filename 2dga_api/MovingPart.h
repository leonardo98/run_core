/*
** 2D Game Animation HGE api 
** by Pakholkov Leonid am98pln@gmail.com
**
**
** Класс подвижного спрайта - "сердце" моего формата анимации
** 
** При портировании нужно заменить:
**     HGE * - указатель на рендер-контекст, нужен для отрисовки hgeQuad
** hgeVector - это класс точки в двумерном пространстве
**  HTEXTURE - указатель на используемую текстуру
**   hgeQuad - структура для вывода спрайта на экран,
**             имеет 4 вершины, для которых можно задать 
**             текстурные и экранные координаты
**
*/

#ifndef MOVINGPART_H
#define MOVINGPART_H

#include <string>
#include "../Core/rapidxml/rapidxml.hpp"
#include "../Core/MotionValues.h"
#include "../Core/Matrix.h"
#include "../Core/CoreTriple.h"
#include "../Core/Render.h"

namespace My {

#define fatof(a) static_cast<float>(atof(a))

class Animation;

class MovingPart;

typedef std::vector<MovingPart *> List;

struct PartPosition {
	float angle;
	float alpha;
	float sx, sy;
	float x, y;
	std::string name;
};

class MovingPart
{
public:

	//
	// Деструктор
	//
	~MovingPart();

	//
	// Конструктор - читаем данные спрайта из XML
	// запоминаем текстуру, спрайты из которой рисуем
	//
	MovingPart(Animation *animation, rapidxml::xml_node<> *xe, GLTexture *hTexture, float width, float height);
	void ReloadTexture(GLTexture *hTexture);
	//
	// Выводим спрайт на экран
	// положение во времени анимации - p, [ 0<= p <= 1 ]
	// положение на экране - stack
	//
	void PreDraw(float p, const Matrix &stack);
	void PreDrawInLocalPosition(const Matrix &stack);

	// для смешивания анимации
	void PreCalcLocalPosition(float p);
	PartPosition precalculatedLocalPosition; 
	PartPosition localPosition; 

	void Draw();
	void SecondLayerDraw();

	bool PixelCheck(const FPoint2D &pos);

	void Attach(GLTexture *, const FPoint2D &, int boneOrder = 0, bool addMode = false);
	void DetachAll();

private:

	Matrix _matrix;
	struct Attachment
	{
		bool addMode;
		FPoint2D pos;
		GLTexture *tex;
	};

	std::list<Attachment> _beforeBone;
	std::list<Attachment> _afterBone;
	std::list<Attachment> _secondLayerBone;

	std::vector<FPoint2D> _points;
	std::vector<FPoint2D> _pointsResult;
	std::string boneName;
	
	FPoint2D _center;
	MotionValues _x;
	MotionValues _y;
	MotionValues _angle;
	MotionValues _alpha;
	float _alphaCurrent;
	MotionValues _scaleX;
	MotionValues _scaleY;
	MotionValues::Motion _movingType;

	List _bones;
	int _order;
    bool _visible;

	//
	bool _enable;
	//
	void CreateQuad(GLTexture *hTexture, float width, float height, const std::string &texture);
	bool _hasTexture;
	CoreQuad _quad;

	FPoint2D _origin[4];	

    friend bool CmpBoneOrder(MovingPart *one, MovingPart *two);
    friend class Animation;
    friend class AnimationState;
public:
	void SetEnable(bool v) { _enable = v; }
	inline bool HasTexture() const { return _hasTexture; }
	void ClearPoints() { _points.clear(); _pointsResult.clear(); }
	void PushPoint(const FPoint2D &pos) { _points.push_back(pos); _pointsResult.resize(_points.size()); }
	std::vector<FPoint2D> &GetPointsResult() { return _pointsResult; }
	std::vector<FPoint2D> &GetPoints() { return _points; }
};

bool CmpBoneOrder(MovingPart *one, MovingPart *two);

};// namescape My

#endif//MOVINGPART_H