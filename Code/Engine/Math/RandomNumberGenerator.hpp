#pragma once

struct Vec2;
struct Vec3;
struct Rgba8;

class RandomNumberGenerator
{
public:

	RandomNumberGenerator(unsigned int seed = 0) : m_seed(seed) {};

	int			RollRandomIntLessThan( int maxNotInclusive );
	int			RollRandomIntRange( int minInclusive, int maxInclusive );
	float		RollRandomFloatZeroToOne();
	float		RollRandomFloatInRange( float minInclusive, float maxInclusive );

	Vec3		RollRandomVectorOnUnitSphere();
	Vec2		RollRandomVectorOnUnitCircle();
	Rgba8		RollRandomColor(bool enableAlphaRandom = false);

private:
	unsigned int m_seed = 0;
	int	m_position = 0;
};

