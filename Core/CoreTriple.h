#ifndef CORE_TRIPLE_H
#define CORE_TRIPLE_H

#include "FPoint2D.h"

#include <GL/glew.h>

#include <vector>
#include <assert.h>

class GLTexture;

struct CoreTriple
{
    FPoint2D v[3];
    FPoint2D uv[3];
    GLTexture *tex;
};

struct CoreQuad
{
    FPoint2D v[4];
    FPoint2D uv[4];
    GLTexture *tex;
};

class VertexBuffer
{
public:
    GLTexture *tex;

    unsigned int &Index(size_t inIndex);
    FPoint2D &VertXY(size_t index);
    FPoint2D &VertUV(size_t index);

    VertexBuffer();
    VertexBuffer(const VertexBuffer &l);
    VertexBuffer &operator=(const VertexBuffer &l);
    virtual ~VertexBuffer();

    void Reserve(unsigned int vert, unsigned int tri);
    void Init(unsigned int vert = 0, unsigned int tri = 0);
    void AddTriple(const CoreTriple &t, unsigned int col, bool useMatrix = false);
    void AddQuad(const CoreQuad &q, unsigned int col, bool useMatrix = false);
    void Add(const VertexBuffer &vb);
    void Union(const VertexBuffer &vb);// slow add
    void Draw(bool useMatrix = true);
    unsigned int VertSize() const;
private:

    static unsigned int _sizeWorkingVert;

    std::vector<FPoint2D> v;
    std::vector<FPoint2D> uv;
    std::vector<unsigned int> color;
    std::vector<unsigned int> index;

    void InitGLArrays();
    void PushDataToGLArrays(bool useMatrix);
    GLuint _VertexArrayID;
    GLuint _vertexbuffer;
    GLuint _uvbuffer;
    GLuint _colorbuffer;

    friend class Render;
};

extern const int BATCH_SIZE;

#endif//CORE_TRIPLE_H
