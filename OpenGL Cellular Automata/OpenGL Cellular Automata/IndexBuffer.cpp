#include "IndexBuffer.h"

#include "ErrorHandling.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : m_Count(count) {

    glGenBuffers(1, &m_rendererID);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);

}

IndexBuffer::~IndexBuffer() {

    GLCall(glDeleteBuffers(1, &m_rendererID));
}

void IndexBuffer::Bind() const {

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
}

void IndexBuffer::Unbind() const {

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
