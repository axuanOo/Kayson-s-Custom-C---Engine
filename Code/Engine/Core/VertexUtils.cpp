#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Hexagon.hpp"
#include "Engine/Math/Cylinder3.hpp"
#include "Engine/Math/Tetrahedron.hpp"
#include "Engine/Math/ConvexPoly2.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"


void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Capsule2 const& capsule, Rgba8 const& color)
{
	int NUM_TRIANGLES = 32;

	int NUM_VERTS_DISC = NUM_TRIANGLES * 6;
	int NUM_VERTS_AABB = 6;
	int NUM_VERTS = NUM_VERTS_DISC + NUM_VERTS_AABB;

	float SINGLE_ANGLE_STEP = 180.0f / NUM_TRIANGLES;

	verts.reserve(NUM_VERTS);

	float startAngle = 0.0f;

	Vec3 UpDirectionLocal = Vec3(0.0f, 1.0f, 0.0f) * capsule.radius;
	Vec3 DownDirectionLocal = Vec3(0.0f, -1.0f, 0.0f) * capsule.radius;

	float startToEndDistance = (capsule.m_end - capsule.m_start).GetLength();
	float orientDegree = (capsule.m_end - capsule.m_start).GetOrientationDegrees();

	Vec3 capsuleCenterLocal = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 capsuleStartLocal = Vec3(-0.5f * startToEndDistance, 0.0f, 0.0f);
	Vec3 capsuleEndLocal = Vec3(0.5f * startToEndDistance, 0.0f, 0.0f);

	Vertex_PCU boneStartCenterPCU = Vertex_PCU(ConstructVec3FromVec2(capsule.GetCenter()), color, Vec2(0.0f, 0.0f));
	Vertex_PCU boneEndCenterPCU = Vertex_PCU(capsuleEndLocal.GetRotatedAboutZDegrees(orientDegree)+ConstructVec3FromVec2(capsule.GetCenter()), color, Vec2(0.0f, 0.0f));

	Vec3 capsuleAABB_LeftBottom = capsuleStartLocal + DownDirectionLocal;
	Vec3 capsuleAABB_LeftTop = capsuleStartLocal + UpDirectionLocal;
	Vec3 capsuleAABB_RightBottom = capsuleEndLocal + DownDirectionLocal;
	Vec3 capsuleAABB_RightTop = capsuleEndLocal + UpDirectionLocal;

	capsuleAABB_LeftBottom = capsuleAABB_LeftBottom.GetRotatedAboutZDegrees(orientDegree) + ConstructVec3FromVec2(capsule.GetCenter());
	capsuleAABB_LeftTop = capsuleAABB_LeftTop.GetRotatedAboutZDegrees(orientDegree)+ ConstructVec3FromVec2(capsule.GetCenter());
	capsuleAABB_RightBottom = capsuleAABB_RightBottom.GetRotatedAboutZDegrees(orientDegree)+ ConstructVec3FromVec2(capsule.GetCenter());
	capsuleAABB_RightTop = capsuleAABB_RightTop.GetRotatedAboutZDegrees(orientDegree)+ ConstructVec3FromVec2(capsule.GetCenter());

	Vertex_PCU PCU_LeftBottom = Vertex_PCU(capsuleAABB_LeftBottom, color, Vec2(0.0f, 0.0f));
	Vertex_PCU PCU_LeftTop = Vertex_PCU(capsuleAABB_LeftTop, color, Vec2(0.0f, 0.0f));
	Vertex_PCU PCU_RightBottom = Vertex_PCU(capsuleAABB_RightBottom, color, Vec2(0.0f, 0.0f));
	Vertex_PCU PCU_RightTop = Vertex_PCU(capsuleAABB_RightTop, color, Vec2(0.0f, 0.0f));

	verts.push_back(PCU_LeftBottom);
	verts.push_back(PCU_RightBottom);
	verts.push_back(PCU_RightTop);

	verts.push_back(PCU_LeftBottom);
	verts.push_back(PCU_RightTop);
	verts.push_back(PCU_LeftTop);


	for (int i = 0; i < NUM_TRIANGLES; i++)
	{
		Vec3 startLow = UpDirectionLocal.GetRotatedAboutZDegrees(startAngle) + capsuleStartLocal;
		startLow = startLow.GetRotatedAboutZDegrees(orientDegree) + ConstructVec3FromVec2(capsule.GetCenter());
		Vec3 endLow = DownDirectionLocal.GetRotatedAboutZDegrees(startAngle) + capsuleEndLocal;
		endLow = endLow.GetRotatedAboutZDegrees(orientDegree) + ConstructVec3FromVec2(capsule.GetCenter());
		Vertex_PCU lowerEdgeOfTriangleStart = Vertex_PCU(startLow, color, Vec2(1.0f, 0.0f));
		Vertex_PCU lowerEdgeOfTriangleEnd = Vertex_PCU(endLow, color, Vec2(1.0f, 0.0f));

		startAngle += SINGLE_ANGLE_STEP;

		Vec3 startHigh = UpDirectionLocal.GetRotatedAboutZDegrees(startAngle) + capsuleStartLocal;
		startHigh = startHigh.GetRotatedAboutZDegrees(orientDegree) + ConstructVec3FromVec2(capsule.GetCenter());
		Vec3 endHigh = DownDirectionLocal.GetRotatedAboutZDegrees(startAngle) + capsuleEndLocal;
		endHigh = endHigh.GetRotatedAboutZDegrees(orientDegree) + ConstructVec3FromVec2(capsule.GetCenter());

		Vertex_PCU higherEdgeOfTriangleStart = Vertex_PCU(startHigh, color, Vec2(1.0f, 0.0f));
		Vertex_PCU higherEdgeOfTriangleEnd = Vertex_PCU(endHigh, color, Vec2(1.0f, 0.0f));

		verts.push_back(boneStartCenterPCU);
		verts.push_back(lowerEdgeOfTriangleStart);
		verts.push_back(higherEdgeOfTriangleStart);

		verts.push_back(boneEndCenterPCU);
		verts.push_back(lowerEdgeOfTriangleEnd);
		verts.push_back(higherEdgeOfTriangleEnd);
	}

}

void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color)
{
	int NUM_TRIANGLES = 32;

	int NUM_VERTS_DISC = NUM_TRIANGLES * 6;
	int NUM_VERTS_AABB = 6;
	int NUM_VERTS = NUM_VERTS_DISC + NUM_VERTS_AABB;

	float SINGLE_ANGLE_STEP = 180.0f / NUM_TRIANGLES;

	verts.reserve(NUM_VERTS);

	float startAngle = 0.0f;

	Vec3 UpDirectionLocal = Vec3(0.0f, 1.0f, 0.0f) * radius;
	Vec3 DownDirectionLocal = Vec3(0.0f, -1.0f, 0.0f) * radius;

	float startToEndDistance = (boneEnd - boneStart).GetLength();

	Vec3 capsuleCenterLocal = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 capsuleStartLocal = Vec3(-0.5f * startToEndDistance, 0.0f, 0.0f);
	Vec3 capsuleEndLocal = Vec3(0.5f * startToEndDistance, 0.0f, 0.0f);

	Vertex_PCU boneStartCenterPCU = Vertex_PCU(capsuleStartLocal, color, Vec2(0.0f, 0.0f));
	Vertex_PCU boneEndCenterPCU = Vertex_PCU(capsuleEndLocal, color, Vec2(0.0f, 0.0f));

	Vec3 capsuleAABB_LeftBottom = capsuleStartLocal + DownDirectionLocal;
	Vec3 capsuleAABB_LeftTop = capsuleStartLocal + UpDirectionLocal;
	Vec3 capsuleAABB_RightBottom = capsuleEndLocal + DownDirectionLocal;
	Vec3 capsuleAABB_RightTop = capsuleEndLocal + UpDirectionLocal;

	Vertex_PCU PCU_LeftBottom = Vertex_PCU(capsuleAABB_LeftBottom, color, Vec2(0.0f, 0.0f));
	Vertex_PCU PCU_LeftTop = Vertex_PCU(capsuleAABB_LeftTop, color, Vec2(0.0f, 0.0f));
	Vertex_PCU PCU_RightBottom = Vertex_PCU(capsuleAABB_RightBottom, color, Vec2(0.0f, 0.0f));
	Vertex_PCU PCU_RightTop = Vertex_PCU(capsuleAABB_RightTop, color, Vec2(0.0f, 0.0f));

	verts.push_back(PCU_LeftBottom);
	verts.push_back(PCU_LeftTop);
	verts.push_back(PCU_RightBottom);

	verts.push_back(PCU_RightTop);
	verts.push_back(PCU_LeftTop);
	verts.push_back(PCU_RightBottom);


	for (int i = 0; i < NUM_TRIANGLES; i++)
	{
		Vertex_PCU lowerEdgeOfTriangleStart = Vertex_PCU(UpDirectionLocal.GetRotatedAboutZDegrees(startAngle) + capsuleStartLocal, color, Vec2(1.0f, 0.0f));
		Vertex_PCU lowerEdgeOfTriangleEnd = Vertex_PCU(DownDirectionLocal.GetRotatedAboutZDegrees(startAngle) + capsuleEndLocal, color, Vec2(1.0f, 0.0f));

		startAngle += SINGLE_ANGLE_STEP;

		Vertex_PCU higherEdgeOfTriangleStart = Vertex_PCU(UpDirectionLocal.GetRotatedAboutZDegrees(startAngle) + capsuleStartLocal, color, Vec2(1.0f, 0.0f));
		Vertex_PCU higherEdgeOfTriangleEnd = Vertex_PCU(DownDirectionLocal.GetRotatedAboutZDegrees(startAngle) + capsuleEndLocal, color, Vec2(1.0f, 0.0f));

		verts.push_back(lowerEdgeOfTriangleStart);
		verts.push_back(boneStartCenterPCU);
		verts.push_back(higherEdgeOfTriangleStart);

		verts.push_back(lowerEdgeOfTriangleEnd);
		verts.push_back(boneEndCenterPCU);
		verts.push_back(higherEdgeOfTriangleEnd);
	}

	Vec2 capsuleCenterWorld = (boneStart + boneEnd) * 0.5f;
	float orientDegree = (boneEnd - boneStart).GetOrientationDegrees();
	TransformVertexArrayXY3D(NUM_VERTS, verts.data(), 1.0f, orientDegree, capsuleCenterWorld);
}

