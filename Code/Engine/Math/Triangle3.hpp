#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

struct Triangle2 
{
public:
	Vec2 m_PointA;
	Vec2 m_PointB;
	Vec2 m_PointC;
};

struct Triangle3
{
public:
	Triangle3() = default;
	Triangle3(Vec3 const& a, Vec3 const& b, Vec3 const& c) : m_PointA(a), m_PointB(b), m_PointC(c) {};

	Vec3 EdgeAB() const;
	Vec3 EdgeAC() const;
	Vec3 EdgeBC() const;

	Vec3 Normal() const;
	Vec3 Center() const;

	bool IsPointInFrontOfTriangle(Vec3 const& referencePos) const;
public:

	Vec3 m_PointA;
	Vec3 m_PointB;
	Vec3 m_PointC;
};