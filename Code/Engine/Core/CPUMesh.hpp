#pragma once
#include <vector>
#include <string>
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Matrix44.hpp"

class CPUMesh 
{
public:

	CPUMesh() = default;
	CPUMesh(const std::string& objFileName, const Mat44& transform);
	virtual ~CPUMesh();

	void Load(const std::string& objFileName, const Mat44& transform);

public:
	std::vector<Vertex_PCUTBN>		m_vertexes;
	std::vector<unsigned int>		m_indexes;
	bool							m_hasNormal;
	bool							m_hasUv;
};
	