void AddVertsForHalfDisc(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, Rgba8 const& color, float startDegree)
{
	int NUM_TRIANGLES = 32;
	int NUM_VERTS = NUM_TRIANGLES * 3;
	float SINGLE_ANGLE_STEP = 180.0f / NUM_TRIANGLES;

	verts.reserve(NUM_VERTS);

	float currentAngle = startDegree;

	Vec3 rightDirectionLocal = Vec3(1.0f, 0.0f, 0.0f) * radius;
	Vec3 circleCenterLocal = Vec3(0.0f, 0.0f, 0.0f);

	Vertex_PCU circleCenterPCU = Vertex_PCU(circleCenterLocal, color, Vec2(0.0f, 0.0f));

	for (int i = 0; i < NUM_TRIANGLES; i++)
	{
		Vertex_PCU lowerEdgeOfTriangle = Vertex_PCU(rightDirectionLocal.GetRotatedAboutZDegrees(currentAngle), color, Vec2(1.0f, 0.0f));

		currentAngle += SINGLE_ANGLE_STEP;

		Vertex_PCU higherEdgeOfTriangle = Vertex_PCU(rightDirectionLocal.GetRotatedAboutZDegrees(currentAngle), color, Vec2(1.0f, 0.0f));

		verts.push_back(lowerEdgeOfTriangle);
		verts.push_back(circleCenterPCU);
		verts.push_back(higherEdgeOfTriangle);
	}

	TransformVertexArrayXY3D(NUM_VERTS, verts.data(), 1.0f, 0.0f, center);
}

void AddVertsForDisc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, Rgba8 const& color)
{
	int NUM_TRIANGLES = 64;
	//int NUM_VERTS = NUM_TRIANGLES * 3;
	float SINGLE_ANGLE_STEP = 360.0f / NUM_TRIANGLES;

	//verts.reserve(NUM_VERTS);

	float currentAngle = 0.0f;

	Vec3 rightDirectionLocal = Vec3(1.0f, 0.0f, 0.0f) * radius;
	Vec3 circleCenterLocal = Vec3(center.x, center.y, 0.0f);

	Vertex_PCU circleCenterPCU = Vertex_PCU(circleCenterLocal, color, Vec2(0.0f, 0.0f));

	for (int i = 0; i < NUM_TRIANGLES; i++)
	{
		Vertex_PCU lowerEdgeOfTriangle = Vertex_PCU(rightDirectionLocal.GetRotatedAboutZDegrees(currentAngle) + circleCenterLocal, color, Vec2(1.0f, 0.0f));
		currentAngle += SINGLE_ANGLE_STEP;
		Vertex_PCU higherEdgeOfTriangle = Vertex_PCU(rightDirectionLocal.GetRotatedAboutZDegrees(currentAngle) + circleCenterLocal, color, Vec2(1.0f, 0.0f));

		verts.push_back(circleCenterPCU);
		verts.push_back(lowerEdgeOfTriangle);
		verts.push_back(higherEdgeOfTriangle);
	}
}

void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 const& uvAtMins, Vec2 const& uvAtMaxs)
{
	Vec2 leftBottomUV = uvAtMins;
	Vec2 rightBottomUV = Vec2(uvAtMaxs.x, uvAtMins.y);
	Vec2 leftTopUV = Vec2(uvAtMins.x, uvAtMaxs.y);
	Vec2 rightTopUV = uvAtMaxs;

	Vec3 leftBottomPointPosition = Vec3(bounds.GetPointAtUV(Vec2(0.0f, 0.0f)).x, bounds.GetPointAtUV(Vec2(0.0f, 0.0f)).y, 0.0f);
	Vec3 rightBottomPointPosition = Vec3(bounds.GetPointAtUV(Vec2(1.0f, 0.0f)).x, bounds.GetPointAtUV(Vec2(1.0f, 0.0f)).y, 0.0f);
	Vec3 leftTopPointPosition = Vec3(bounds.GetPointAtUV(Vec2(0.0f, 1.0f)).x, bounds.GetPointAtUV(Vec2(0.0f, 1.0f)).y, 0.0f);
	Vec3 rightTopPointPosition = Vec3(bounds.GetPointAtUV(Vec2(1.0f, 1.0f)).x, bounds.GetPointAtUV(Vec2(1.0f, 1.0f)).y, 0.0f);

	Vertex_PCU LBVert = Vertex_PCU(leftBottomPointPosition, color, leftBottomUV);
	Vertex_PCU RBVert = Vertex_PCU(rightBottomPointPosition, color, rightBottomUV);
	Vertex_PCU LTVert = Vertex_PCU(leftTopPointPosition, color, leftTopUV);
	Vertex_PCU RTVert = Vertex_PCU(rightTopPointPosition, color, rightTopUV);

	verts.push_back(LBVert);
	verts.push_back(RBVert);
	verts.push_back(RTVert);

	verts.push_back(LBVert);
	verts.push_back(RTVert);
	verts.push_back(LTVert);

}

void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, AABB2 const& uvBox)
{
	Vec2 leftBottomUV = uvBox.GetPointAtUV(Vec2::ZERO);
	Vec2 rightBottomUV = uvBox.GetPointAtUV(Vec2(1.0f, 0.0f));
	Vec2 leftTopUV = uvBox.GetPointAtUV(Vec2(0.0f, 1.0f));
	Vec2 rightTopUV = uvBox.GetPointAtUV(Vec2(1.0f, 1.0f));

	Vec3 leftBottomPointPosition = Vec3(bounds.GetPointAtUV(Vec2(0.0f, 0.0f)).x, bounds.GetPointAtUV(Vec2(0.0f, 0.0f)).y, 0.0f);
	Vec3 rightBottomPointPosition = Vec3(bounds.GetPointAtUV(Vec2(1.0f, 0.0f)).x, bounds.GetPointAtUV(Vec2(1.0f, 0.0f)).y, 0.0f);
	Vec3 leftTopPointPosition = Vec3(bounds.GetPointAtUV(Vec2(0.0f, 1.0f)).x, bounds.GetPointAtUV(Vec2(0.0f, 1.0f)).y, 0.0f);
	Vec3 rightTopPointPosition = Vec3(bounds.GetPointAtUV(Vec2(1.0f, 1.0f)).x, bounds.GetPointAtUV(Vec2(1.0f, 1.0f)).y, 0.0f);

	Vertex_PCU LBVert = Vertex_PCU(leftBottomPointPosition, color, leftBottomUV);
	Vertex_PCU RBVert = Vertex_PCU(rightBottomPointPosition, color, rightBottomUV);
	Vertex_PCU LTVert = Vertex_PCU(leftTopPointPosition, color, leftTopUV);
	Vertex_PCU RTVert = Vertex_PCU(rightTopPointPosition, color, rightTopUV);

	verts.push_back(LBVert);
	verts.push_back(RBVert);
	verts.push_back(RTVert);

	verts.push_back(LBVert);
	verts.push_back(RTVert);
	verts.push_back(LTVert);
}

void AddVertsForOBB2D(std::vector<Vertex_PCU>& verts, OBB2 const& box, Rgba8 const& color)
{
	verts.reserve(6);

	Vec2 jBasisNormal = box.m_iBasisNormal.GetRotated90Degrees();

	float boxHalfDimensionX = box.m_halfDimensions.x;
	float boxHalfDimensionY = box.m_halfDimensions.y;

	Vec2 OBB_LeftBottom = box.m_center - (boxHalfDimensionX * box.m_iBasisNormal) - (jBasisNormal * boxHalfDimensionY);
	Vec2 OBB_RightBottom =  box.m_center + (boxHalfDimensionX * box.m_iBasisNormal) - (jBasisNormal * boxHalfDimensionY);
	Vec2 OBB_LeftTop = box.m_center - (boxHalfDimensionX * box.m_iBasisNormal) + (jBasisNormal * boxHalfDimensionY);
	Vec2 OBB_RightTop =  box.m_center + (boxHalfDimensionX * box.m_iBasisNormal) + (jBasisNormal * boxHalfDimensionY);

	Vertex_PCU OBB_LB_PCU = Vertex_PCU(ConstructVec3FromVec2(OBB_LeftBottom), color, Vec2(0.0f, 0.0f));
	Vertex_PCU OBB_RB_PCU = Vertex_PCU(ConstructVec3FromVec2(OBB_RightBottom), color, Vec2(1.0f, 0.0f));
	Vertex_PCU OBB_LT_PCU = Vertex_PCU(ConstructVec3FromVec2(OBB_LeftTop), color, Vec2(0.0f, 1.0f));
	Vertex_PCU OBB_RT_PCU = Vertex_PCU(ConstructVec3FromVec2(OBB_RightTop), color, Vec2(1.0f, 1.0f));

	verts.push_back(OBB_LB_PCU);
	verts.push_back(OBB_RB_PCU);
	verts.push_back(OBB_RT_PCU);

	verts.push_back(OBB_LB_PCU);
	verts.push_back(OBB_RT_PCU);
	verts.push_back(OBB_LT_PCU);
}

void AddVertsForOBB2D(std::vector<Vertex_PCU>& verts, OBB2 const& box, Rgba8 const& color, AABB2 const& uvBox)
{
	verts.reserve(6);

	Vec2 jBasisNormal = box.m_iBasisNormal.GetRotated90Degrees();

	float boxHalfDimensionX = box.m_halfDimensions.x;
	float boxHalfDimensionY = box.m_halfDimensions.y;

	Vec2 OBB_LeftBottom = box.m_center - (boxHalfDimensionX * box.m_iBasisNormal) - (jBasisNormal * boxHalfDimensionY);
	Vec2 OBB_RightBottom = box.m_center + (boxHalfDimensionX * box.m_iBasisNormal) - (jBasisNormal * boxHalfDimensionY);
	Vec2 OBB_LeftTop = box.m_center - (boxHalfDimensionX * box.m_iBasisNormal) + (jBasisNormal * boxHalfDimensionY);
	Vec2 OBB_RightTop = box.m_center + (boxHalfDimensionX * box.m_iBasisNormal) + (jBasisNormal * boxHalfDimensionY);

	Vertex_PCU OBB_LB_PCU = Vertex_PCU(ConstructVec3FromVec2(OBB_LeftBottom), color, uvBox.GetPointAtUV(Vec2(0.0f, 0.0f)));
	Vertex_PCU OBB_RB_PCU = Vertex_PCU(ConstructVec3FromVec2(OBB_RightBottom), color, uvBox.GetPointAtUV(Vec2(1.0f, 0.0f)));
	Vertex_PCU OBB_LT_PCU = Vertex_PCU(ConstructVec3FromVec2(OBB_LeftTop), color, uvBox.GetPointAtUV(Vec2(0.0f, 1.0f)));
	Vertex_PCU OBB_RT_PCU = Vertex_PCU(ConstructVec3FromVec2(OBB_RightTop), color, uvBox.GetPointAtUV(Vec2(1.0f, 1.0f)));

	verts.push_back(OBB_LB_PCU);
	verts.push_back(OBB_RB_PCU);
	verts.push_back(OBB_RT_PCU);

	verts.push_back(OBB_LB_PCU);
	verts.push_back(OBB_RT_PCU);
	verts.push_back(OBB_LT_PCU);

}

