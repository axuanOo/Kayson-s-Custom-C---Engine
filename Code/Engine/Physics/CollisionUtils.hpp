#pragma once
#include "Engine/Core/EngineCommon.hpp"

struct CollisionResult2D
{

	void Clear() { m_didImpact = false, m_minimumTranslationVector = Vec2::ZERO; };

	bool m_didImpact = false;
	Vec2 m_minimumTranslationVector = Vec2::ZERO;

};


CollisionResult2D OBBCollisionWithOBB2D(OBB2 const& obb1, OBB2 const& obb2);