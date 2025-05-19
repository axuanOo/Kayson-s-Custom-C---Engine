#include "Engine/Math/OBB2.hpp"


OBB2::OBB2()
{

}


OBB2::~OBB2()
{

}

void OBB2::GetCornerPoints(Vec2* out_fourCornerWorldPositions) const
{
	Vec2 jBasisNormal = m_iBasisNormal.GetRotated90Degrees();

	out_fourCornerWorldPositions[0] = m_center - m_iBasisNormal * m_halfDimensions.x - jBasisNormal * m_halfDimensions.y;
	out_fourCornerWorldPositions[1] = m_center - m_iBasisNormal * m_halfDimensions.x + jBasisNormal * m_halfDimensions.y;
	out_fourCornerWorldPositions[2] = m_center + m_iBasisNormal * m_halfDimensions.x + jBasisNormal * m_halfDimensions.y;
	out_fourCornerWorldPositions[3] = m_center + m_iBasisNormal * m_halfDimensions.x - jBasisNormal * m_halfDimensions.y;
}

Vec2 OBB2::GetLocalPosForWorldPos(Vec2 worldPos) const
{
	Vec2 displacement = worldPos - m_center;
	Vec2 jBasisNormal = m_iBasisNormal.GetRotated90Degrees();

	float localX = (displacement.x * m_iBasisNormal.x) + (displacement.y * m_iBasisNormal.y);
	float localY = (displacement.x * jBasisNormal.x) + (displacement.y * jBasisNormal.y);

	return Vec2(localX, localY);
}

Vec2 OBB2::GetWorldPosForLocalPos(Vec2 localPos) const
{
	Vec2 jBasisNormal = m_iBasisNormal.GetRotated90Degrees();

	float worldX = m_center.x + (localPos.x * m_iBasisNormal.x) + (localPos.y * jBasisNormal.x);
	float worldY = m_center.y + (localPos.x * m_iBasisNormal.y) + (localPos.y * jBasisNormal.y);

	return Vec2(worldX, worldY);
}

void OBB2::RotateAboutCenter(float rotationDeltaDegrees)
{
	m_iBasisNormal.RotateDegrees(rotationDeltaDegrees);
}