void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color)
{
	verts.reserve(6);

	Vec2 startToEndNormalized = (end - start).GetNormalized();

	Vec2 GetUpDirectionalVector = startToEndNormalized.GetRotated90Degrees();
	Vec2 GetDownDirectionalVector = startToEndNormalized.GetRotatedMinus90Degrees();

	Vec2 leftBottomUV = Vec2(0.0f, 0.0f);
	Vec2 rightBottomUV = Vec2(1.0f, 0.0f);
	Vec2 leftTopUV = Vec2(0.0f, 1.0f);
	Vec2 rightTopUV = Vec2(1.0f, 1.0f);

	Vec2 leftBottomPointPosition = start + GetDownDirectionalVector * 0.5f * thickness;
	Vec2 rightBottomPointPosition = end + GetDownDirectionalVector * 0.5f * thickness;
	Vec2 leftTopPointPosition = start + GetUpDirectionalVector * 0.5f * thickness;
	Vec2 rightTopPointPosition = end + GetUpDirectionalVector * 0.5f * thickness;

	Vertex_PCU RBVert = Vertex_PCU(Vec3(rightBottomPointPosition.x, rightBottomPointPosition.y, 0.0f), color, rightBottomUV);
	Vertex_PCU LBVert = Vertex_PCU(Vec3(leftBottomPointPosition.x, leftBottomPointPosition.y, 0.0f), color, leftBottomUV);
	Vertex_PCU LTVert = Vertex_PCU(Vec3(leftTopPointPosition.x, leftTopPointPosition.y, 0.0f), color, leftTopUV);
	Vertex_PCU RTVert = Vertex_PCU(Vec3(rightTopPointPosition.x, rightTopPointPosition.y, 0.0f), color, rightTopUV);

	verts.push_back(LBVert);
	verts.push_back(RBVert);
	verts.push_back(RTVert);

	verts.push_back(LBVert);
	verts.push_back(RTVert);
	verts.push_back(LTVert);

}

void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, LineSegment2 const& lineSegment, float thickness, Rgba8 const& color)
{
	verts.reserve(6);

	Vec2 startToEndNormalized = (lineSegment.m_end - lineSegment.m_start).GetNormalized();

	Vec2 GetUpDirectionalVector = startToEndNormalized.GetRotated90Degrees();
	Vec2 GetDownDirectionalVector = startToEndNormalized.GetRotatedMinus90Degrees();

	Vec2 leftBottomUV = Vec2(0.0f, 0.0f);
	Vec2 rightBottomUV = Vec2(1.0f, 0.0f);
	Vec2 leftTopUV = Vec2(0.0f, 1.0f);
	Vec2 rightTopUV = Vec2(1.0f, 1.0f);

	Vec2 leftBottomPointPosition = lineSegment.m_start + GetDownDirectionalVector * 0.5f * thickness;
	Vec2 rightBottomPointPosition = lineSegment.m_end + GetDownDirectionalVector * 0.5f * thickness;
	Vec2 leftTopPointPosition = lineSegment.m_start + GetUpDirectionalVector * 0.5f * thickness;
	Vec2 rightTopPointPosition = lineSegment.m_end + GetUpDirectionalVector * 0.5f * thickness;

	Vertex_PCU RBVert = Vertex_PCU(Vec3(rightBottomPointPosition.x, rightBottomPointPosition.y, 0.0f), color, rightBottomUV);
	Vertex_PCU LBVert = Vertex_PCU(Vec3(leftBottomPointPosition.x, leftBottomPointPosition.y, 0.0f), color, leftBottomUV);
	Vertex_PCU LTVert = Vertex_PCU(Vec3(leftTopPointPosition.x, leftTopPointPosition.y, 0.0f), color, leftTopUV);
	Vertex_PCU RTVert = Vertex_PCU(Vec3(rightTopPointPosition.x, rightTopPointPosition.y, 0.0f), color, rightTopUV);

	verts.push_back(LBVert);
	verts.push_back(RBVert);
	verts.push_back(RTVert);

	verts.push_back(LBVert);
	verts.push_back(RTVert);
	verts.push_back(LTVert);
}


void AddVertsForArrow2D(std::vector<Vertex_PCU>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 const& color)
{
	verts.reserve(18);

	Vec2 fwdNormal = (tailPos - tipPos).GetNormalized();

	Vec2 fwdPlus45 = fwdNormal.GetRotatedDegrees(45.0f) * arrowSize;
	Vec2 fwdMinus45 = fwdNormal.GetRotatedDegrees(-45.0f) * arrowSize;

	AddVertsForLineSegment2D(verts, tipPos, tailPos, lineThickness, color);
	AddVertsForLineSegment2D(verts, tipPos, tipPos + fwdPlus45, lineThickness, color);
	AddVertsForLineSegment2D(verts, tipPos, tipPos + fwdMinus45, lineThickness, color);
}


void AddVertsForArrow2DStyle2(std::vector<Vertex_PCU>& verts, Vec2 tailPos, Vec2 tipPos, float lineThickness, Rgba8 const& color)
{
	verts.reserve(9);
	AddVertsForLineSegment2D(verts, tipPos, tailPos, lineThickness, color);

	Vec2 fwdNormal = (tipPos - tailPos).GetNormalized();

	Vec2 fwdPlus90 = fwdNormal.GetRotated90Degrees();
	Vec2 fwdMinus90 = fwdNormal.GetRotatedMinus90Degrees();

	Vec2 trianglePoint1 = tipPos + fwdNormal * 2.0f * lineThickness;
	Vec2 trianglePoint2 = tipPos + fwdPlus90 * 2.0f * lineThickness;
	Vec2 trianglePoint3 = tipPos + fwdMinus90 * 2.0f * lineThickness;

	verts.emplace_back(trianglePoint1, color, Vec2::ZERO);
	verts.emplace_back(trianglePoint2, color, Vec2::ZERO);
	verts.emplace_back(trianglePoint3, color, Vec2::ZERO);
}

void AddVertsForRing2D(std::vector<Vertex_PCU>& verts, Vec2 const& Center, float radius, float thickness, Rgba8 const& color)
{
	float halfThickness = 0.5f * thickness;
	float innerRadius = radius - halfThickness;
	float outerRadius = radius + halfThickness;
	Vec3 center = Vec3(Center.x, Center.y, 0.f);

	constexpr int NUM_SIDES = 32;
	constexpr float DEGREES_PER_SIDE = 360.f / static_cast<float>(NUM_SIDES);

	for (int sideNum = 0; sideNum < NUM_SIDES; sideNum++)
	{
		float startDegrees = DEGREES_PER_SIDE * static_cast<float>(sideNum);
		float endDegrees = DEGREES_PER_SIDE * static_cast<float>(sideNum + 1);

		float cosStart = CosDegrees(startDegrees);
		float sinStart = SinDegrees(startDegrees);
		float cosEnd = CosDegrees(endDegrees);
		float sinEnd = SinDegrees(endDegrees);

		Vec3 innerStartPos = Vec3(innerRadius * cosStart, innerRadius * sinStart, 0.f);
		Vec3 outerStartPos = Vec3(outerRadius * cosStart, outerRadius * sinStart, 0.f);
		Vec3 innerEndPos = Vec3(innerRadius * cosEnd, innerRadius * sinEnd, 0.f);
		Vec3 outerEndPos = Vec3(outerRadius * cosEnd, outerRadius * sinEnd, 0.f);

		Vertex_PCU vertA(center + innerEndPos, color, Vec2::ZERO);
		Vertex_PCU vertB(center + innerStartPos, color, Vec2::ZERO);
		Vertex_PCU vertC(center + outerStartPos, color, Vec2::ZERO);
		Vertex_PCU vertD(center + innerEndPos, color, Vec2::ZERO);
		Vertex_PCU vertE(center + outerStartPos, color, Vec2::ZERO);
		Vertex_PCU vertF(center + outerEndPos, color, Vec2::ZERO);

		verts.push_back(vertA);
		verts.push_back(vertB);
		verts.push_back(vertC);

		verts.push_back(vertD);
		verts.push_back(vertE);
		verts.push_back(vertF);
	}
}

void AddVertsForConvexPoly2DOutLine(std::vector<Vertex_PCU>& verts, ConvexPoly2 const& poly2, float thickness, Rgba8 const& outlineColor)
{
	if (poly2.m_pointsInPositiveThetaOrder.empty())
	{
		return;
	}

	for (int j = 0; j < (int)poly2.m_pointsInPositiveThetaOrder.size(); j++)
	{
		Vec2 const& point1 = poly2.m_pointsInPositiveThetaOrder[j];
		size_t nextIndex = (j + 1) % poly2.m_pointsInPositiveThetaOrder.size();
		Vec2 const& point2 = poly2.m_pointsInPositiveThetaOrder[nextIndex];

		Vec2 hori = (point2 - point1).GetNormalized();
		Vec2 vert = hori.GetRotatedMinus90Degrees();

		OBB2 outlineBox = OBB2();

		outlineBox.m_center = 0.5f * (point1 + point2) + vert * thickness * 0.5f;
		outlineBox.m_iBasisNormal = -hori;
		outlineBox.m_halfDimensions = Vec2((point2 - point1).GetLength() * 0.5f + thickness, 0.5f * thickness);

		AddVertsForOBB2D(verts, outlineBox, outlineColor);
	}

}

