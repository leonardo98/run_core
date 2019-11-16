#include "sprite.h"
#include "../Render.h"
#include "../Math.h"
#include "../FPoint2D.h"

void Sprite::Set(GLTexture *texture, float x, float y, float width, float height) {
    _quad.tex = texture;
    if (_quad.tex == NULL) {
        return;
    }
    _originX = x;
    _originY = y;
    if (width <= 0.f)
    {
        _originWidth = width = texture->Width();
        _originHeight = height = texture->Height();
    }
    else
    {
        _originWidth = width;
        _originHeight = height;
    }

    _quad.v[0].Set(0, 0);
    _quad.v[1].Set(width, 0);
    _quad.v[2].Set(width, height);
    _quad.v[3].Set(0, height);
    _quad.uv[0].Set(x / texture->Width(), y / texture->Height());
    _quad.uv[1].Set((x + width)/ texture->Width(), y / texture->Height());
    _quad.uv[2].Set((x + width)/ texture->Width(), (y + height) / texture->Height());
    _quad.uv[3].Set(x / texture->Width(), (y + height) / texture->Height());

    _matrix.Unit();
}
    
void Sprite::Render() {
    Render::DrawQuad(_quad);
}

void Sprite::Render(float x, float y) {
    Render::PushMatrix();
    Render::MatrixMove(x, y);
    Render();
    Render::PopMatrix();
}

void Sprite::GetTextureRect(float &x, float &y, float &w, float &h) const
{
    x = _originX;
    y = _originY;
    w = _originWidth;
    h = _originHeight;
}

Sprite::Sprite(GLTexture *texture, float x/* = 0.f*/, float y/* = 0.f*/, float width/* = 0.f*/, float height/* = 0.f*/, float offsetX/* = 0.f*/, float offsetY/* = 0.f*/)
    : GLTexture2D(width, height)
{
    m_texture = texture->GetGLTextureUint();
    Set(texture, x, y, width, height);
    for (unsigned int i = 0; i < 4; ++i)
    {
        FPoint2D &a = _quad.v[i];
        a.x += offsetX;
        a.y += offsetY;
    }
}

void Sprite::Render(const FPoint2D &pos)
{
    Render(pos.x, pos.y);
}

void Sprite::Render(const Matrix &m)
{
    Render::PushMatrix();
    Render::MatrixMul(m);
    Render();
    Render::PopMatrix();
}

void Sprite::GetUVRect(Rect &rect) const
{
    float w = static_cast<float>(_quad.tex->Width());
    float h = static_cast<float>(_quad.tex->Height());

    rect.Init(_originX / w, _originY / h, (_originX + _originWidth) / w, (_originY + _originHeight) / h);
}
