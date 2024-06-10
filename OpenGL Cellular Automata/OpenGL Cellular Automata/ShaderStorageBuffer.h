#pragma once
class ShaderStorageBuffer {

private:
	unsigned m_rendererID;
public:
	ShaderStorageBuffer(const void* data, unsigned int size);
	~ShaderStorageBuffer();

	void Bind() const;
	void Unbind() const;

	void UpdateColors(int offset, unsigned int size, float* data) const;
};