void AddVertsForConvexPoly2D(std::vector<Vertex_PCU>& verts, ConvexPoly2 const& poly2, Rgba8 const& color)
{
	if (poly2.m_pointsInPositiveThetaOrder.empty())
	{
		return;
	}

	Vertex_PCU startPoint = Vertex_PCU(poly2.m_pointsInPositiveThetaOrder[0],color, Vec2::ZERO);

	for (int i = 1; i < (int)poly2.m_pointsInPositiveThetaOrder.size() - 1; i++)
	{
		verts.push_back(startPoint);
		verts.emplace_back(Vertex_PCU(poly2.m_pointsInPositiveThetaOrder[i], color, Vec2::ZERO));
		verts.emplace_back(Vertex_PCU(poly2.m_pointsInPositiveThetaOrder[i+1], color, Vec2::ZERO));
	}

	//for (int j = 0; j < (int)poly2.m_pointsInPositiveThetaOrder.size(); j++)
	//{
	//	Vec2 const& point1 = poly2.m_pointsInPositiveThetaOrder[j];
	//	size_t nextIndex = (j + 1) % poly2.m_pointsInPositiveThetaOrder.size();
	//	Vec2 const& point2 = poly2.m_pointsInPositiveThetaOrder[nextIndex];

	//	Vec2 hori = (point2 - point1).GetNormalized();
	//	Vec2 vert = hori.GetRotatedMinus90Degrees();

	//	OBB2 outlineBox = OBB2();

	//	outlineBox.m_center = 0.5f * (point1 + point2) + vert * thickness * 0.5f;
	//	outlineBox.m_iBasisNormal = -hori;
	//	outlineBox.m_halfDimensions = Vec2((point2 - point1).GetLength() * 0.5f + thickness, 0.5f * thickness);

	//	AddVertsForOBB2D(verts, outlineBox, outlineColor);
	//}
}

void AddVertsForConvexPoly2DInnerLine(std::vector<Vertex_PCU>& verts, ConvexPoly2 const& poly2, Rgba8 const& color, float thickness, Rgba8 const& inlineColor)
{
	if (poly2.m_pointsInPositiveThetaOrder.empty())
	{
		return;
	}

	Vertex_PCU startPoint = Vertex_PCU(poly2.m_pointsInPositiveThetaOrder[0], color, Vec2::ZERO);

	for (int i = 1; i < (int)poly2.m_pointsInPositiveThetaOrder.size() - 1; i++)
	{
		verts.push_back(startPoint);
		verts.emplace_back(Vertex_PCU(poly2.m_pointsInPositiveThetaOrder[i], color, Vec2::ZERO));
		verts.emplace_back(Vertex_PCU(poly2.m_pointsInPositiveThetaOrder[i + 1], color, Vec2::ZERO));
	}

	for (int j = 0; j < (int)poly2.m_pointsInPositiveThetaOrder.size(); j++)
	{
		Vec2 const& point1 = poly2.m_pointsInPositiveThetaOrder[j];
		size_t nextIndex = (j + 1) % poly2.m_pointsInPositiveThetaOrder.size();
		Vec2 const& point2 = poly2.m_pointsInPositiveThetaOrder[nextIndex];

		Vec2 hori = (point2 - point1).GetNormalized();
		Vec2 vert = hori.GetRotatedMinus90Degrees();

		OBB2 outlineBox = OBB2();

		outlineBox.m_center = 0.5f * (point1 + point2) + vert * thickness * 0.5f;
		outlineBox.m_iBasisNormal = -hori;
		outlineBox.m_halfDimensions = Vec2((point2 - point1).GetLength() * 0.5f + thickness, 0.5f * thickness);

		AddVertsForOBB2D(verts, outlineBox, inlineColor);
	}
}

void AddVertsForTriangle3D(std::vector<Vertex_PCU>& verts, const Triangle3& triangle3d, bool doubleFace /*= true*/, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	verts.reserve(6);

	Vertex_PCU vertA = Vertex_PCU(triangle3d.m_PointA, color, UVs.GetPointAtUV(Vec2(0.0f, 0.0f)));
	Vertex_PCU vertB = Vertex_PCU(triangle3d.m_PointB, color, UVs.GetPointAtUV(Vec2(1.0f, 0.0f)));
	Vertex_PCU vertC = Vertex_PCU(triangle3d.m_PointC, color, UVs.GetPointAtUV(Vec2(0.0f, 1.0f)));

	verts.push_back(vertA);
	verts.push_back(vertB);
	verts.push_back(vertC);

	if (doubleFace)
	{
		verts.push_back(vertA);
		verts.push_back(vertC);
		verts.push_back(vertB);
	}
}

void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	verts.reserve(6);

	Vertex_PCU bottomLeftVertex = Vertex_PCU(bottomLeft, color, UVs.GetPointAtUV(Vec2(0.0f, 0.0f)));
	Vertex_PCU bottomRightVertex = Vertex_PCU(bottomRight, color, UVs.GetPointAtUV(Vec2(1.0f, 0.0f)));
	Vertex_PCU topLeftVertex = Vertex_PCU(topLeft, color, UVs.GetPointAtUV(Vec2(0.0f, 1.0f)));
	Vertex_PCU topRightVertex = Vertex_PCU(topRight, color, UVs.GetPointAtUV(Vec2(1.0f, 1.0f)));

	verts.push_back(bottomLeftVertex);
	verts.push_back(bottomRightVertex);
	verts.push_back(topRightVertex);

	verts.push_back(bottomLeftVertex);
	verts.push_back(topRightVertex);
	verts.push_back(topLeftVertex);

}

void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/, int subdivisionX, int subdivisionY)
{
	Vec3 bottomLeftNormal = CrossProduct3D((bottomRight - bottomLeft), (topRight - bottomLeft)).GetNormalized();
	Vec3 bottomRightNormal = CrossProduct3D((topRight - bottomRight), (bottomLeft - bottomRight)).GetNormalized();
	Vec3 topRightNormal = CrossProduct3D((bottomLeft - topRight), (bottomRight - topRight)).GetNormalized();
	Vec3 topLeftNormal = CrossProduct3D((bottomLeft - topLeft), (topRight - topLeft)).GetNormalized();

	if (subdivisionX == 1 && subdivisionY == 1)
	{
		int vertexesSize = (int)verts.size();

		Vertex_PCUTBN bottomLeftVertex = Vertex_PCUTBN(bottomLeft, color, UVs.GetPointAtUV(Vec2(0.0f, 0.0f)), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), bottomLeftNormal);
		Vertex_PCUTBN bottomRightVertex = Vertex_PCUTBN(bottomRight, color, UVs.GetPointAtUV(Vec2(1.0f, 0.0f)), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), bottomRightNormal);
		Vertex_PCUTBN topRightVertex = Vertex_PCUTBN(topRight, color, UVs.GetPointAtUV(Vec2(1.0f, 1.0f)), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), topRightNormal);
		Vertex_PCUTBN topLeftVertex = Vertex_PCUTBN(topLeft, color, UVs.GetPointAtUV(Vec2(0.0f, 1.0f)), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), topLeftNormal);

		verts.push_back(bottomLeftVertex);
		verts.push_back(bottomRightVertex);
		verts.push_back(topRightVertex);
		verts.push_back(topLeftVertex);


		indexes.push_back(0 + vertexesSize);
		indexes.push_back(1 + vertexesSize);
		indexes.push_back(2 + vertexesSize);

		indexes.push_back(0 + vertexesSize);
		indexes.push_back(2 + vertexesSize);
		indexes.push_back(3 + vertexesSize);
	}
	else 
	{
		float subdivisionStepX = 1.0f / subdivisionX;
		float subdivisionStepY = 1.0f / subdivisionY;

		Vec3 localRight = bottomRight - bottomLeft;
		Vec3 localUp = topLeft - bottomLeft;

		for (int i = 0; i < subdivisionX; i++)
		{
			for (int j = 0; j < subdivisionY; j++)
			{
				int vertexesSize = (int)verts.size();

				Vec3 localLeftBottom = bottomLeft + (float)i * subdivisionStepX * localRight + (float)j * subdivisionStepY *localUp;
				Vec3 localRightBottom = bottomLeft + ((float)(i + 1) *subdivisionStepX * localRight) + (float)j *subdivisionStepY * localUp;
				Vec3 localTopRight = bottomLeft + ((float)(i + 1) *subdivisionStepX * localRight) + ((float)(j + 1) *subdivisionStepY * localUp);
				Vec3 localTopLeft = bottomLeft + (float)i *subdivisionStepX * localRight + ((float)(j + 1) *subdivisionStepY * localUp);

				Vertex_PCUTBN bottomLeftVertex = Vertex_PCUTBN(localLeftBottom, color, UVs.GetPointAtUV(Vec2(0.0f, 0.0f)), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), bottomLeftNormal);
				Vertex_PCUTBN bottomRightVertex = Vertex_PCUTBN(localRightBottom, color, UVs.GetPointAtUV(Vec2(1.0f, 0.0f)), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), bottomRightNormal);
				Vertex_PCUTBN topRightVertex = Vertex_PCUTBN(localTopRight, color, UVs.GetPointAtUV(Vec2(1.0f, 1.0f)), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), topRightNormal);
				Vertex_PCUTBN topLeftVertex = Vertex_PCUTBN(localTopLeft, color, UVs.GetPointAtUV(Vec2(0.0f, 1.0f)), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), topLeftNormal);

				verts.push_back(bottomLeftVertex);
				verts.push_back(bottomRightVertex);
				verts.push_back(topRightVertex);
				verts.push_back(topLeftVertex);


				indexes.push_back(0 + vertexesSize);
				indexes.push_back(1 + vertexesSize);
				indexes.push_back(2 + vertexesSize);

				indexes.push_back(0 + vertexesSize);
				indexes.push_back(2 + vertexesSize);
				indexes.push_back(3 + vertexesSize);

			}
		}

		
	}

}

