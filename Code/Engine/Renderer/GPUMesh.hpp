#pragma once

#include "Engine/Core/CPUMesh.hpp"

class IndexBuffer;
class VertexBuffer;
class Renderer;

class GPUMesh 
{
public:

	GPUMesh() = default;
	GPUMesh(Renderer* renderer);
	virtual ~GPUMesh();

	void Create(const CPUMesh* cpuMesh);
	void Render() const;

public:

	Renderer*		m_renderer = nullptr;
	IndexBuffer*	m_indexBuffer = nullptr;
	VertexBuffer*	m_vertexBuffer = nullptr;
	unsigned int	m_indexCount = 0;
};
	