#include "Engine/Math/SDF.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Timer.hpp"
#include <Engine/Core/EngineCommon.hpp>

bool CappedConeSDF::IsInsideShape(Vec3 const& point) const
{
	UNUSED(point);
	return false;
}

float CappedConeSDF::GetDistanceFromSurface(Vec3 const& point) const
{
	UNUSED(point);

	return 1.0f;
}

SphereSDF::SphereSDF(Vec3 const& center, float radius, bool isDynamic, float timePeriod)
	:m_center(center), m_desiredRadius(radius)
{
	if (isDynamic)
	{
		m_timer = new Timer(timePeriod);
		m_timer->Start();
		m_radius = 0.0f;
	}
	else 
	{
		m_radius = m_desiredRadius;
	}
}

SphereSDF::~SphereSDF()
{
	delete m_timer;
}

bool SphereSDF::IsInsideShape(Vec3 const& point) const
{
	float distanceSquare = GetDistanceSquared3D(point,m_center);
	return distanceSquare < m_radius * m_radius;
}

float SphereSDF::GetDistanceFromSurface(Vec3 const& point) const
{
	float distance = GetDistance3D(m_center,point);
	return distance - m_radius;
}

void SphereSDF::Update()
{

	if (m_hasFinished == false)
	{
		if (m_timer && m_timer->IsStopped() == false)
		{
			m_radius = m_desiredRadius * SmoothStop5(m_timer->GetElapsedFraction());
			if (m_radius > 0.99f * m_desiredRadius)
			{
				m_radius = m_desiredRadius;
				m_timer->Stop();
				m_hasFinished = true;
			}
		}
	}
	else
	{
		if (m_timer->IsStopped() && m_hasExpired == false)
		{
			m_timer->m_period = 5.0f;
			m_timer->Start();
			return;
		}

		if (m_timer->HasPeriodElapsed())
		{
			m_timer->Stop();
			m_hasExpired = true;
		}
	}
}

void SphereSDF::SetSphereCenter(Vec3 const& center)
{
	m_center = center;
}

void SphereSDF::SetRadius(float radius)
{
	m_radius = radius;
}

SignedDistanceField::~SignedDistanceField()
{
}

void SignedDistanceField::Update()
{

}