void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	int vertexesSize = (int)verts.size();

	Vertex_PCU bottomLeftVertex = Vertex_PCU(bottomLeft, color, UVs.GetPointAtUV(Vec2(0.0f, 0.0f)));
	Vertex_PCU bottomRightVertex = Vertex_PCU(bottomRight, color, UVs.GetPointAtUV(Vec2(1.0f, 0.0f)));
	Vertex_PCU topRightVertex = Vertex_PCU(topRight, color, UVs.GetPointAtUV(Vec2(1.0f, 1.0f)));
	Vertex_PCU topLeftVertex = Vertex_PCU(topLeft, color, UVs.GetPointAtUV(Vec2(0.0f, 1.0f)));

	verts.push_back(bottomLeftVertex);
	verts.push_back(bottomRightVertex);
	verts.push_back(topRightVertex);
	verts.push_back(topLeftVertex);

	indexes.push_back(0 + vertexesSize);
	indexes.push_back(1 + vertexesSize);
	indexes.push_back(2 + vertexesSize);

	indexes.push_back(0 + vertexesSize);
	indexes.push_back(2 + vertexesSize);
	indexes.push_back(3 + vertexesSize);
}

void AddVertsForAABB3D(std::vector<Vertex_PCU>& verts, const AABB3& bounds, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	verts.reserve(36);

	Vec3 min = bounds.m_mins;
	Vec3 max = bounds.m_maxs;

	Vec3 box[] =
	{
		min,
		Vec3(max.x,min.y,min.z),
		Vec3(max.x,min.y,max.z),
		Vec3(min.x,min.y,max.z),
		Vec3(min.x,max.y,min.z),
		Vec3(max.x,max.y,min.z),
		max,
		Vec3(min.x,max.y,max.z)
	};

	AddVertsForQuad3D(verts, box[0], box[1], box[2], box[3], color, UVs);
	AddVertsForQuad3D(verts, box[1], box[5], box[6], box[2],color,UVs);
	AddVertsForQuad3D(verts, box[5], box[4], box[7], box[6],color,UVs);
	AddVertsForQuad3D(verts, box[4], box[0], box[3], box[7],color,UVs);
	AddVertsForQuad3D(verts, box[7], box[3], box[2], box[6],color,UVs);
	AddVertsForQuad3D(verts, box[5], box[1], box[0], box[4], color, UVs);
}


void AddVertsForAABB3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, const AABB3& bounds, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	verts.reserve(36);

	Vec3 min = bounds.m_mins;
	Vec3 max = bounds.m_maxs;

	Vec3 box[] =
	{
		min,
		Vec3(max.x,min.y,min.z),
		Vec3(max.x,min.y,max.z),
		Vec3(min.x,min.y,max.z),
		Vec3(min.x,max.y,min.z),
		Vec3(max.x,max.y,min.z),
		max,
		Vec3(min.x,max.y,max.z)
	};

	AddVertsForQuad3D(verts, indexes, box[0], box[1], box[2], box[3], color, UVs);
	AddVertsForQuad3D(verts, indexes, box[1], box[5], box[6], box[2], color, UVs);
	AddVertsForQuad3D(verts, indexes, box[5], box[4], box[7], box[6], color, UVs);
	AddVertsForQuad3D(verts, indexes, box[4], box[0], box[3], box[7], color, UVs);
	AddVertsForQuad3D(verts, indexes, box[7], box[3], box[2], box[6], color, UVs);
	AddVertsForQuad3D(verts, indexes, box[5], box[1], box[0], box[4], color, UVs);
}


void AddVertsForAABB3D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, const AABB3& bounds, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	verts.reserve(36);

	Vec3 min = bounds.m_mins;
	Vec3 max = bounds.m_maxs;

	Vec3 box[] =
	{
		min,
		Vec3(max.x,min.y,min.z),
		Vec3(max.x,min.y,max.z),
		Vec3(min.x,min.y,max.z),
		Vec3(min.x,max.y,min.z),
		Vec3(max.x,max.y,min.z),
		max,
		Vec3(min.x,max.y,max.z)
	};

	AddVertsForQuad3D(verts, indexes, box[0], box[1], box[2], box[3], color, UVs);
	AddVertsForQuad3D(verts, indexes, box[1], box[5], box[6], box[2], color, UVs);
	AddVertsForQuad3D(verts, indexes, box[5], box[4], box[7], box[6], color, UVs);
	AddVertsForQuad3D(verts, indexes, box[4], box[0], box[3], box[7], color, UVs);
	AddVertsForQuad3D(verts, indexes, box[7], box[3], box[2], box[6], color, UVs);
	AddVertsForQuad3D(verts, indexes, box[5], box[1], box[0], box[4], color, UVs);
}

void AddVertsForSkyBox3D(std::vector<Vertex_PCU>& verts, const AABB3& bounds, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	verts.reserve(36);

	Vec3 min = bounds.m_mins;
	Vec3 max = bounds.m_maxs;

	Vec3 box[] =
	{
		min,
		Vec3(max.x,min.y,min.z),
		Vec3(max.x,min.y,max.z),
		Vec3(min.x,min.y,max.z),
		Vec3(min.x,max.y,min.z),
		Vec3(max.x,max.y,min.z),
		max,
		Vec3(min.x,max.y,max.z)
	};

	AddVertsForQuad3D(verts, box[3], box[2], box[1], box[0], color, UVs);
	AddVertsForQuad3D(verts, box[2], box[6], box[5], box[1], color, UVs);
	AddVertsForQuad3D(verts, box[6], box[7], box[4], box[5], color, UVs);
	AddVertsForQuad3D(verts, box[7], box[3], box[0], box[4], color, UVs);
	AddVertsForQuad3D(verts, box[6], box[2], box[3], box[7], color, UVs);
	AddVertsForQuad3D(verts, box[4], box[0], box[1], box[5], color, UVs);

}

void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, const Vec3& center, float radius, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/, int numLatitudeSlices /*= 8*/, int numLongtitudeSlices /*= 8*/)
{
	verts.reserve(100000);

	float currentLongitude = 0.0f;
	float currentLatitude = -90.0f;

	int longitudeNum = numLongtitudeSlices;

	float singleStepDegreesLong = 360.0f / longitudeNum;
	float singleStepDegreesLati = 180.0f / numLatitudeSlices;

	float UVPercentage = 1.0f / numLatitudeSlices;

	Vec3 BLPosition;
	Vec3 BRPosition;
	Vec3 TRPosition;
	Vec3 TLPosition;

	Vertex_PCU BLVert;
	Vertex_PCU BRVert;
	Vertex_PCU TRVert;
	Vertex_PCU TLVert;

	for (int longtitudeIndex = 0; longtitudeIndex < longitudeNum; longtitudeIndex++)
	{
		currentLatitude = -90.0f;

		for (int latitudetitudeIndex = 0; latitudetitudeIndex < numLatitudeSlices; latitudetitudeIndex++)
		{
			BLPosition = Vec3::MakeFromPolarDegrees(currentLatitude, currentLongitude, radius);
			BRPosition = Vec3::MakeFromPolarDegrees(currentLatitude, currentLongitude + singleStepDegreesLong, radius);
			TRPosition = Vec3::MakeFromPolarDegrees(currentLatitude + singleStepDegreesLati, currentLongitude + singleStepDegreesLong, radius);
			TLPosition = Vec3::MakeFromPolarDegrees(currentLatitude + singleStepDegreesLati, currentLongitude, radius);

			BLPosition += center;
			BRPosition += center;
			TRPosition += center;
			TLPosition += center;

			BLVert = Vertex_PCU(BLPosition, color, UVs.GetPointAtUV(Vec2(longtitudeIndex * 0.5f * UVPercentage, latitudetitudeIndex * UVPercentage)));
			BRVert = Vertex_PCU(BRPosition, color, UVs.GetPointAtUV(Vec2((longtitudeIndex + 1) * 0.5f * UVPercentage, latitudetitudeIndex * UVPercentage)));
			TRVert = Vertex_PCU(TRPosition, color, UVs.GetPointAtUV(Vec2((longtitudeIndex + 1) * 0.5f * UVPercentage, (latitudetitudeIndex + 1) * UVPercentage)));
			TLVert = Vertex_PCU(TLPosition, color, UVs.GetPointAtUV(Vec2(longtitudeIndex * 0.5f * UVPercentage, (latitudetitudeIndex + 1) * UVPercentage)));

			verts.push_back(BLVert);
			verts.push_back(BRVert);
			verts.push_back(TRVert);

			verts.push_back(BLVert);
			verts.push_back(TRVert);
			verts.push_back(TLVert);

			currentLatitude += singleStepDegreesLati;
		}

		currentLongitude += singleStepDegreesLong;
	}

}

void AddVertsForSphere3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, const Vec3& center, float radius, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/, int numLatitudeSlices /*= 8*/, int numLongtitudeSlices /*= 16*/)
{
	// 1. Add the bottom pole of the sphere to the verts list

	Vec3 bottomPolePosition = center - Vec3(0.0f, 0.0f, radius);
	Vertex_PCUTBN bottomPoleVert = Vertex_PCUTBN(bottomPolePosition, color, UVs.GetPointAtUV(Vec2::ZERO));
	bottomPoleVert.m_normal = Vec3(0.f, 0.f, -1.f);
	verts.push_back(bottomPoleVert);


	// 2. Add vert for each slice in each stack	
	float singleStepDegreesLati = 180.0f / numLatitudeSlices;
	float singleStepDegreesLong = 360.0f / numLongtitudeSlices;

	float UVPercentageVertically = 1.0f / numLatitudeSlices;
	float UVPercentageHorizontally = 1.0f / numLongtitudeSlices;

	for (int stackIndex = 1; stackIndex < numLatitudeSlices; stackIndex++)
	{
		float currentLatitude = -90.0f + singleStepDegreesLati * stackIndex;

		for (int sliceIndex = 0; sliceIndex <= numLongtitudeSlices; sliceIndex++)
		{
			float currentLongitude = singleStepDegreesLong * sliceIndex;

			Vec3 vertPosition = Vec3::MakeFromPolarDegrees(currentLatitude, currentLongitude, radius) + center;
			
			Vec2 vertUVPercentage = Vec2(sliceIndex * UVPercentageHorizontally, stackIndex * UVPercentageVertically);

			Vertex_PCUTBN vert = Vertex_PCUTBN(vertPosition, color, UVs.GetPointAtUV(vertUVPercentage));

			vert.m_normal = (vertPosition - center).GetNormalized();

			verts.push_back(vert);
		}
	}

	// 3. Add vert for top pole of the sphere
	Vec3 topPolePosition = center + Vec3(0.0f, 0.0f, radius);
	Vertex_PCUTBN topPoleVert = Vertex_PCUTBN(topPolePosition, color, UVs.GetPointAtUV(Vec2(0.0f, 1.0f)));
	topPoleVert.m_normal = Vec3(0.f, 0.f, 1.f);
	verts.push_back(topPoleVert);

	// 4. Add indices for bottom stack
	for (int sliceIndex = 1; sliceIndex <= numLongtitudeSlices; sliceIndex++)
	{
		indexes.push_back(0);
		indexes.push_back(sliceIndex+1);
		indexes.push_back(sliceIndex);
	}

	// 5. Add indices for middle part of the stacks
	for (int i = 1; i < numLatitudeSlices - 1; ++i) 
	{
		for (int j = 0; j < numLongtitudeSlices; ++j) 
		{
			int BL = (i - 1) * (numLongtitudeSlices + 1) + j + 1;
			int BR = BL + 1;
			int TR = BR + numLongtitudeSlices + 1;
			int TL = BL + numLongtitudeSlices + 1;

			indexes.push_back(BL);
			indexes.push_back(BR);
			indexes.push_back(TR);

			indexes.push_back(BL);
			indexes.push_back(TR);
			indexes.push_back(TL);
		}
	}

	//// 6. Add indices for top part of the stacks
	unsigned int topPoleIndex = (unsigned int)verts.size() - 1;
	for (int sliceIndex = 1; sliceIndex <= numLongtitudeSlices; sliceIndex++) 
	{
		int currentVertIndex = (numLatitudeSlices - 2) * (numLongtitudeSlices + 1) + sliceIndex;
		int nextVertIndex = currentVertIndex + 1;

		indexes.push_back(currentVertIndex);
		indexes.push_back(nextVertIndex);
		indexes.push_back(topPoleIndex);
	}
}

