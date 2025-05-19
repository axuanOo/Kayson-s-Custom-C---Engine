#pragma once
#include "Engine/Math/Vec3.hpp"
class Timer;

class SignedDistanceField 
{
public:
	virtual ~SignedDistanceField();
	virtual bool	IsInsideShape(Vec3 const& point) const = 0;
	virtual float	GetDistanceFromSurface(Vec3 const& point) const = 0;
	virtual void		Update();
public:
	Timer*			m_timer;
	bool			m_hasFinished = false;
	bool			m_hasExpired = false;
};

class CappedConeSDF : public SignedDistanceField 
{
	virtual bool	IsInsideShape(Vec3 const& point) const override;
	virtual float	GetDistanceFromSurface(Vec3 const& point) const override;
};

class SphereSDF : public SignedDistanceField 
{
public:

	SphereSDF(Vec3 const& center, float radius, bool isDynamic = false, float timePeriod = 0.0f);
	virtual ~SphereSDF() override;

	virtual bool	IsInsideShape(Vec3 const& point) const override;
	virtual float	GetDistanceFromSurface(Vec3 const& point) const override;
	virtual void	Update() override;

	void			SetSphereCenter(Vec3 const& center);
	void			SetRadius(float radius);
public:
	Vec3			m_center;
	float			m_radius = 0.0f;
	float			m_desiredRadius;
};