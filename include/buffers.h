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

struct VBufferElement
{
    GLuint type;
    GLuint count;
};

// Class serving as a helper for elements held in a vertex buffer
class VBufferLayout
{
public:
    VBufferLayout()
    {

    }

    template<typename T>
    void addElement(GLuint count)
    {
        std::runtime_error("Illigal argument");
    }
    
    template<>
    void addElement<GLfloat>(GLuint count)
    {
        m_elements.emplace_back(GL_FLOAT, count);
        m_stride += count * sizeof(GLuint);
    }

    template<>
    void addElement<GLuint>(GLuint count)
    {
        m_elements.emplace_back(GL_UNSIGNED_INT, count);
        m_stride += count * sizeof(GLuint);
    }

    const std::vector<VBufferElement>& elements() const
    {
        return m_elements;
    }

    GLuint stride() const
    {
        return m_stride;
    }

private:
    std::vector<VBufferElement> m_elements;
    GLuint m_stride{};
};


// Class abstracting vertex array
class VArray
{
public:
    VArray();
    ~VArray();

    void addBuffer(const VBuffer& vb, const VBufferLayout& layout);
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

#endif