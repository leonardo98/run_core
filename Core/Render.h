// device specified function
#ifndef MYENGINE_RENDER_H
#define MYENGINE_RENDER_H

#include "Matrix.h"
#include "textures.h"
#include "CoreTriple.h"
#include "CoreFont.h"

#define MAX_MATRIX_AMOUNT 32
#define MAX_COLOR_AMOUNT 32

struct LineCach
{
    FPoint2D start;
    FPoint2D end;
    unsigned int color;
};

class Render
{

private:
    static Matrix _matrixStack[MAX_MATRIX_AMOUNT];
    static unsigned int _currentMatrix;
    
    static unsigned int _currentColor[MAX_COLOR_AMOUNT];
    static unsigned int _currentColorIndex;

public:

    static void PushMatrix();
    static void PopMatrix();
    static void MatrixMove(float x, float y);
    static void MatrixMove(const FPoint2D &pos) { MatrixMove(pos.x, pos.y); }
    static void MatrixRotate(float angle);
    static void MatrixScale(float sx, float sy);
    static void MatrixScale(float s);
    static void MatrixMul(const Matrix &matrix);
    static void SetMatrixUnit();
    static void SetMatrix(const Matrix &matrix);
    static const Matrix & GetCurrentMatrix();

    static void MulColor(unsigned int color);
    static inline void PushColor() {
        assert((_currentColorIndex + 1)< MAX_COLOR_AMOUNT);
        _currentColor[_currentColorIndex + 1] = _currentColor[_currentColorIndex];
        ++_currentColorIndex;
    }
    static inline void PopColor() {
        assert(_currentColorIndex > 0);
        --_currentColorIndex;
    }
    static void PushColorAndMul(unsigned int color)
    {
        PushColor();
        MulColor(color);
    }

    static void SetColor(unsigned int color);
    static unsigned int GetColor();

    static void SetAlpha(float alpha);
    static void SetBlendMode(unsigned int blendMode);

    static inline unsigned int GetBlendMode() {
        return addBlendingGlobal ? 1 : 0;
    }

    static void PrintString(float x, float y, std::string fontName, const std::string &text, unsigned int color = 0xFFFFFFFF);
    static void PrintString(float x, float y, const std::string &text, int align);

    static void PrintStringScaled(float x, float y, float sx, float sy, std::shared_ptr<CoreFont> &font, const char *text, int xalign = 0, int yalign = 0);
    static void PrintStringScaled(float x, float y, float sx, float sy, std::shared_ptr<CoreFont> &font, const std::string &text, int xalign = 0, int yalign = 0);

    static bool InitApplication();
    static void RunApplication();
    static std::string Error();
    static int GetFontHeight(const std::string &fontName);
    static int GetStringWidth(const std::string &fontName, const char *s);
    static void ShowMessage(const char *str, const char *caption);
    static bool ShowAskMessage(const char *str, const char *caption);
    static void DrawBar(float x, float y, float width, float height, unsigned int color);
    static void DrawBarTextured(float x, float y, float width, float height, unsigned int color);

    static void Line(float x1, float y1, float x2, float y2, unsigned int color);
    static void Circle(float x, float y, float r, unsigned int color);

    static void DrawVertexBuffer(VertexBuffer &vb);
    static void DrawStripe(const std::vector<FPoint2D> &dots, GLTexture *texture, float y1, float y2, float x1, float stepX, int startI, int endI, VertexBuffer *vb = NULL);

    static void SetFiltering(bool);
    static bool GetFiltering();

    static void SetScreenSize(int width, int height);

    static int ScreenWidth() { return _screenWidth; }
    static int ScreenHeight() { return _screenHeight; }

    static void DrawQuad(const CoreQuad &quad);
    static void DrawTriangle(const CoreTriple &triple);

    static void ExitApplication();
    static void FlushPrimitives();

private:
    static bool addBlendingGlobal;
    static int _screenWidth;
    static int _screenHeight;

    static VertexBuffer *_batch;

    friend class VertexBuffer;
};


#endif//MYENGINE_RENDER_H
