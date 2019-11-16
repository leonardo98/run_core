// device specified function

#include "Render.h"

Matrix Render::_matrixStack[MAX_MATRIX_AMOUNT];
unsigned int Render::_currentMatrix = 0;
bool Render::addBlendingGlobal = false;

VertexBuffer * Render::_batch = nullptr;

unsigned int Render::_currentColor[MAX_COLOR_AMOUNT];
unsigned int Render::_currentColorIndex;

void Render::PushMatrix() {
    //assert((_currentMatrix + 1)< MAX_MATRIX_AMOUNT);
    ++_currentMatrix;
    _matrixStack[_currentMatrix] = _matrixStack[_currentMatrix - 1];
}

void Render::PopMatrix() {
    //assert(_currentMatrix > 0);
    --_currentMatrix;
}

void Render::MatrixMove(float x, float y) {
    _matrixStack[_currentMatrix].Move(x, y);
}

void Render::MatrixRotate(float angle) {
    _matrixStack[_currentMatrix].Rotate(angle);
}

void Render::MatrixScale(float sx, float sy) {
    _matrixStack[_currentMatrix].Scale(sx, sy);
}

void Render::MatrixScale(float s) {
    _matrixStack[_currentMatrix].Scale(s, s);
}
void Render::MatrixMul(const Matrix &matrix) {
    _matrixStack[_currentMatrix].Mul(matrix);
}

void Render::SetMatrixUnit() {
    _matrixStack[_currentMatrix].Unit();
}

void Render::SetMatrix(const Matrix &matrix) {
    _matrixStack[_currentMatrix] = matrix;
}

const Matrix & Render::GetCurrentMatrix() {
    return _matrixStack[_currentMatrix];
}

void Render::PrintString(float x, float y, const std::string &text, int align) {
    FlushPrimitives();
    //std::string fontName("data\\font2.fnt");
    //fontName = GetDataDir() + fontName;
    //Font *font;
    //if (_fonts.find(fontName) == _fonts.end()) {
    //    _fonts[fontName] = new Font(fontName.c_str());
    //}
    //font = _fonts[fontName];
    //font->Render(x, y, align, text.c_str());
}

void Render::PrintString(float x, float y, std::string fontName, const std::string &text, unsigned int color) {
    FlushPrimitives();
    //if (fontName == "") {
    //    fontName = "data\\font2.fnt";
    //}
    //if (fontName[1] != ':') {
    //    fontName = GetDataDir() + fontName;
    //}
    //Font *font;
    //if (_fonts.find(fontName) == _fonts.end()) {
    //    _fonts[fontName] = new Font(fontName.c_str());
    //}
    //font = _fonts[fontName];
    //font->SetColor(color);
    //font->Render(x, y, HGETEXT_CENTER, text.c_str());
}

int Render::GetFontHeight(const std::string &fontName) {
    //std::string fn = GetDataDir() + fontName;
    //if (_fonts.find(fn) == _fonts.end()) {
    //    _fonts[fn] = new hgeFont(fn.c_str());
    //}
    //hgeFont *font = _fonts[fn];
    //return font->GetHeight();
    return 1;
}

int Render::GetStringWidth(const std::string &fontName, const char *s) {
    //if (_fonts.find(fontName) == _fonts.end()) {
    //    _fonts[fontName] = new Font(fontName.c_str());
    //}
    //hgeFont *font = _fonts[fontName];
    //return font->GetStringWidth(s);
    return 1;
}

bool Render::InitApplication() {
    _currentColor[_currentColorIndex = 0] = 0xFFFFFFFF;
    _currentMatrix = 0;
    _batch = new VertexBuffer();
    SetMatrixUnit();
    return true;
}

void Render::DrawBar(float x, float y, float width, float height, unsigned int color) {
    FlushPrimitives();
    static VertexBuffer tmp_quad_for_bar;
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    Render::PushColorAndMul(color);
    tmp_quad_for_bar.Init(4, 6);
    tmp_quad_for_bar.VertXY(0).x = x;
    tmp_quad_for_bar.VertXY(0).y = y;
    tmp_quad_for_bar.VertXY(1).x = x + width;
    tmp_quad_for_bar.VertXY(1).y = y;
    tmp_quad_for_bar.VertXY(2).x = x + width;
    tmp_quad_for_bar.VertXY(2).y = y + height;
    tmp_quad_for_bar.VertXY(3).x = x;
    tmp_quad_for_bar.VertXY(3).y = y + height;
    for (int i = 0; i < 4; i++) {
        tmp_quad_for_bar.VertUV(i).x = 0;
        tmp_quad_for_bar.VertUV(i).y = 0;
    }
    tmp_quad_for_bar.Draw();
    glEnable(GL_TEXTURE_2D);
    Render::PopColor();
}

