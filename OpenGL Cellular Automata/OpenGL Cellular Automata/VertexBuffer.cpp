#include "VertexBuffer.h"

#include "ErrorHandling.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size) {

    glGenBuffers(1, &m_rendererID);

    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);

    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {

    GLCall(glDeleteBuffers(1, &m_rendererID));
}

void VertexBuffer::Bind() const {

    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void VertexBuffer::Unbind() const {

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::UpdateVertex(const void* data, unsigned int size) const
{
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
