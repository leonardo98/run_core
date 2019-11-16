#include "CoreTriple.h"

#include "Render.h"
#include "engine.h"

const int BATCH_SIZE = 1024;

FPoint2D _verticesWorkingCopy[BATCH_SIZE];

unsigned int &VertexBuffer::Index(size_t inIndex)
{
    assert(inIndex < index.size());
    return (index[inIndex]);
}

FPoint2D &VertexBuffer::VertXY(size_t index)
{
    assert(index < v.size());
    return v[index];
}

FPoint2D &VertexBuffer::VertUV(size_t index)
{
    assert(index < uv.size());
    return uv[index];
}

void VertexBuffer::Reserve(unsigned int vert, unsigned int tri)
{
    v.reserve(vert);
    uv.reserve(vert);
    color.reserve(vert);
    index.reserve(tri);
}

void VertexBuffer::Init(unsigned int vert/* = 0*/, unsigned int tri/* = 0*/)
{
    v.resize(vert);
    uv.resize(vert);
    color.resize(vert);
    index.resize(tri);
}

void VertexBuffer::AddTriple(const CoreTriple &t, unsigned int col, bool useMatrix /* = false */)
{
    unsigned int size = v.size();
    v.resize(size + 3);
    uv.resize(size + 3);
    color.resize(size + 3);

    unsigned int count = index.size();
    index.resize(count + 3);

    color[size + 2] = color[size + 1] = color[size + 0] = col;

    if (useMatrix)
    {
        Render::GetCurrentMatrix().Mul(t.v[0], v[size]);
        Render::GetCurrentMatrix().Mul(t.v[1], v[size + 1]);
        Render::GetCurrentMatrix().Mul(t.v[2], v[size + 2]);
    }
    else
    {
        v[size]     = t.v[0];
        v[size + 1] = t.v[1];
        v[size + 2] = t.v[2];
    }

    uv[size]     = t.uv[0];
    uv[size + 1] = t.uv[1];
    uv[size + 2] = t.uv[2];

    index[count] = size;
    index[count + 1] = size + 1;
    index[count + 2] = size + 2;
}

void VertexBuffer::AddQuad(const CoreQuad &q, unsigned int col, bool useMatrix /* = false */)
{
    unsigned int size = v.size();
    v.resize(size + 4);
    uv.resize(size + 4);
    color.resize(size + 4);

    unsigned int count = index.size();
    index.resize(count + 6);

    color[size + 3] = color[size + 2] = color[size + 1] = color[size + 0] = col;

    if (useMatrix)
    {
        Render::GetCurrentMatrix().Mul(q.v[0], v[size]);
        Render::GetCurrentMatrix().Mul(q.v[1], v[size + 1]);
        Render::GetCurrentMatrix().Mul(q.v[2], v[size + 2]);
        Render::GetCurrentMatrix().Mul(q.v[3], v[size + 3]);
    }
    else
    {
        v[size]     = q.v[0];
        v[size + 1] = q.v[1];
        v[size + 2] = q.v[2];
        v[size + 3] = q.v[3];
    }

    uv[size]     = q.uv[0];
    uv[size + 1] = q.uv[1];
    uv[size + 2] = q.uv[2];
    uv[size + 3] = q.uv[3];

    index[count + 3] = index[count] = size;
    index[count + 1] = index[count + 5] = size + 2;
    index[count + 2] = size + 1;
    index[count + 4] = size + 3;
}

unsigned int VertexBuffer::VertSize() const
{
    return v.size();
}

void VertexBuffer::Draw(bool useMatrix/* = true */)
{
   if (v.size() == 0)
   {
       return;
   }
   assert(0 < v.size() && v.size() < BATCH_SIZE);

   if (tex)
   {
       Engine::Instance()->SetShader(ShaderType::Textured);
        tex->bind();
   }
   else
   {
       Engine::Instance()->SetShader(ShaderType::Flat);
       glBindTexture(GL_TEXTURE_2D, 0);
   }

   PushDataToGLArrays(useMatrix);

   glBindVertexArray(_VertexArrayID);

   // 1rst attribute buffer : vertices
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
   glVertexAttribPointer(
       0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
       2,                  // size
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       0,                  // stride
       (void*)0            // array buffer offset
   );

   // 2nd attribute buffer : UVs
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, _uvbuffer);
   glVertexAttribPointer(
       1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
       2,                                // size : U+V => 2
       GL_FLOAT,                         // type
       GL_FALSE,                         // normalized?
       0,                                // stride
       (void*)0                          // array buffer offset
   );

   // 3rd attribute buffer : Colors
   glEnableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER, _colorbuffer);
   glVertexAttribPointer(
       2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
       4,                                // size : 1
       GL_UNSIGNED_BYTE,                         // type
       GL_TRUE,                         // normalized?
       0,                                // stride
       (void*)0                          // array buffer offset
   );

   // Draw the triangle !
   //glDrawArrays(GL_TRIANGLES, 0, 3); // 12*3 indices starting at 0 -> 12 triangles
   glDrawElements(GL_TRIANGLES, index.size(),  GL_UNSIGNED_INT, index.data());

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);
}