void AddVertsForOBB3D(std::vector<Vertex_PCU>& verts, OBB3 const& obb, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	verts.reserve(36);

	Vec3 box[] =
	{
		obb.GetPointAt(Vec3(0.0f,0.0f,0.0f)),
		obb.GetPointAt(Vec3(1.0f,0.0f,0.0f)),
		obb.GetPointAt(Vec3(1.0f,0.0f,1.0f)),
		obb.GetPointAt(Vec3(0.0f,0.0f,1.0f)),
		obb.GetPointAt(Vec3(0.0f,1.0f,0.0f)),
		obb.GetPointAt(Vec3(1.0f,1.0f,0.0f)),
		obb.GetPointAt(Vec3(1.0f,1.0f,1.0f)),
		obb.GetPointAt(Vec3(0.0f,1.0f,1.0f))
	};

	AddVertsForQuad3D(verts, box[0], box[1], box[2], box[3], color, UVs);
	AddVertsForQuad3D(verts, box[4], box[0], box[3], box[7], color, UVs);
	AddVertsForQuad3D(verts, box[5], box[4], box[7], box[6], color, UVs);
	AddVertsForQuad3D(verts, box[1], box[5], box[6], box[2], color, UVs);
	AddVertsForQuad3D(verts, box[7], box[3], box[2], box[6], color, UVs);
	AddVertsForQuad3D(verts, box[5], box[1], box[0], box[4], color, UVs);

}

void AddVertsForTetrahedron3D(std::vector<Vertex_PCU>& verts, Tetrahedron const& tetrahedron, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	verts.reserve(12);

	Vertex_PCU vertA = Vertex_PCU(tetrahedron.m_PointA, color, UVs.GetUVForPoint(Vec2::ZERO));
	Vertex_PCU vertB = Vertex_PCU(tetrahedron.m_PointB, color, UVs.GetUVForPoint(Vec2(0.0f,1.0f)));
	Vertex_PCU vertC = Vertex_PCU(tetrahedron.m_PointC, color, UVs.GetUVForPoint(Vec2(1.0f, 0.0f)));
	Vertex_PCU vertD = Vertex_PCU(tetrahedron.m_PointD, color, UVs.GetUVForPoint(Vec2(1.0f, 1.0f)));

	verts.push_back(vertA);
	verts.push_back(vertB);
	verts.push_back(vertC);

	verts.push_back(vertA);
	verts.push_back(vertC);
	verts.push_back(vertB);

	verts.push_back(vertA);
	verts.push_back(vertC);
	verts.push_back(vertD);
	verts.push_back(vertA);
	verts.push_back(vertD);
	verts.push_back(vertC);


	verts.push_back(vertA);
	verts.push_back(vertD);
	verts.push_back(vertB);
	verts.push_back(vertA);
	verts.push_back(vertB);
	verts.push_back(vertD);

	verts.push_back(vertB);
	verts.push_back(vertC);
	verts.push_back(vertD);

	verts.push_back(vertB);
	verts.push_back(vertD);
	verts.push_back(vertC);

}


void AddVertsForHexagon3D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, Hexagon const& hexagon,  float edgeLength, Rgba8 const& color /*= Rgba8::WHITE*/)
{
	float innerRadius = hexagon.m_innerRadius; 
	float circumradius = innerRadius / CosDegrees(30.0f); 
	float halfThickness = edgeLength / 2.0f; 
	float sideExtension = edgeLength / 4.0f; 
	int vertexesSize = static_cast<int>(verts.size());

	for (int i = 0; i < 6; ++i) 
	{
		float angle1 = i * 60.0f;
		float angle2 = (i + 1) * 60.0f;

		Vec3 point1(circumradius * CosDegrees(angle1), circumradius * SinDegrees(angle1), 0.0f);
		Vec3 point2(circumradius * CosDegrees(angle2), circumradius * SinDegrees(angle2), 0.0f);

		Vec3 direction = (point2 - point1).GetNormalized();
		Vec3 perpendicular(-direction.y, direction.x, direction.z); 

		Vec3 extendedPoint1 = point1 - direction * sideExtension;
		Vec3 extendedPoint2 = point2 + direction * sideExtension;

		Vec3 vert1 = extendedPoint1 + perpendicular * halfThickness;
		Vec3 vert2 = extendedPoint1 - perpendicular * halfThickness;
		Vec3 vert3 = extendedPoint2 + perpendicular * halfThickness;
		Vec3 vert4 = extendedPoint2 - perpendicular * halfThickness;

		verts.push_back(Vertex_PCU(vert1 + hexagon.m_center, color, Vec2::ONE));
		verts.push_back(Vertex_PCU(vert2 + hexagon.m_center, color, Vec2::ONE));
		verts.push_back(Vertex_PCU(vert3 + hexagon.m_center, color, Vec2::ONE));
		verts.push_back(Vertex_PCU(vert4 + hexagon.m_center, color, Vec2::ONE));

		unsigned int idx = vertexesSize + i * 4;

		indexes.push_back(idx);
		indexes.push_back(idx + 1);
		indexes.push_back(idx + 2);
		indexes.push_back(idx + 1);
		indexes.push_back(idx + 3);
		indexes.push_back(idx + 2);
	}
}

void AddVertsForSolidHexagon3D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, Hexagon const& hexagon, float edgeLength, Rgba8 const& color /*= Rgba8::WHITE*/)
{
	float innerRadius = hexagon.m_innerRadius;
	float circumradius = innerRadius + edgeLength; // Solid hexagon should occupy the previous hollow part

	Vertex_PCU centerVert = Vertex_PCU(hexagon.m_center, color, Vec2::ZERO);
	int vertexesSize = static_cast<int>(verts.size());

	verts.push_back(centerVert);

	for (int i = 0; i < 6; ++i)
	{
		float angle1 = i * 60.0f;
		float angle2 = (i + 1) * 60.0f;

		Vec3 point1(circumradius * CosDegrees(angle1), circumradius * SinDegrees(angle1), 0.0f);
		Vec3 point2(circumradius * CosDegrees(angle2), circumradius * SinDegrees(angle2), 0.0f);

		verts.push_back(Vertex_PCU(point1 + hexagon.m_center, color, Vec2::ONE));
		verts.push_back(Vertex_PCU(point2 + hexagon.m_center, color, Vec2::ONE));

		unsigned int idx = vertexesSize + i * 2 + 1;

		indexes.push_back(vertexesSize); // Center vertex
		indexes.push_back(idx);          // First vertex of the edge
		indexes.push_back(idx + 1);      // Second vertex of the edge
	}
}

void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, const Mat44& transform)
{
	for (int i = 0; i < (int)verts.size(); i++)
	{
		verts[i].m_position = transform.TransformPosition3D(verts[i].m_position);
	}
}

void TransformVertexArray3D(std::vector<Vertex_PCUTBN>& verts, const Mat44& transform)
{
	for (int i = 0; i < (int)verts.size(); i++)
	{
		verts[i].m_position = transform.TransformPosition3D(verts[i].m_position);
		
		verts[i].m_normal = transform.TransformVectorQuantity3D(verts[i].m_normal).GetNormalized();
		verts[i].m_tangent = transform.TransformVectorQuantity3D(verts[i].m_tangent).GetNormalized();
		verts[i].m_bitangent = transform.TransformVectorQuantity3D(verts[i].m_bitangent).GetNormalized();
	}
}

AABB2 GetVertexBounds2D(const std::vector<Vertex_PCU>& verts)
{
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;

	for (int i = 0; i < (int)verts.size(); i++)
	{
		if (verts[i].m_position.x < minX)
		{
			minX = verts[i].m_position.x;
		}
		if (verts[i].m_position.x > maxX)
		{
			maxX = verts[i].m_position.x;
		}
		if (verts[i].m_position.y < minY)
		{
			minY = verts[i].m_position.y;
		}
		if (verts[i].m_position.y > maxY)
		{
			maxY = verts[i].m_position.y;
		}
	}

	return AABB2(minX, minY, maxX, maxY);
}

