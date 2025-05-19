#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

struct Hexagon
{
public:
	Hexagon() = delete;
	Hexagon(Vec3 const& center, float innerRadius);
	~Hexagon() = default;

public:
	float	m_innerRadius;
	Vec3	m_center;
};