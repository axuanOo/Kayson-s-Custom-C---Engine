#pragma once
#include "Engine/Math/Vec3.hpp"

struct Cylinder3
{
	Vec3 GetDirection() const;
	float GetLength() const;
public:
	Vec3 m_start;
	Vec3 m_end;
	float m_radius;
};