void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts, const Vec3& start, const Vec3& end, float radius, const Rgba8& color, const AABB2& UVs, int numSlices)
{
	std::vector<Vertex_PCU> localVerts;
	localVerts.reserve(1000);
	Vec3 worldZBasis = Vec3(0.0f, 0.0f, 1.0f);
	Vec3 worldYBasis = Vec3(0.0f, 1.0f, 0.0f);
	
	Vec3 cylinderIBasis = (end - start).GetNormalized();
	Vec3 cylinderJBasis;
	Vec3 cylinderKBasis;

	if (fabsf(DotProduct3D(worldZBasis, cylinderIBasis)) < 0.999f)
	{
		cylinderJBasis = CrossProduct3D(worldZBasis, cylinderIBasis).GetNormalized();
		cylinderKBasis = CrossProduct3D(cylinderIBasis, cylinderJBasis).GetNormalized();
	}
	else 
	{
		cylinderKBasis = CrossProduct3D(cylinderIBasis, worldYBasis).GetNormalized();
		cylinderJBasis = CrossProduct3D(cylinderKBasis, cylinderIBasis).GetNormalized();
	}

	Mat44 transformMatrix = Mat44();
	transformMatrix.SetIJKT3D(cylinderIBasis, cylinderJBasis, cylinderKBasis, start);

	float SINGLE_UV_STEP = 1.0f / numSlices;
	float SINGLE_ANGLE_STEP = 360.0f * SINGLE_UV_STEP;
	float currentAngle = 0.0f;
	float currentUVForSideView = 0.0f;

	Vec2 rightDirectionStandard = Vec2(0.0f, 0.5f);
	Vec2 rightDirectionForUV = Vec2(0.0f, 0.5f);

	Vec3 rightDirectionLocal = Vec3(0.0f, -1.0f, 0.0f) * radius;
	Vec3 upDiretionLocal = Vec3(1.0f, 0.0f, 0.0f) * (end - start).GetLength();
	Vec3 startCenterLocal = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 endCenterLocal = startCenterLocal + upDiretionLocal;

	Vertex_PCU startCenterLocalPCU = Vertex_PCU(startCenterLocal, color, UVs.GetPointAtUV(Vec2(0.5f, 0.5f)));
	Vertex_PCU endCenterLocalPCU = Vertex_PCU(endCenterLocal, color, UVs.GetPointAtUV(Vec2(0.5f, 0.5f)));

	Vec3 startLowPoint;
	Vec3 startHighPoint;
	Vec3 endLowPoint;
	Vec3 endHighPoint;

	Vertex_PCU PCU_startLowPointForBottom;
	Vertex_PCU PCU_startHighPointForBottom;
	Vertex_PCU PCU_endLowPointForTop;
	Vertex_PCU PCU_endHighPointForTop;

	Vertex_PCU PCU_startLowPointForSideView;
	Vertex_PCU PCU_startHighPointForSideView;
	Vertex_PCU PCU_endLowPointForSideView;
	Vertex_PCU PCU_endHighPointForSideView;

	for (int i = 0; i < numSlices; i++)
	{
		Mat44 rotationAroundXMatrix;
		Mat44 rotationAroundWorldZForUV;

		rotationAroundXMatrix.AppendXRotation(currentAngle);
		rotationAroundWorldZForUV.AppendZRotation(currentAngle);

		startLowPoint = rotationAroundXMatrix.TransformPosition3D(rightDirectionLocal);
		endLowPoint = startLowPoint + upDiretionLocal;
		rightDirectionForUV = rotationAroundWorldZForUV.TransformPosition2D(rightDirectionStandard);

		PCU_startLowPointForBottom = Vertex_PCU(startLowPoint, color, UVs.GetPointAtUV(Vec2(0.5f, 0.5f) + rightDirectionForUV));
		PCU_startLowPointForSideView = Vertex_PCU(startLowPoint, color, UVs.GetPointAtUV(Vec2(currentUVForSideView, 0.0f)));
		PCU_endLowPointForSideView = Vertex_PCU(endLowPoint, color, UVs.GetPointAtUV(Vec2(currentUVForSideView, 1.0f)));
		PCU_endLowPointForTop = Vertex_PCU(endLowPoint, color, UVs.GetPointAtUV(Vec2(0.5f, 0.5f) + rightDirectionForUV));


		rotationAroundXMatrix.AppendXRotation(SINGLE_ANGLE_STEP);
		rotationAroundWorldZForUV.AppendZRotation(SINGLE_ANGLE_STEP);

		startHighPoint = rotationAroundXMatrix.TransformPosition3D(rightDirectionLocal);
		rightDirectionForUV = rotationAroundWorldZForUV.TransformVectorQuantity2D(rightDirectionStandard);
		endHighPoint = startHighPoint +upDiretionLocal;

		PCU_startHighPointForBottom = Vertex_PCU(startHighPoint, color,  UVs.GetPointAtUV(Vec2(0.5f, 0.5f) + rightDirectionForUV));
		PCU_startHighPointForSideView = Vertex_PCU(startHighPoint, color, UVs.GetPointAtUV(Vec2(currentUVForSideView + SINGLE_UV_STEP, 0.0f)));	
		PCU_endHighPointForSideView = Vertex_PCU(endHighPoint, color,  UVs.GetPointAtUV(Vec2(currentUVForSideView + SINGLE_UV_STEP, 1.0f)));
		PCU_endHighPointForTop = Vertex_PCU(endHighPoint, color,  UVs.GetPointAtUV(Vec2(0.5f, 0.5f) + rightDirectionForUV));

		localVerts.push_back(startCenterLocalPCU);
		localVerts.push_back(PCU_startHighPointForBottom);
		localVerts.push_back(PCU_startLowPointForBottom);
		
		localVerts.push_back(PCU_startHighPointForSideView);
		localVerts.push_back(PCU_endLowPointForSideView);
		localVerts.push_back(PCU_startLowPointForSideView);
		
		localVerts.push_back(PCU_startHighPointForSideView);
		localVerts.push_back(PCU_endHighPointForSideView);
		localVerts.push_back(PCU_endLowPointForSideView);
		
		localVerts.push_back(endCenterLocalPCU);
		localVerts.push_back(PCU_endLowPointForTop);
		localVerts.push_back(PCU_endHighPointForTop);

		currentAngle+=SINGLE_ANGLE_STEP;
		currentUVForSideView += SINGLE_UV_STEP;
	}

	TransformVertexArray3D(localVerts, transformMatrix);

	for (Vertex_PCU vert : localVerts)
	{
		verts.push_back(vert);
	}
}

void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts, const Cylinder3& cylinder, const Rgba8& color, const AABB2& UVs, int numSlices)
{
	AddVertsForCylinder3D(verts, cylinder.m_start, cylinder.m_end, cylinder.m_radius, color, UVs, numSlices);
}

void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, const Vec3& start, const Vec3& end, float radius, const Rgba8& color, const AABB2& UVs, int numSlices)
{
	verts.reserve(1000);
	Vec3 worldZBasis = Vec3(0.0f, 0.0f, 1.0f);
	Vec3 worldYBasis = Vec3(0.0f, 1.0f, 0.0f);

	Vec3 cylinderIBasis = (end - start).GetNormalized();
	Vec3 cylinderJBasis;
	Vec3 cylinderKBasis;

	if (fabsf(DotProduct3D(worldZBasis, cylinderIBasis)) < 0.999f)
	{
		cylinderJBasis = CrossProduct3D(worldZBasis, cylinderIBasis);
		cylinderKBasis = CrossProduct3D(cylinderIBasis, cylinderJBasis);
	}
	else
	{
		cylinderKBasis = CrossProduct3D(cylinderIBasis, worldYBasis);
		cylinderJBasis = CrossProduct3D(cylinderKBasis, cylinderIBasis);
	}

	Mat44 transformMatrix = Mat44();
	transformMatrix.SetIJKT3D(cylinderIBasis, cylinderJBasis, cylinderKBasis, start);

	float SINGLE_UV_STEP = 1.0f / numSlices;
	float SINGLE_ANGLE_STEP = 360.0f * SINGLE_UV_STEP;
	float currentAngle = 0.0f;
	float currentUVForSideView = 0.0f;

	Vec2 rightDirectionStandard = Vec2(0.0f, 0.5f);
	Vec2 rightDirectionForUV = Vec2(0.0f, 0.5f);

	Vec3 rightDirectionLocal = Vec3(0.0f, -1.0f, 0.0f) * radius;
	Vec3 upDiretionLocal = Vec3(1.0f, 0.0f, 0.0f) * (end - start).GetLength();
	Vec3 startCenterLocal = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 endCenterLocal = startCenterLocal + upDiretionLocal;

	Vertex_PCU startCenterLocalPCU = Vertex_PCU(startCenterLocal, color, UVs.GetPointAtUV(Vec2(0.5f, 0.5f)));
	Vertex_PCU endCenterLocalPCU = Vertex_PCU(endCenterLocal, color, UVs.GetPointAtUV(Vec2(0.5f, 0.5f)));

	Vec3 startLowPoint;
	Vec3 startHighPoint;

	Vertex_PCU PCU_startLowPointForBottom;
	Vertex_PCU PCU_startHighPointForBottom;

	Vertex_PCU PCU_startLowPointForSideView;
	Vertex_PCU PCU_startHighPointForSideView;
	for (int i = 0; i < numSlices; i++)
	{
		Mat44 rotationAroundXMatrix;
		Mat44 rotationAroundWorldZForUV;

		rotationAroundXMatrix.AppendXRotation(currentAngle);
		rotationAroundWorldZForUV.AppendZRotation(currentAngle);

		startLowPoint = rotationAroundXMatrix.TransformPosition3D(rightDirectionLocal);
		rightDirectionForUV = rotationAroundWorldZForUV.TransformPosition2D(rightDirectionStandard);

		PCU_startLowPointForBottom = Vertex_PCU(startLowPoint, color, UVs.GetPointAtUV(Vec2(0.5f, 0.5f) + rightDirectionForUV));
		PCU_startLowPointForSideView = Vertex_PCU(startLowPoint, color, UVs.GetPointAtUV(Vec2(currentUVForSideView, 0.0f)));


		rotationAroundXMatrix.AppendXRotation(SINGLE_ANGLE_STEP);
		rotationAroundWorldZForUV.AppendZRotation(SINGLE_ANGLE_STEP);

		startHighPoint = rotationAroundXMatrix.TransformPosition3D(rightDirectionLocal);
		rightDirectionForUV = rotationAroundWorldZForUV.TransformVectorQuantity2D(rightDirectionStandard);

		PCU_startHighPointForBottom = Vertex_PCU(startHighPoint, color, UVs.GetPointAtUV(Vec2(0.5f, 0.5f) + rightDirectionForUV));
		PCU_startHighPointForSideView = Vertex_PCU(startHighPoint, color, UVs.GetPointAtUV(Vec2(currentUVForSideView + SINGLE_UV_STEP, 0.0f)));

		verts.push_back(startCenterLocalPCU);
		verts.push_back(PCU_startHighPointForBottom);
		verts.push_back(PCU_startLowPointForBottom);


		verts.push_back(PCU_startHighPointForSideView);
		verts.push_back(endCenterLocalPCU);
		verts.push_back(PCU_startLowPointForSideView);

		currentAngle += SINGLE_ANGLE_STEP;
		currentUVForSideView += SINGLE_UV_STEP;
	}

	TransformVertexArray3D(verts, transformMatrix);

}

