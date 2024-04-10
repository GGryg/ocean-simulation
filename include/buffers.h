#ifndef BUFFERS_H
#define BUFFERS_H

#include <glad/gl.h>
#include <vector>
#include <stdexcept>

// Class abstracting vertex buffer
class VBuffer
{
public:
    VBuffer();
    VBuffer(const void* data, std::size_t size, GLenum type);
    ~VBuffer();

    void bind() const;
    void unbind() const;

    void addData(const void* data, std::size_t size, GLenum type);

private:
    GLuint m_id;
};


// Element in vertex buffer
struct VBufferElement
{
    GLuint count;
    GLenum type;
    GLenum normalized;
    GLuint offset;
};

using VBufferLayout = std::vector<VBufferElement>;

// Class abstracting vertex array
class VArray
{
public:
    VArray();
    ~VArray();

    void addBuffer(const VBuffer* vb, std::size_t size, const VBufferLayout& layout);
    void bind() const;
    void unbind() const;

private:
    GLuint m_id;
};

// Class abstracting element buffer
class EBuffer
{
public:
    EBuffer();
    EBuffer(const GLuint* data, std::size_t count, GLenum type);
    ~EBuffer();

    void bind() const;
    void unbind() const;

    inline GLuint count() const
    {
        return m_count;
    }

private:
    GLuint m_id;
    GLuint m_count;
};

class UBuffer
{
public:
    UBuffer();
    UBuffer(std::size_t size, GLenum type);
    ~UBuffer();

    void bind() const;
    void unbind() const;

private:
    GLuint m_id;
};

#endif