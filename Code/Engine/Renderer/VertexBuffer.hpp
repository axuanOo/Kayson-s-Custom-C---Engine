#pragma once
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class VertexBuffer
{
	friend class Renderer;

public:
	VertexBuffer(size_t size);
	VertexBuffer(const VertexBuffer& copy) = delete;
	virtual ~VertexBuffer();

	ID3D11Buffer*				m_buffer = nullptr;
	ID3D11ShaderResourceView*	m_shaderResourceView = nullptr;

	bool m_isLinePrimitive = false;
	unsigned int m_stride = 0;
	size_t m_size = 0;
};