void Render::DrawBarTextured(float x, float y, float width, float height, unsigned int color) {
    FlushPrimitives();
    static VertexBuffer tmp_quad_for_bar;
    Render::PushColorAndMul(color);
    tmp_quad_for_bar.Init(4, 6);
    tmp_quad_for_bar.VertXY(0).x = x;
    tmp_quad_for_bar.VertXY(0).y = y;
    tmp_quad_for_bar.VertXY(1).x = x + width;
    tmp_quad_for_bar.VertXY(1).y = y;
    tmp_quad_for_bar.VertXY(2).x = x + width;
    tmp_quad_for_bar.VertXY(2).y = y + height;
    tmp_quad_for_bar.VertXY(3).x = x;
    tmp_quad_for_bar.VertXY(3).y = y + height;
    for (int i = 0; i < 4; i++) {
        tmp_quad_for_bar.VertUV(i).x = (tmp_quad_for_bar.VertXY(i).x - x) / width;
        tmp_quad_for_bar.VertUV(i).y = 1 - (tmp_quad_for_bar.VertXY(i).y - y) / height;
    }
    tmp_quad_for_bar.Draw();
    Render::PopColor();
}


void Render::Line(float x1, float y1, float x2, float y2, unsigned int color) {
    FlushPrimitives();
    GetCurrentMatrix().Mul(x1, y1, x1, y1);
    GetCurrentMatrix().Mul(x2, y2, x2, y2);
    Render::PushColorAndMul(color);
    unsigned int currentColor = Render::GetColor();
    glColor4ubv( (GLubyte*)&currentColor );
    glBegin(GL_LINES);
    glVertex2d(x1, y1);
    glVertex2d(x2, y2);
    glEnd();
    Render::PopColor();
}

void Render::SetAlpha(float fAlpha) {
    unsigned int alpha = fAlpha * 0xFF;
    SetColor((alpha << 24) | (GetColor() & 0xFFFFFF));
}

void Render::SetBlendMode(unsigned int blendMode)
{
    bool newBlending = (blendMode != 0);
    if (newBlending == addBlendingGlobal)
    {
        return;
    }

    FlushPrimitives();

    addBlendingGlobal = newBlending;
    if (addBlendingGlobal)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
    }
}

void Render::Circle(float x, float y, float r, unsigned int color) {
    FlushPrimitives();
    Render::Line(x, y, x + r, y, color);
    float angle = 0.f;
    float xs = sin(angle) * r;
    float ys = cos(angle) * r;
    while (angle < M_PI * 2) {
        angle += static_cast<float>(M_PI)/ 10.f;
        float xe = sin(angle) * r;
        float ye = cos(angle) * r;
        Render::Line(x + xs, y + ys, x + xe, y + ye, color);
        xs = xe;
        ys = ye;
    }
}

void Render::DrawStripe(const std::vector<FPoint2D> &dots, GLTexture *texture, float y1, float y2, float x1, float stepX, int startI, int endI, VertexBuffer *vb)
{
    assert(dots.size() >= 2);

    VertexBuffer quad;
    quad.Init(4, 6);
    quad.VertUV(2).y = quad.VertUV(3).y = y1;
    quad.VertUV(1).y = quad.VertUV(0).y = y2;

    FPoint2D f;
    FPoint2D n;
    float lenX = x1;

    float h2 = fabs(y2 - y1) / 2 * texture->Height();

    for (int i = startI; i < endI - 1; ++i)
    {
        f.x = dots[i].y - dots[i + 1].y;
        f.y = dots[i + 1].x - dots[i].x;
        f *= h2 / f.Length();

        if ((i + 2) < static_cast<int>(dots.size()))
        {
            n.x = dots[i + 1].y - dots[i + 2].y;
            n.y = dots[i + 2].x - dots[i + 1].x;
            n *= h2 / n.Length();
        }
        else
        {
            n = f;
        }

        quad.VertUV(3).x = quad.VertUV(0).x = lenX;
        lenX += stepX;
        quad.VertUV(2).x = quad.VertUV(1).x = lenX;

        quad.VertXY(0).x = dots[i].x + f.x;
        quad.VertXY(0).y = dots[i].y + f.y;

        quad.VertXY(1).x = dots[i + 1].x + n.x;
        quad.VertXY(1).y = dots[i + 1].y + n.y;

        quad.VertXY(2).x = dots[i + 1].x - n.x;
        quad.VertXY(2).y = dots[i + 1].y - n.y;

        quad.VertXY(3).x = dots[i].x - f.x;
        quad.VertXY(3).y = dots[i].y - f.y;

        if (vb == NULL)
        {
            quad.Draw();
        }
        else
        {
            vb->Add(quad);
        }
    }
}

