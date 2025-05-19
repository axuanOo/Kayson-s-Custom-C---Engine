#include "Engine/Math/Tetrahedron.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/MathUtils.hpp"

// This method should ensure all face triangles are face outwards of the tetrahedron
std::vector<Triangle3> Tetrahedron::GetFourTriangles() const
{
	Vec3 centroid = (m_PointA + m_PointB + m_PointC + m_PointD) * 0.25f;
	std::vector<Triangle3> triangles = {
		Triangle3{m_PointA, m_PointB, m_PointC},
		Triangle3{m_PointA, m_PointB, m_PointD},
		Triangle3{m_PointA, m_PointC, m_PointD},
		Triangle3{m_PointB, m_PointC, m_PointD}
	};

	for (auto& triangle : triangles) {
		Vec3 normal = triangle.Normal();
		Vec3 toCentroid = centroid - triangle.m_PointA;
		if (DotProduct3D(normal, toCentroid) > 0.0f) {
			// Flip the normal by swapping two vertices
			std::swap(triangle.m_PointB, triangle.m_PointC);
		}
	}

	return triangles;
}

Vec3 Tetrahedron::Center() const
{
	return (m_PointA + m_PointB + m_PointC + m_PointD) * 0.25f;
}
