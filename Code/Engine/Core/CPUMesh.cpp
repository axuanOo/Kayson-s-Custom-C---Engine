#include "Engine/Core/CPUMesh.hpp"
#include "Engine/Core/ObjLoader.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include <windows.h>
#include <chrono>

CPUMesh::CPUMesh(const std::string& objFileName, const Mat44& transform)
{
	Load(objFileName, transform);
}

CPUMesh::~CPUMesh()
{

}

void CPUMesh::Load(const std::string& objFileName, const Mat44& transform)
{
	m_vertexes.clear();
	m_indexes.clear();
	m_hasUv = false;
	m_hasNormal = false;

	ObjLoader::Load(objFileName, m_vertexes, m_indexes, m_hasNormal, m_hasUv, transform);

	if (m_hasUv)
	{
		CalculateTangentSpaceBasisVectors(m_vertexes, m_indexes, !m_hasNormal);
	}
}