void VertexBuffer::Add(const VertexBuffer &vb)
{
   if (vb.v.size() == 0)
   {
       return;
   }
   unsigned int size = v.size();
   assert(v.size() == uv.size());
   v.resize(size + vb.v.size());
   uv.resize(size + vb.uv.size());
   unsigned int indexSize = index.size();
   index.resize(indexSize + vb.index.size());

   for (unsigned int i = 0; i < vb.v.size(); ++i)
   {
       v[size + i] = vb.v[i];
       uv[size + i] = vb.uv[i];
   }

   for (unsigned int i = 0; i < vb.index.size(); ++i)
   {
       index[indexSize + i] = size + vb.index[i];
   }
}

void VertexBuffer::Union(const VertexBuffer &vb)
{
  assert(false);// what for? 

   // if (vb.Size() == 0)
   // {
   //     return;
   // }
   // _vertices.resize(_sizeVert + vb._sizeVert);
   // _texCoords.resize(_sizeVert + vb._sizeVert);
   // _indexes.resize(_sizeIndex + vb._sizeIndex);

   // // íîâûå âåðøèíû íóæíî ïðîâåðèòü - âäðóã òàêàÿ óæå åñòü
   // int counter = _sizeVert;

   // for (unsigned int i = 0; i < vb._sizeIndex; ++i)
   // {
   //     FPoint2D p(vb._vertices[vb._indexes[i]]);
   //     FPoint2D uv(vb._texCoords[vb._indexes[i]]);

   //     int index = -1;
   //     for (unsigned int j = 0; j < counter; ++j)
   //     {
   //         if ((_vertices[j] - p).Length() < 1e-5
   //             && (_texCoords[j] - uv).Length() < 1e-5)
   //         {
   //             index = (int)j;
   //             break;
   //         }
   //     }

   //     if (index == -1)
   //     {
   //         _vertices[counter] = p;
   //         _texCoords[counter] = uv;
   //         _indexes[_sizeIndex + i] = counter;
   //         ++counter;
   //     }
   //     else
   //     {
   //         _indexes[_sizeIndex + i] = index;
   //     }
   // }

   // _sizeVert = counter;
   // _sizeIndex += vb._sizeIndex;
}

VertexBuffer::VertexBuffer()
    : tex(nullptr)
{
    InitGLArrays();
}

VertexBuffer::VertexBuffer(const VertexBuffer &l)
{
    tex = l.tex;
    v = l.v;
    uv = l.uv;
    color = l.color;
    index = l.index;

    InitGLArrays();
}

VertexBuffer &VertexBuffer::operator=(const VertexBuffer &l)
{
    tex = l.tex;
    v = l.v;
    uv = l.uv;
    color = l.color;
    index = l.index;

    return *this;
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &_vertexbuffer);
    glDeleteBuffers(1, &_uvbuffer);
    glDeleteBuffers(1, &_colorbuffer);
    glDeleteVertexArrays(1, &_VertexArrayID);
}

void VertexBuffer::InitGLArrays()
{
    glGenVertexArrays(1, &_VertexArrayID);
    glBindVertexArray(_VertexArrayID);

    glGenBuffers(1, &_vertexbuffer);

    glGenBuffers(1, &_uvbuffer);

    glGenBuffers(1, &_colorbuffer);
}

void VertexBuffer::PushDataToGLArrays(bool useMatrix)
{
    if (useMatrix)
    {
        for (unsigned int i = 0; i < v.size(); ++i)
        {
            Render::GetCurrentMatrix().Mul(v[i].x, v[i].y
                                           , _verticesWorkingCopy[i].x, _verticesWorkingCopy[i].y);
        }
    }

    glBindVertexArray(_VertexArrayID);

    glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(FPoint2D), useMatrix ? _verticesWorkingCopy : v.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, _uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(FPoint2D), uv.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, _colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(unsigned int), color.data(), GL_STATIC_DRAW);
}
