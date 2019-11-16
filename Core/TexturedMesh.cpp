#include "TexturedMesh.h"
#include "../Core/Core.h"
#include "../Core/Math.h"

TexturedMesh::TexturedMesh(rapidxml::xml_node<> *xe)
	: BeautyBase(xe)
{
	rapidxml::xml_attribute<> *tmp = xe->first_attribute("texture");
	if (tmp)
	{
		_fileName = tmp->value();
		_texture = Core::getTexture(_fileName);
	}
	else
	{
		_texture = 0;
	}
	rapidxml::xml_node<> *mesh = xe->first_node("mesh");
    if (mesh)
    {
        _vb.tex = _texture ? _texture : 0;
        int vert = atoi(mesh->first_attribute("vert")->value());
        int ind = atoi(mesh->first_attribute("ind")->value());
		_vb.Init(vert, ind);

        unsigned int count = 0;
        for (rapidxml::xml_node<> *i = mesh->first_node(); i; i = i->next_sibling())
        {
			rapidxml::xml_attribute<> *geom = i->first_attribute("geom");
			if (geom)
			{
				sscanf(geom->value(), "%g;%g;%g;%g"
                        , &(_vb.VertXY(count).x)
                        , &(_vb.VertXY(count).y)
                        , &(_vb.VertUV(count).x)
                        , &(_vb.VertUV(count).y)
						);
			}
			else
			{
                _vb.VertXY(count).x = atof(i->first_attribute("x")->value());
                _vb.VertXY(count).y = atof(i->first_attribute("y")->value());
                _vb.VertUV(count).x = atof(i->first_attribute("tx")->value());
                _vb.VertUV(count).y = atof(i->first_attribute("ty")->value());
			}
            count++;
        }
		assert(count == vert);

	    rapidxml::xml_node<> *indexesXe = xe->first_node("indexes");
		rapidxml::xml_attribute<> *indexArray = indexesXe->first_attribute("array");
		if (indexArray)
		{
			std::string array = indexArray->value();
			std::string::size_type start = 0;
			std::string::size_type sigma = array.find(";");
			count = 0;
			int index;
			while (sigma != std::string::npos)
			{
				assert(count < ind);
				sscanf(array.substr(start, sigma - start).c_str(), "%i", &index);
                _vb.Index(count) = index;
				start = sigma + 1;
				sigma = array.find(";", start);
				count++;
			}
			assert(ind == count);
		}
		else
		{
			count = 0;
			for (rapidxml::xml_node<> *i = indexesXe->first_node(); i; i = i->next_sibling())
			{
				rapidxml::xml_attribute<> *tmp = i->first_attribute("v3");
				if (tmp)
				{
                    _vb.Index(count + 3) = _vb.Index(count + 0) = atoi(i->first_attribute("v0")->value());
                    _vb.Index(count + 5) = _vb.Index(count + 1) = atoi(i->first_attribute("v2")->value());
                    _vb.Index(count + 2) = atoi(i->first_attribute("v1")->value());
                    _vb.Index(count + 4) = atoi(tmp->value());
					count += 6;
				}
				else
				{
                    _vb.Index(count++) = atoi(i->first_attribute("v0")->value());
                    _vb.Index(count++) = atoi(i->first_attribute("v1")->value());
                    _vb.Index(count++) = atoi(i->first_attribute("v2")->value());
				}
			}
			assert(ind == count);
		}
    }
	SearchMinMax();

	assert(fabs(_scale.x - 1.f) < 1e-3);
	assert(fabs(_scale.y - 1.f) < 1e-3);
}

TexturedMesh::TexturedMesh(const TexturedMesh &l)
	: BeautyBase(l)
{
	_fileName = l._fileName;
	_texture = l._texture;
	_vb = l._vb;
	_min = l._min;
	_max = l._max;

	assert(fabs(_scale.x - 1.f) < 1e-3);
	assert(fabs(_scale.y - 1.f) < 1e-3);
}

TexturedMesh::TexturedMesh(const std::string &textureName, const FPoint2D pos, const Besier &besier)
	: BeautyBase("", pos, FPoint2D(pos.x + 1, pos.y))
{
	_fileName = textureName;
	_texture = Core::getTexture(_fileName);

	std::vector<FPoint2D> dots;
	float ln = besier.Length();
	int n = std::max(2, Math::round(ln / _texture->Width() * 2));
	besier.RecalcWithNumber(dots, n);
	Render::DrawStripe(dots, _texture, 0.f, 1.f, 0.f, 0.5f, 0, dots.size(), &_vb);
    _vb.tex = _texture;
	SearchMinMax();

	assert(fabs(_scale.x - 1.f) < 1e-3);
	assert(fabs(_scale.y - 1.f) < 1e-3);
}

void TexturedMesh::SearchMinMax()
{
    assert(_vb.VertSize() > 0);
    _min.x = _max.x = _vb.VertXY(0).x;
    _min.y = _max.y = _vb.VertXY(0).y;
    for (unsigned int i = 1; i < _vb.VertSize(); ++i)
	{
        if (_vb.VertXY(i).x < _min.x)
		{
            _min.x = _vb.VertXY(i).x;
		}
        if (_vb.VertXY(i).y < _min.y)
		{
            _min.y = _vb.VertXY(i).y;
		}
        if (_vb.VertXY(i).x > _max.x)
		{
            _max.x = _vb.VertXY(i).x;
		}
        if (_vb.VertXY(i).y > _max.y)
		{
            _max.y = _vb.VertXY(i).y;
		}
	}
}

std::string TexturedMesh::Type() const 
{
	return "TexturedMesh"; 
}

void TexturedMesh::Draw()
{

	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	Render::MatrixScale(_scale.x, _scale.y); //need global test

	float x1, y1;
	float x2, y2;
	Render::GetCurrentMatrix().Mul(_max, x1, y1);
	Render::GetCurrentMatrix().Mul(_min, x2, y2);
	
	if ((x1 < 0 && x2 < 0)
		|| (y1 < 0 && y2 < 0)
		|| (x1 > Render::ScreenWidth() && x2 > Render::ScreenWidth())
		|| (y1 > Render::ScreenHeight() && y2 > Render::ScreenHeight()))
	{
		Render::PopMatrix();
		return;
	}

	Render::PushColorAndMul(_color);
	Render::DrawVertexBuffer(_vb);
	Render::PopColor();

	Render::PopMatrix();

}

void TexturedMesh::ReloadTextures(bool touch)
{
	if (_fileName.size())
	{
//        if (touch)
//            _texture->GetTexture()->Touch();
//        else
//        {
            _texture = Core::getTexture(_fileName);
            _vb.tex = _texture;
//        }
	}
	else
	{
		_texture = 0;
	}	
}

void TexturedMesh::ShiftU(float u)
{
    for (unsigned int i = 0; i < _vb.VertSize(); ++i)
	{
        _vb.VertUV(i).x += u;
	}
}

//
//bool ColoredPolygon::PixelCheck(const FPoint2D &point) { 
//	if (Math::Inside(point, _screenDots)) {
//		return true;
//	}
//	return false;
//}
