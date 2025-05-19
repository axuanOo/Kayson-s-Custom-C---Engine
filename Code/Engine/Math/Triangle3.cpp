#include "Engine/Math/Triangle3.hpp"
#include "Engine/Math/MathUtils.hpp"

Vec3 Triangle3::EdgeAB() const
{
	return m_PointB - m_PointA;
}

Vec3 Triangle3::EdgeAC() const
{
	return m_PointC - m_PointA;
}

Vec3 Triangle3::EdgeBC() const
{
	return m_PointC - m_PointB;
}

Vec3 Triangle3::Normal() const
{
	return CrossProduct3D(EdgeAB(), EdgeAC());
}

Vec3 Triangle3::Center() const
{
	return (m_PointA + m_PointB + m_PointC) / 3.0f;
}

bool Triangle3::IsPointInFrontOfTriangle(Vec3 const& referencePos) const
{	
	return DotProduct3D(Normal(), referencePos - m_PointA) > 0.0f;
}
