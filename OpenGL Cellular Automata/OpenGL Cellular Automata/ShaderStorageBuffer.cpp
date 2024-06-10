#include "ShaderStorageBuffer.h"

#include "ErrorHandling.h"

ShaderStorageBuffer::ShaderStorageBuffer(const void* data, unsigned int size) {

    glGenBuffers(1, &m_rendererID);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_rendererID);

    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_rendererID);
}

ShaderStorageBuffer::~ShaderStorageBuffer() {

    GLCall(glDeleteBuffers(1, &m_rendererID));
}

void ShaderStorageBuffer::Bind() const {

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_rendererID);
}

void ShaderStorageBuffer::Unbind() const {

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBuffer::UpdateColors(int offset, unsigned int size, float* data) const {

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset * 4 * sizeof(float), size, &data[offset * 4]);
}