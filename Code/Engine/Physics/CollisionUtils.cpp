#include "Engine/Physics/CollisionUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"

CollisionResult2D OBBCollisionWithOBB2D(OBB2 const& obb1, OBB2 const& obb2)
{
	CollisionResult2D result = CollisionResult2D();
	float halfDimensionSumSquaredX = (obb1.m_halfDimensions.x + obb2.m_halfDimensions.x);
	float halfDimensionSumSquaredY = (obb1.m_halfDimensions.y + obb2.m_halfDimensions.y);

	float distanceSquared = (obb1.m_center - obb2.m_center).GetLengthSquared();

	if ((halfDimensionSumSquaredX * halfDimensionSumSquaredX + halfDimensionSumSquaredY * halfDimensionSumSquaredY) < distanceSquared)
	{
		return result;
	}

	std::vector<Vec2> obbAxis;

	obbAxis.push_back(obb1.m_iBasisNormal);
	obbAxis.push_back(obb1.m_iBasisNormal.GetRotated90Degrees());
	obbAxis.push_back(obb2.m_iBasisNormal);
	obbAxis.push_back(obb2.m_iBasisNormal.GetRotated90Degrees());

	float minmumDepth = FLT_MAX;
	Vec2 mtvAxis = Vec2::ZERO;

	for (int axisIndex = 0; axisIndex < (int)obbAxis.size(); axisIndex++)
	{
		FloatRange obb1ProjectionRange;
		FloatRange obb2ProjectionRange;

		obb1ProjectionRange.m_min = FLT_MAX;
		obb1ProjectionRange.m_max = -FLT_MAX;
		obb2ProjectionRange.m_min = FLT_MAX;
		obb2ProjectionRange.m_max = -FLT_MAX;

		Vec2 out_Obb1Points[4];
		Vec2 out_Obb2Points[4];
		obb1.GetCornerPoints(out_Obb1Points);
		obb2.GetCornerPoints(out_Obb2Points);

		for (int obb1VertIndex = 0; obb1VertIndex < 4; obb1VertIndex++)
		{
			float projectValue = DotProduct2D(out_Obb1Points[obb1VertIndex], obbAxis[axisIndex]);

			if (projectValue < obb1ProjectionRange.m_min)
			{
				obb1ProjectionRange.m_min = projectValue;
			}

			if (projectValue > obb1ProjectionRange.m_max)
			{
				obb1ProjectionRange.m_max = projectValue;
			}
		}

		for (int obb2VertIndex = 0; obb2VertIndex < 4; obb2VertIndex++)
		{
			float projectValue = DotProduct2D(out_Obb2Points[obb2VertIndex], obbAxis[axisIndex]);

			if (projectValue < obb2ProjectionRange.m_min)
			{
				obb2ProjectionRange.m_min = projectValue;
			}

			if (projectValue > obb2ProjectionRange.m_max)
			{
				obb2ProjectionRange.m_max = projectValue;
			}
		}

		if (obb1ProjectionRange.IsOverlappingWith(obb2ProjectionRange) == false)
		{
			result.m_didImpact = false;
			result.m_minimumTranslationVector = Vec2::ZERO;
			return result;
		}

		if (obb1ProjectionRange.OverlapLength(obb2ProjectionRange) < minmumDepth)
		{
			minmumDepth = obb1ProjectionRange.OverlapLength(obb2ProjectionRange);
			mtvAxis = obbAxis[axisIndex];
		}

	}

	Vec2 centerDifference = obb2.m_center - obb1.m_center;
	if (DotProduct2D(centerDifference, mtvAxis) < 0) {
		mtvAxis = -mtvAxis;
	}

	result.m_didImpact = true;
	result.m_minimumTranslationVector = mtvAxis * minmumDepth;
	return result;
}
