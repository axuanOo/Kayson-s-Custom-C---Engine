#pragma once
#include <vector>

struct ID3D11Buffer;
struct Vertex_PCU;

class IndexBuffer
{
	friend class Renderer;

public:
	IndexBuffer(size_t size);
	IndexBuffer(const IndexBuffer& copy) = delete;
	virtual ~IndexBuffer();
	ID3D11Buffer* m_buffer = nullptr;
	size_t m_size = 0;
};