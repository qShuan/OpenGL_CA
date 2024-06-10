#pragma once
class VertexBuffer {

private:
	unsigned m_rendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;

	void UpdateVertex(const void* data, unsigned int size) const;
};