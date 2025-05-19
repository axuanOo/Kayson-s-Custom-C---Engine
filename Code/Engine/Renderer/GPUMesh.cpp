#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"

GPUMesh::GPUMesh(Renderer* renderer)
	:m_renderer(renderer)
{

}

GPUMesh::~GPUMesh()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;
}

void GPUMesh::Create(const CPUMesh* cpuMesh)
{
	if (m_renderer)
	{
		if (m_vertexBuffer != nullptr)
		{
			delete m_vertexBuffer;
		}
		if (m_indexBuffer != nullptr)
		{
			delete m_indexBuffer;
		}
		m_indexCount = (unsigned int)cpuMesh->m_indexes.size();
		m_vertexBuffer = m_renderer->CreateVertexBuffer(sizeof(cpuMesh->m_vertexes[0]) * cpuMesh->m_vertexes.size());
		m_indexBuffer = m_renderer->CreateIndexBuffer(sizeof(cpuMesh->m_indexes[0]) * cpuMesh->m_indexes.size());
		m_renderer->CopyCPUToGPU(cpuMesh->m_vertexes.data(), sizeof(cpuMesh->m_vertexes[0]) * cpuMesh->m_vertexes.size(), m_vertexBuffer,
			cpuMesh->m_indexes.data(), sizeof(cpuMesh->m_indexes[0]) * cpuMesh->m_indexes.size(), m_indexBuffer);
	}
}

void GPUMesh::Render() const
{
	m_renderer->SetStatesIfChanged();
	m_renderer->DrawVertexBufferAndIndexBuffer(m_vertexBuffer, m_indexBuffer, (int)m_indexCount, 0, VertexType::Vertex_PCUTBN);
}
