#include "buffers.h"

GLuint getSizeOfElement(const VBufferElement& element)
{
    if(element.type == GL_FLOAT)
    {
        return element.count * sizeof(GLfloat);
    }
    else if(element.type == GL_UNSIGNED_INT)
    {
        return element.count * sizeof(GLuint);
    }

    return 0;
}

VBuffer::VBuffer()
{
    glGenBuffers(1, &m_id);
}

VBuffer::VBuffer(const void* data, std::size_t size, GLenum type)
{
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, type);
}

VBuffer::~VBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void VBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBuffer::addData(const void* data, std::size_t size, GLenum type)
{
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, type);
}

VArray::VArray()
{
    glGenVertexArrays(1, &m_id);
}

VArray::~VArray()
{
    glDeleteVertexArrays(1, &m_id);
}

void VArray::addBuffer(const VBuffer& vb, std::size_t stride, const VBufferLayout& elements)
{
    bind();
    vb.bind();
    for(GLuint i = 0; i < elements.size(); ++i)
    {
        const VBufferElement element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, GL_FALSE, stride, (const void*)(element.offset));
    }
}

void VArray::bind() const
{
    glBindVertexArray(m_id);
}

void VArray::unbind() const
{
    glBindVertexArray(0);
}

EBuffer::EBuffer()
{
    
}

EBuffer::EBuffer(const GLuint* data, std::size_t count, GLenum type)
    : m_count{static_cast<GLuint>(count)}
{
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, type);
}

EBuffer::~EBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void EBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void EBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

UBuffer::UBuffer()
{
    glGenBuffers(1, &m_id);
}

UBuffer::UBuffer(std::size_t size, GLenum type)
{
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, type);
}

UBuffer::~UBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void UBuffer::bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
}

void UBuffer::unbind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}