#ifndef TEXTURES_H
#define TEXTURES_H

#include "FPoint2D.h"
#include "Rect.h"
#include "Matrix.h"
#include "CoreTriple.h"

#include <GL/glew.h>

class GLTexture
{
public:
    static int globalLoadingCounter;
    GLTexture();
    virtual ~GLTexture();
    virtual void bind() const = 0;
    virtual void unbind() const = 0;
    virtual bool failed() const {return m_failed;}
    virtual void Render(const FPoint2D &pos);
    virtual void Render(float posX, float posY);
    virtual void Render(const Matrix &m);
    virtual void GetUVRect(Rect &uv) const;
    virtual int Width() const  = 0;
    virtual int Height() const  = 0;
    GLuint GetGLTextureUint() const { return m_texture; }
protected:
    GLuint m_texture;
    bool m_failed;
    unsigned int *m_alphaMask;
};

class GLTexture2D : public GLTexture
{
private:
    int _width;
    int _height;
    CoreQuad _quad;
    //void internalLoading(FIBITMAP* bits);
    void createAlphaMask(const void *);
public:
    static bool filteringGlobal;
    virtual ~GLTexture2D();
    GLTexture2D(int width, int height);
    GLTexture2D(const char *fileName);
    void load(int width, int height, const unsigned char *data);
    unsigned char *CreateBitmapCopy() const;
    virtual void bind() const;
    virtual void unbind() const;
    int Width() const override { return _width; }
    int Height() const override  { return _height; }
    GLuint glHandle() { return m_texture; }
    bool Transparency(int i, int j) const;
    void Render(float posX, float posY) override;
    void Render(const Matrix &m) override;
};

#endif // TEXTURES_H
