#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>

struct Mat44;
class RandomNumberGenerator;

struct Plane2 
{
	Plane2() = default;
	Plane2(Vec2 const& normal, float radius) : m_normal(normal), m_distanceFromCenter(radius) {};
	Vec2		m_normal;
	float		m_distanceFromCenter;
};

struct ConvexPoly2 
{
	std::vector<Vec2>	m_pointsInPositiveThetaOrder;

	void				RandomizeShape(RandomNumberGenerator* rng, Vec2 const& discCenter, float discRadius);
	
	void				Translate(Vec2 const& translation);
	void				Rotate(Vec2 const& pivot, float deltaDegrees);
	void				Scale(Vec2 const& pivot, float deltaScale);

	void				Reset();
};

struct ConvexHull2 
{
	ConvexHull2();
	ConvexHull2(const ConvexPoly2& cp);

	void				ConstructConvexHullFromConvexPoly2(const ConvexPoly2& cp);

	std::vector<Plane2> m_enclosedPlanes;
};