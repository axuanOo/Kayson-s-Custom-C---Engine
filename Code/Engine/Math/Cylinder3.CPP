#include "Engine/Math/Cylinder3.hpp"

Vec3 Cylinder3::GetDirection() const
{
	return (m_end - m_start).GetNormalized();
}

float Cylinder3::GetLength() const
{
	return (m_end - m_start).GetLength();
}