void Render::DrawVertexBuffer(VertexBuffer &vb)
{
    unsigned int color = Render::GetColor();
    FlushPrimitives();
    for (auto &c : vb.color)
    {
        c = color;
    }
    vb.Draw();
}

void Render::SetFiltering(bool value)
{
    GLTexture2D::filteringGlobal = value;
}

bool Render::GetFiltering()
{
    return GLTexture2D::filteringGlobal;
}

int Render::_screenWidth = 0;
int Render::_screenHeight = 0;

void Render::SetScreenSize(int width, int height)
{
    _screenWidth = width;
    _screenHeight = height;
}

void Render::MulColor(unsigned int color)
{
//#ifdef WIN32
//    color = (0xFF00FF00 & color)
//                | ((0x00FF0000 & color) >> 16)
//                | ((0x000000FF & color) << 16);
//#endif
    if (color == 0xFFFFFFFF)
    {
        return;
    }
    unsigned int currentColor = GetColor();
    if (currentColor == 0xFFFFFFFF)
    {
        SetColor(color);
        return;
    }
    SetColor(
                (((((currentColor & 0xFF000000) >> 24) * ((color & 0xFF000000) >> 24)) / 0xFF) << 24)
                    | (((((currentColor & 0xFF0000) >> 16) * ((color & 0xFF0000) >> 16)) / 0xFF) << 16)
                    | (((((currentColor & 0xFF00) >> 8) * ((color & 0xFF00) >> 8)) / 0xFF) << 8)
                    | ((currentColor & 0xFF) * (color & 0xFF) / 0xFF)
            );
}

void Render::SetColor(unsigned int color) {
//#ifdef WIN32
//    _currentColor[_currentColorIndex] = (0xFF00FF00 & color)
//                                        | ((0x00FF0000 & color) >> 16)
//                                        | ((0x000000FF & color) << 16);
//#else
    _currentColor[_currentColorIndex] = color;
    if ((color & 0xFF) > 3 * ((color & 0xFF0000) >> 16))
    {
        int i = 0;
    }
//#endif
}

unsigned int Render::GetColor() {
//#ifdef WIN32
//    return (0xFF00FF00 & _currentColor[_currentColorIndex])
//            | ((0x00FF0000 & _currentColor[_currentColorIndex]) >> 16)
//            | ((0x000000FF & _currentColor[_currentColorIndex]) << 16);
//#else
    return _currentColor[_currentColorIndex];
//#endif
}

void Render::PrintStringScaled(float x, float y, float sx, float sy, std::shared_ptr<CoreFont> &font, const char *text, int xalign /*= 0*/, int yalign /*= 0*/)
{
    FlushPrimitives();
    PushMatrix();
    MatrixMove(x, y);
    MatrixScale(sx, sy);
    if (yalign != 0)
    {
        MatrixMove(0, yalign * font->GetStringHeight(nullptr) / 2.f);
    }
    font->Render(0, 0, xalign, text);
    PopMatrix();
}

void Render::PrintStringScaled(float x, float y, float sx, float sy, std::shared_ptr<CoreFont> &font, const std::string &text, int xalign /*= 0*/, int yalign /*= 0*/)
{
    FlushPrimitives();
    PushMatrix();
    MatrixMove(x, y);
    MatrixScale(sx, sy);
    font->Render(0, 0, xalign, text.c_str());
    PopMatrix();
}

void Render::DrawQuad(const CoreQuad &quad)
{
    if (_batch->VertSize() == 0)
    {
        _batch->tex = quad.tex;
        _batch->AddQuad(quad, Render::GetColor(), true);
    }
    else if (quad.tex == _batch->tex && _batch->VertSize() + 4 < BATCH_SIZE)
    {
        _batch->AddQuad(quad, Render::GetColor(), true);
    }
    else
    {
        FlushPrimitives();
        _batch->tex = quad.tex;
        _batch->AddQuad(quad, Render::GetColor(), true);
    }
}

void Render::DrawTriangle(const CoreTriple &triple)
{
    if (_batch->VertSize() == 0)
    {
        _batch->tex = triple.tex;
        _batch->AddTriple(triple, Render::GetColor(), true);
    }
    else if (triple.tex == _batch->tex && _batch->VertSize() + 3 < BATCH_SIZE)
    {
        _batch->AddTriple(triple, Render::GetColor(), true);
    }
    else
    {
        FlushPrimitives();
        _batch->tex = triple.tex;
        _batch->AddTriple(triple, Render::GetColor(), true);
    }
}

void Render::FlushPrimitives()
{
    _batch->Draw(false);
    _batch->Init(0, 0);
}

void Render::ExitApplication()
{
    if (_batch)
    {
        delete _batch;
        _batch = nullptr;
    }
}
