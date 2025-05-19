#pragma once
#include <string>
#include <vector>
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/Vertex_PCU.hpp"


class ObjLoader 
{
public:
	static bool Load( const std::string& fileName, 
		std::vector<Vertex_PCUTBN>& out_Vertexes, std::vector<unsigned int>& out_Indexes,
		bool& out_hasNormal, bool& out_hasUVs, const Mat44& transform = Mat44());
};
	