#include "Engine/Math/ConvexPoly2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Matrix44.hpp"

void ConvexPoly2::RandomizeShape(RandomNumberGenerator* rng, Vec2 const& discCenter, float discRadius)
{
	float beginTheta = rng->RollRandomFloatInRange(20.0f, 60.0f);
	Vec2 firstPoint = discCenter + discRadius * Vec2(1.0f,0.0f).GetRotatedDegrees(beginTheta);
	m_pointsInPositiveThetaOrder.push_back(firstPoint);

	float currentTheta = beginTheta;
	currentTheta += rng->RollRandomFloatInRange(30.0f, 160.0f);

	while (currentTheta < 360.0f + beginTheta - 20.0f)
	{
		Vec2 point = discCenter + discRadius * Vec2(1.0f,0.0f).GetRotatedDegrees(currentTheta);
		m_pointsInPositiveThetaOrder.push_back(point);
		currentTheta += rng->RollRandomFloatInRange(30.0f, 160.0f);
	}
}


void ConvexPoly2::Translate(Vec2 const& translation)
{
	for (int i = 0; i < (int)m_pointsInPositiveThetaOrder.size(); i++)
	{
		m_pointsInPositiveThetaOrder[i] += translation;
	}
}

void ConvexPoly2::Rotate(Vec2 const& pivot, float deltaDegrees)
{
	for (int i = 0; i < (int)m_pointsInPositiveThetaOrder.size(); i++)
	{
		Vec2& point = m_pointsInPositiveThetaOrder[i];
		Vec2 pivotToPoint = point - pivot;

		pivotToPoint.RotateDegrees(deltaDegrees);
		point = pivot + pivotToPoint;
	}
}

void ConvexPoly2::Scale(Vec2 const& pivot, float deltaScale)
{
	float scale = 1.0f + deltaScale;

	for (int i = 0; i < (int)m_pointsInPositiveThetaOrder.size(); i++)
	{
		Vec2& point = m_pointsInPositiveThetaOrder[i];
		Vec2 pivotToPoint = point - pivot;

		pivotToPoint.SetLength(scale * pivotToPoint.GetLength());
		point = pivot + pivotToPoint;
	}
}

void ConvexPoly2::Reset()
{
	m_pointsInPositiveThetaOrder.clear();
}

ConvexHull2::ConvexHull2(const ConvexPoly2& cp)
{
	ConstructConvexHullFromConvexPoly2(cp);
}

ConvexHull2::ConvexHull2()
{

}

void ConvexHull2::ConstructConvexHullFromConvexPoly2(const ConvexPoly2& cp)
{
	m_enclosedPlanes.clear();
	m_enclosedPlanes.reserve(cp.m_pointsInPositiveThetaOrder.size() - 1);

	for (int i = 0; i < (int)cp.m_pointsInPositiveThetaOrder.size(); i++)
	{
		Plane2 edgePlane = Plane2();

		Vec2 const& point1 = cp.m_pointsInPositiveThetaOrder[i];
		size_t nextIndex = (i + 1) % cp.m_pointsInPositiveThetaOrder.size();
		Vec2 const& point2 = cp.m_pointsInPositiveThetaOrder[nextIndex];

		Vec2 SToE = (point2 - point1).GetNormalized();

		edgePlane.m_normal = SToE.GetRotatedMinus90Degrees();
		edgePlane.m_distanceFromCenter = DotProduct2D(point1, edgePlane.m_normal);

		m_enclosedPlanes.push_back(edgePlane);
	}

}
