#include "MovingPart.h"
#include "Animation.h"
#include "../Core/Math.h"
#include "../Core/InputSystem.h"

namespace My {

// 
// «аполнение hgeQuad
// тут вычисл€ютс€ текстурные координаты дл€ спрайта,
// данные об его экранном размере читаютс€ из строки "texture"
//
//
void MovingPart::CreateQuad(GLTexture *hTexture, float width, float height, const std::string &texture) {

	// начало блока - этот код зависит от используемого движка
	if (hTexture)
	{
        width = (float)(hTexture->Width());
        height = (float)(hTexture->Height());
	}
	// конец блока

	std::string::size_type start = 0;
	std::string::size_type end = texture.find(':', start);
	float x = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float y = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float w = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float h = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float offX = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	float offY = fatof(texture.substr(start).c_str());

	// начало блока - этот код зависит от используемого движка
	assert(width > 0 && height > 0);
	_quad.tex = hTexture;
	//_quad.color[3] = _quad.color[2] = _quad.color[1] = _quad.color[0] = 0xFFFFFFFF;
	_quad.uv[0].x = x / width; _quad.uv[0].y = y / height;
	_quad.uv[1].x = (x + w) / width; _quad.uv[1].y = y / height;
	_quad.uv[2].x = (x + w) / width; _quad.uv[2].y = (y + h) / height;
	_quad.uv[3].x = x / width; _quad.uv[3].y = (y + h) / height;
	// конец блока

	x = offX;
	y = offY;
	_origin[0].x = x;     _origin[0].y = y;
	_origin[1].x = x + w;     _origin[1].y = y;
	_origin[2].x = x + w; _origin[2].y = y + h;
	_origin[3].x = x; _origin[3].y = y + h;

}


//
// ƒеструктор - освобождаем рекурсивно пам€ть
//
MovingPart::~MovingPart() {
	for (unsigned int i = 0; i < _bones.size(); ++i) 
		delete _bones[i];
}


//
//  онструктор - читаем данные спрайта из XML
// запоминаем текстуру, спрайты из которой рисуем
//
MovingPart::MovingPart(Animation *animation, rapidxml::xml_node<> *xe, GLTexture *hTexture, float width, float height)
	: _enable(true)
	, _alphaCurrent(1.f)
{
    animation->AddBone(this);

	boneName =  xe->first_attribute("name")->value();

	rapidxml::xml_attribute<> *tmp = xe->first_attribute("moving_type");
	if (tmp == NULL || strcmp(tmp->value(), "spline") == 0) {
		_movingType = MotionValues::m_spline;
	} else if (strcmp(tmp->value(), "line") == 0) {
		_movingType = MotionValues::m_line;
	} else {
		_movingType = MotionValues::m_discontinuous;
    }
	_order = atoi(xe->first_attribute("order")->value());
	_center.x = Math::Read(xe, "centerX", 0.f);
	_center.y = Math::Read(xe, "centerY", 0.f);
	tmp = xe->first_attribute("texture");
	if ((_hasTexture = (tmp != NULL))) {
		CreateQuad(hTexture, width, height, tmp->value());
	}

	//
	// вот это очень важный блок - тут читаютс€ данные 
	// о положении подвижного спрайта на экране и его движении 
	// во времени
	//
	rapidxml::xml_node<> *pos = xe->first_node("pos");
	if (pos == NULL) {
		_x.AddKey(0.f, 0.f);
		_y.AddKey(0.f, 0.f);
		_scaleX.AddKey(0.f, 1.f);
		_scaleY.AddKey(0.f, 1.f);
		_angle.AddKey(0.f, 0.f);
		_alpha.AddKey(0.f, 1.f);
	}
	while (pos) {
		float time = Math::Read(pos, "time", 0.f);
		_x.AddKey(time, Math::Read(pos, "x", 0.f));
		_y.AddKey(time, Math::Read(pos, "y", 0.f));	
		_scaleX.AddKey(time, Math::Read(pos, "scaleX", 1.f));
		_scaleY.AddKey(time, Math::Read(pos, "scaleY", 1.f));
		_angle.AddKey(time, Math::Read(pos, "angle", 0.f));
		_alpha.AddKey(time, Math::Read(pos, "alpha", 1.f));

		pos = pos->next_sibling("pos");
	}
	_x.SetType(_movingType);
	_y.SetType(_movingType);	
	_scaleX.SetType(_movingType);
	_scaleY.SetType(_movingType);
	_angle.SetType(_movingType);
	_alpha.SetType(MotionValues::m_line);

	// 
	// если есть - создаем дочерние подвижные спрайты
	//
	rapidxml::xml_node<> *element = xe->first_node();
	_bones.reserve(20);
	while (element) {
		std::string name = element->name();
		if (name == "movingPart") {
			_bones.push_back( new MovingPart(animation, element, hTexture, width, height) );
		}
		element = element->next_sibling();
	}
}


//
// ¬ыводим спрайт на экран
// положение во времени анимации - p, [ 0<= p <= 1 ]
// положение на экране - stack
//
void MovingPart::PreDraw(float p, const Matrix &stack) 
{
	float localT;
	int index = _x.Value(p, localT);

	// если функци€ определена дл€ значени€ "p" - рисуем
	// тут проверка только дл€ "_x" - т.к. у остальных функций движени€ область определени€ така€ же
	if (_visible = (index >= 0)) {
		_matrix = stack;
		_matrix.Move(_x.GetFrame(index, localT), _y.GetFrame(index, localT));
		_matrix.Rotate(_angle.GetFrame(index, localT));
		_matrix.Scale(_scaleX.GetFrame(index, localT), _scaleY.GetFrame(index, localT));

		_matrix.Move(-_center.x, -_center.y);

		_alphaCurrent = _alpha.GetFrame(index, localT);

		if (_hasTexture) 
		{
			_matrix.Mul(_origin[0].x, _origin[0].y, _quad.v[0].x, _quad.v[0].y);
			_matrix.Mul(_origin[1].x, _origin[1].y, _quad.v[1].x, _quad.v[1].y);
			_matrix.Mul(_origin[2].x, _origin[2].y, _quad.v[2].x, _quad.v[2].y);
			_matrix.Mul(_origin[3].x, _origin[3].y, _quad.v[3].x, _quad.v[3].y);
		}        
		// конец блока
		for (std::vector<FPoint2D>::size_type i = 0, e = _points.size(); i != e; ++i)
		{
			_matrix.Mul(_points[i].x, _points[i].y, _pointsResult[i].x, _pointsResult[i].y);
		}


		for (unsigned int i = 0; i < _bones.size(); ++i)
		{
			_bones[i]->PreDraw(p, _matrix);
		}
	}
}

void MovingPart::ReloadTexture(GLTexture *hTexture) 
{ 
	if (HasTexture()) 
	{ 
		_quad.tex = hTexture; 
	} 
	for (unsigned int i = 0; i < _bones.size(); ++i)
	{
		_bones[i]->ReloadTexture(hTexture);
	}
}

void MovingPart::PreDrawInLocalPosition(const Matrix &stack) 
{
	// если функци€ определена дл€ значени€ "p" - рисуем
	// тут проверка только дл€ "_x" - т.к. у остальных функций движени€ область определени€ така€ же
	if (_visible) {
		_matrix = stack;
		_matrix.Move(localPosition.x, localPosition.y);
		_matrix.Rotate(localPosition.angle);
		_matrix.Scale(localPosition.sx, localPosition.sy);

		_matrix.Move(-_center.x, -_center.y);

		if (_hasTexture) {
			_matrix.Mul(_origin[0].x, _origin[0].y, _quad.v[0].x, _quad.v[0].y);
			_matrix.Mul(_origin[1].x, _origin[1].y, _quad.v[1].x, _quad.v[1].y);
			_matrix.Mul(_origin[2].x, _origin[2].y, _quad.v[2].x, _quad.v[2].y);
			_matrix.Mul(_origin[3].x, _origin[3].y, _quad.v[3].x, _quad.v[3].y);
		}
		// конец блока
		for (unsigned int i = 0; i < _bones.size(); ++i)
			_bones[i]->PreDrawInLocalPosition(_matrix);	
	}
}

void MovingPart::PreCalcLocalPosition(float p) 
{
	float localT;
	int index = _x.Value(p, localT);

	// если функци€ определена дл€ значени€ "p" - рисуем
	// тут проверка только дл€ "_x" - т.к. у остальных функций движени€ область определени€ така€ же
	if (_visible = (index >= 0)) {
		precalculatedLocalPosition.x = _x.GetFrame(index, localT);
		precalculatedLocalPosition.y = _y.GetFrame(index, localT);
		precalculatedLocalPosition.angle = _angle.GetFrame(index, localT);
		precalculatedLocalPosition.sx = _scaleX.GetFrame(index, localT);
		precalculatedLocalPosition.sy = _scaleY.GetFrame(index, localT);
		precalculatedLocalPosition.alpha = _alpha.GetFrame(index, localT);
	}
}

void MovingPart::SecondLayerDraw() 
{
	if (_secondLayerBone.size())
	{
		Render::PushMatrix();
		Render::MatrixMul(_matrix);
		for (std::list<Attachment>::iterator i = _secondLayerBone.begin(), e = _secondLayerBone.end(); i != e; ++i)
		{
			if (i->addMode)
			{
				Render::SetBlendMode(1);
                i->tex->Render(i->pos);
				Render::SetBlendMode(0);
			}
			else
			{
                i->tex->Render(i->pos);
			}
		}
		Render::PopMatrix();
	}
}

void MovingPart::Draw() 
{
	if (_visible) {
		if (_beforeBone.size())
		{
			Render::PushMatrix();
			Render::MatrixMul(_matrix);
			for (std::list<Attachment>::iterator i = _beforeBone.begin(), e = _beforeBone.end(); i != e; ++i)
			{
				i->tex->Render(i->pos);
			}
			Render::PopMatrix();
		}
		if (_enable && _hasTexture)
		{
			if (_alphaCurrent < 1.f)
			{
				Render::PushColorAndMul(static_cast<unsigned int>(0xFF * _alphaCurrent) << 24 | 0xFFFFFF);
				Render::DrawQuad(_quad);
				Render::PopColor();
			}
			else
			{
				Render::DrawQuad(_quad);
			}
		}
		if (_afterBone.size())
		{
			Render::PushMatrix();
			Render::MatrixMul(_matrix);
			for (std::list<Attachment>::iterator i = _afterBone.begin(), e = _afterBone.end(); i != e; ++i)
			{
				i->tex->Render(i->pos);
				//i->tex->Render(InputSystem::GetMousePos());
			}
			Render::PopMatrix();
		}
    }
}

bool CmpBoneOrder(MovingPart *one, MovingPart *two) 
{
    return one->_order < two->_order;
}

bool MovingPart::PixelCheck(const FPoint2D &pos) 
{
	if (_visible) 
	{
		FPoint2D m(pos.x - _quad.v[0].x, pos.y - _quad.v[0].y);
		FPoint2D a(_quad.v[1].x - _quad.v[0].x, _quad.v[1].y - _quad.v[0].y);
		FPoint2D b(_quad.v[3].x - _quad.v[0].x, _quad.v[3].y - _quad.v[0].y);

		float k1 = (m.x * b.y - m.y * b.x) / (a.x * b.y - a.y * b.x);
		float k2 = (b.y > 1e-5) ? (m.y - k1 * a.y) / b.y : (m.x - k1 * a.x) / b.x;

		if (k1 < 0 || k1 > 1 || k2 < 0 || k2 > 1) 
		{
			return false;
		}
		return true;
	}
	return false;
}

void MovingPart::Attach(GLTexture *tex, const FPoint2D &pos, int boneOrder, bool mode)
{
	Attachment a;
	a.addMode = mode;
	a.pos = pos;
	a.tex = tex;
	if (boneOrder == -1)
	{
		_beforeBone.push_back(a);
	}
	else if (boneOrder == 0)
	{
		_afterBone.push_back(a);
	}
	else if (boneOrder == 1)
	{
		_secondLayerBone.push_back(a);
	}
}

void MovingPart::DetachAll()
{
	_beforeBone.clear();
	_afterBone.clear();
	_secondLayerBone.clear();
}

};//namespace My
