#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Triangle3.hpp"
#include <vector>

struct Plane3;
struct Triangle3;

struct Tetrahedron
{
	std::vector<Triangle3> GetFourTriangles() const;
	Vec3 Center() const;
public:

	Vec3 m_PointA;
	Vec3 m_PointB;
	Vec3 m_PointC;
	Vec3 m_PointD;
};