#ifndef SPRITE_H
#define SPRITE_H

#include "../types.h"

#include "../CoreTriple.h"
#include "../Matrix.h"
#include "../textures.h"

class Sprite : public GLTexture2D
{
public:
    void Set(GLTexture *texture, float x = 0.f, float y = 0.f, float width = 0.f, float height = 0.f);
    void Render();
    void Render(float x, float y) override;
    inline float Width() {
        return _originWidth;
    }
    inline float Height() {
        return _originHeight;
    }
    int Width() const override {
        return static_cast<int>(_originWidth);
    }
    int Height() const override {
        return static_cast<int>(_originHeight);
    }
    void GetTextureRect(float &x, float &y, float &w, float &h) const;
    const GLTexture *GetTexture() const { return _quad.tex; }

    Sprite(GLTexture *texture, float x = 0.f, float y = 0.f, float width = 0.f, float height = 0.f, float offsetX = 0.f, float offsetY = 0.f);
    void Render(const FPoint2D &pos) override;
    void Render(const Matrix &m) override;
    void GetUVRect(Rect &rect) const override;

private:
    CoreQuad _quad;
    Matrix _matrix;
    float _originX;
    float _originY;
    float _originWidth;
    float _originHeight;
};

#endif // SPRITE_H