void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, const Vec3& start, const Vec3& end, float radius, float arrowAspect /*= 0.2f*/, const Rgba8& color /*= Rgba8::WHITE*/, const Rgba8& coneColor, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/, int numSlices /*= 8*/)
{
	UNUSED(UVs);

	std::vector<Vertex_PCU> localVerts;
	localVerts.reserve(1000);
	verts.reserve(1000);
	Vec3 worldZBasis = Vec3(0.0f, 0.0f, 1.0f);
	Vec3 worldYBasis = Vec3(0.0f, 1.0f, 0.0f);

	Vec3 cylinderIBasis = (end - start).GetNormalized();
	Vec3 cylinderJBasis;
	Vec3 cylinderKBasis;

	if (fabsf(DotProduct3D(worldZBasis, cylinderIBasis)) < 0.999f)
	{
		cylinderJBasis = CrossProduct3D(worldZBasis, cylinderIBasis).GetNormalized();
		cylinderKBasis = CrossProduct3D(cylinderIBasis, cylinderJBasis).GetNormalized();;
	}
	else
	{
		cylinderKBasis = CrossProduct3D(cylinderIBasis, worldYBasis).GetNormalized();;
		cylinderJBasis = CrossProduct3D(cylinderKBasis, cylinderIBasis).GetNormalized();;
	}

	Mat44 transformMatrix = Mat44();
	transformMatrix.SetIJKT3D(cylinderIBasis, cylinderJBasis, cylinderKBasis, start);

	float SINGLE_ANGLE_STEP = 360.0f / numSlices;
	float currentAngle = 0.0f;

	Vec3 rightDirectionLocal = Vec3(0.0f, -1.0f, 0.0f) * radius;
	Vec3 rightConeDirectionLocal = Vec3(0.0f, -1.0f, 0.0f) * radius * 2.0f;
	Vec3 upDiretionLocal = Vec3(1.0f, 0.0f, 0.0f) * (end - start).GetLength();
	Vec3 startCenterLocal = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 endCenterLocal = startCenterLocal + upDiretionLocal;

	Vertex_PCU startCenterLocalPCU = Vertex_PCU(startCenterLocal, color, Vec2(1.0f, 1.0f));
	Vertex_PCU endCenterLocalPCU = Vertex_PCU(endCenterLocal, coneColor, Vec2(1.0f, 0.0f));

	Vec3 startLowPoint;
	Vec3 startHighPoint;
	Vec3 endLowPoint;
	Vec3 endHighPoint;

	Vec3 endLowPointLongRadius;
	Vec3 endHightPointLongRadius;
	Vec3 coneHighPoint;

	Vertex_PCU PCU_startLowPoint;
	Vertex_PCU PCU_startHighPoint;
	Vertex_PCU PCU_endLowPoint;
	Vertex_PCU PCU_endHighPoint;

	Vertex_PCU PCU_endLowPointLongRadius;
	Vertex_PCU PCU_endHightPointLongRadius;
	Vertex_PCU PCU_coneHighPoint;

	for (int i = 0; i < numSlices; i++)
	{
		Mat44 rotationAroundXMatrix;
		rotationAroundXMatrix.AppendXRotation(currentAngle);
		startLowPoint = rotationAroundXMatrix.TransformPosition3D(rightDirectionLocal);
		Vec3 startLowPointLongRadius = rotationAroundXMatrix.TransformPosition3D(rightConeDirectionLocal);
		PCU_startLowPoint = Vertex_PCU(startLowPoint, color, Vec2(0.0f, 0.0f));

		rotationAroundXMatrix.AppendXRotation(SINGLE_ANGLE_STEP);
		startHighPoint = rotationAroundXMatrix.TransformPosition3D(rightDirectionLocal);
		Vec3 startHighPointLongRadius = rotationAroundXMatrix.TransformPosition3D(rightConeDirectionLocal);
		PCU_startHighPoint = Vertex_PCU(startHighPoint, color, Vec2(1.0f, 0.0f));

		localVerts.push_back(startCenterLocalPCU);
		localVerts.push_back(PCU_startHighPoint);
		localVerts.push_back(PCU_startLowPoint);

		endLowPoint = startLowPoint + upDiretionLocal;
		PCU_endLowPoint = Vertex_PCU(endLowPoint, color, Vec2(0.0f, 1.0f));
		endHighPoint = startHighPoint + upDiretionLocal;
		PCU_endHighPoint = Vertex_PCU(endHighPoint, color, Vec2(1.0f, 1.0f));

		endLowPointLongRadius = startLowPointLongRadius + upDiretionLocal;
		PCU_endLowPointLongRadius = Vertex_PCU(endLowPointLongRadius, coneColor, Vec2(0.0f, 1.0f));
		endHightPointLongRadius = startHighPointLongRadius + upDiretionLocal;
		PCU_endHightPointLongRadius = Vertex_PCU(endHightPointLongRadius, coneColor, Vec2(1.0f, 1.0f));

		coneHighPoint = endCenterLocal + upDiretionLocal * arrowAspect;
		PCU_coneHighPoint = Vertex_PCU(coneHighPoint, coneColor, Vec2(1.0f, 1.0f));

		localVerts.push_back(PCU_startHighPoint);
		localVerts.push_back(PCU_endLowPoint);
		localVerts.push_back(PCU_startLowPoint);

		localVerts.push_back(PCU_startHighPoint);
		localVerts.push_back(PCU_endHighPoint);
		localVerts.push_back(PCU_endLowPoint);

		localVerts.push_back(endCenterLocalPCU);
		localVerts.push_back(PCU_endLowPoint);
		localVerts.push_back(PCU_endHighPoint);

		//verts.push_back(endCenterLocalPCU);
		//verts.push_back(PCU_endHightPointLongRadius);
		//verts.push_back(PCU_endLowPointLongRadius);

		// Shadow for base of the cone in the arrow
		Vertex_PCU endCenterLocalPCUShadow = Vertex_PCU(endCenterLocal, coneColor * 0.8f, Vec2(1.0f, 0.0f));
		Vertex_PCU endLowPointPCUShadow = Vertex_PCU(endLowPointLongRadius, coneColor * 0.8f, Vec2(0.0f, 1.0f));
		Vertex_PCU endHighPointPCUShadow = Vertex_PCU(endHightPointLongRadius, coneColor * 0.8f, Vec2(0.0f, 1.0f));

		localVerts.push_back(endCenterLocalPCUShadow);
		localVerts.push_back(endHighPointPCUShadow);
		localVerts.push_back(endLowPointPCUShadow);

		localVerts.push_back(PCU_endHightPointLongRadius);
		localVerts.push_back(PCU_coneHighPoint);
		localVerts.push_back(PCU_endLowPointLongRadius);

		currentAngle += SINGLE_ANGLE_STEP;
	}

	TransformVertexArray3D(localVerts, transformMatrix);

	for (Vertex_PCU vert : localVerts)
	{
		verts.push_back(vert);
	}
}

void TransformVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY)
{
	for (int vertIndex = 0; vertIndex < numVerts; vertIndex++)
	{
		Vec2 vertPositionXY(verts[vertIndex].m_position.x, verts[vertIndex].m_position.y);
		float tmp_z = verts[vertIndex].m_position.z;
		float tmp_length = vertPositionXY.GetLength();


		vertPositionXY.SetLength(uniformScaleXY * tmp_length);				// Set scale of the vector
		vertPositionXY.RotateDegrees(rotationDegreesAboutZ);				// Rotate vector around the Z axis
		vertPositionXY += translationXY;									// Translate vector position


		verts[vertIndex].m_position.x = vertPositionXY.x;
		verts[vertIndex].m_position.y = vertPositionXY.y;
		verts[vertIndex].m_position.z = tmp_z;
	}
}

void CalculateTangentSpaceBasisVectors(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, bool computeNormals /*= true*/, bool computeTangents /*= true */)
{

	for (int i = 0; i < (int)indexes.size(); i+=3)
	{
		unsigned int& index0 = indexes[i];
		unsigned int& index1 = indexes[i + 1];
		unsigned int& index2 = indexes[i + 2];

		Vertex_PCUTBN& vert0 = vertexes[index0];
		Vertex_PCUTBN& vert1 = vertexes[index1];
		Vertex_PCUTBN& vert2 = vertexes[index2];

		Vec3 e1 = vert1.m_position - vert0.m_position;
		Vec3 e2 = vert2.m_position - vert0.m_position;

		if (computeNormals)
		{
			Vec3 normal = CrossProduct3D(e1, e2).GetNormalized();

			vert0.m_normal = normal;
			vert1.m_normal = normal;
			vert2.m_normal = normal;
		}

		Vec2 deltaUV0 = vert1.m_uvTexCoords - vert0.m_uvTexCoords;
		Vec2 deltaUV1 = vert2.m_uvTexCoords - vert0.m_uvTexCoords;

		float f = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

		Vec3 tangentBasis = Vec3::ZERO;
		Vec3 bitangentBasis = Vec3::ZERO;

		if (computeTangents == true)
		{
			tangentBasis = f * (deltaUV1.y * e1 - deltaUV0.y * e2);
			bitangentBasis = f * (-deltaUV1.x * e1 + deltaUV0.x * e2);

			tangentBasis.Normalize();
			bitangentBasis.Normalize();
		}

		vert0.m_tangent = tangentBasis;
		vert0.m_bitangent = bitangentBasis;
		vert1.m_tangent = tangentBasis;
		vert1.m_bitangent = bitangentBasis;
		vert2.m_tangent = tangentBasis;
		vert2.m_bitangent = bitangentBasis;

	}
}
