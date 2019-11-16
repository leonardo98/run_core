#include "CircleProgress.h"
#include "Core.h"
#include "Rect.h"

CircleProgress::CircleProgress(Beauty *beauty, bool revers)
    : BeautyBase("CircleProgress", FPoint2D())
	, _revers(revers)
{
	_value = 0.f;
	_pos = beauty->GetPos();
	_scale = beauty->GetScale();
	_angle = beauty->GetAngle();

	Init(beauty->GetTexture());

	Rect uv;
	beauty->GetTexture()->GetUVRect(uv);
	for (unsigned int i = 0; i < _cover.size(); ++i) 
	{
		_cover[i].uv[0].x = (uv.x2 - uv.x1) * _cover[i].uv[0].x + uv.x1;
		_cover[i].uv[0].y = (uv.y2 - uv.y1) * _cover[i].uv[0].y + uv.y1;

		_cover[i].uv[1].x = (uv.x2 - uv.x1) * _cover[i].uv[1].x + uv.x1;
		_cover[i].uv[1].y = (uv.y2 - uv.y1) * _cover[i].uv[1].y + uv.y1;

		_cover[i].uv[2].x = (uv.x2 - uv.x1) * _cover[i].uv[2].x + uv.x1;
		_cover[i].uv[2].y = (uv.y2 - uv.y1) * _cover[i].uv[2].y + uv.y1;
	}
}


void CircleProgress::Init(GLTexture *texture)
{
	_origin = texture;
	float w = static_cast<float>(texture->Width());
	float h = static_cast<float>(texture->Height());
	_center.x = w / 2;
	_center.y = h / 2;
	_cover.resize(8);

	float x[9] = {0.5f, 1.f, 1.f, 1.f, 0.5f, 0.f, 0.f, 0.f, 0.5f};
	if (_revers)
	{
		for (int i = 0; i < 9; ++i)
		{
			x[i] = 1.f - x[i];
		}
	}
	float y[9] = {0.f, 0.f,  0.5f,  1.f, 1.f, 1.f, 0.5f,  0.f,  0.f};

	float cx = w / 2;
	float cy = h / 2;
	for (unsigned int i = 0; i < _cover.size(); ++i) 
	{
        _cover[i].tex = texture;
	
		_cover[i].v[0].x = cx;
		_cover[i].v[0].y = cy;
		_cover[i].uv[0].x = cx / w;
		_cover[i].uv[0].y = cy / h;

		_cover[i].v[1].x = x[i] * w;
		_cover[i].v[1].y = y[i] * h;
		_cover[i].uv[1].x = x[i];
		_cover[i].uv[1].y = y[i];

		_cover[i].v[2].x = x[i + 1] * w;
		_cover[i].v[2].y = y[i + 1] * h;
		_cover[i].uv[2].x = x[i + 1];
		_cover[i].uv[2].y = y[i + 1];
	}
}

void CircleProgress::Draw()
{
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	Render::MatrixRotate(_angle);
	Render::MatrixScale(_scale.x, _scale.y);
	Render::MatrixMove(-_center.x, -_center.y);
	float p = _value;
	assert(0.f <= p);
	if (p >= 1.f)
	{
		p = 1.f;
		_origin->Render(0, 0); 
		Render::PopMatrix();
		return;
	}

	CoreTriple t;
	int last = std::min(7, static_cast<int>(8 * p));
	for (int i = 0; i < last; ++i)
	{
        Render::DrawTriangle(_cover[i]);
	}

	float angle = 2 * M_PI * p - last * M_PI / 4;
	if (last % 2 == 0)
	{
		p = tan(angle);
	}
	else
	{
		p = 1 - tan(M_PI / 4 - angle);
	}

	t = _cover[last];
	t.v[2].x = (t.v[2].x - t.v[1].x) * p + t.v[1].x;
	t.v[2].y = (t.v[2].y - t.v[1].y) * p + t.v[1].y;
	t.uv[2].x = (t.uv[2].x - t.uv[1].x) * p + t.uv[1].x;
	t.uv[2].y = (t.uv[2].y - t.uv[1].y) * p + t.uv[1].y;

	Render::DrawTriangle(t);

	Render::PopMatrix();
}
