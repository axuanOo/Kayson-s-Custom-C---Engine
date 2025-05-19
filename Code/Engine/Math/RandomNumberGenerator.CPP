#include "RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "ThirdParty/Engine_Code_ThirdParty_Squirrel/RawNoise.hpp"
#include <stdlib.h>
#include <math.h>

int RandomNumberGenerator::RollRandomIntLessThan(int maxNotInclusive)
{
	unsigned int randomNumber = Get1dNoiseUint(m_position, m_seed);
    randomNumber = randomNumber % maxNotInclusive;
	m_position++;
    return randomNumber;
}

int RandomNumberGenerator::RollRandomIntRange(int minInclusive, int maxInclusive)
{
	unsigned int randomNumber = Get1dNoiseUint(m_position, m_seed);

	// Corrected the modulo operation to ensure correct range and avoid overflow
	int range = maxInclusive - minInclusive + 1;
	int result = randomNumber % range + minInclusive;

	m_position++;
	return result;
}

float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	return Get1dNoiseZeroToOne(m_position++, m_seed);
}

float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive)
{
    float range = maxInclusive - minInclusive;

	float randomNumber = minInclusive + (range* RollRandomFloatZeroToOne() );

    return randomNumber;
}

Vec3 RandomNumberGenerator::RollRandomVectorOnUnitSphere()
{
	float x = RollRandomFloatInRange(-1.0f,1.0f);
	float y = RollRandomFloatInRange(-1.0f,1.0f);
	float z = RollRandomFloatInRange(-1.0f,1.0f);

	float length = Vec3(x, y, z).GetLength();


	return Vec3(x / length, y / length, z / length);
}

Vec2 RandomNumberGenerator::RollRandomVectorOnUnitCircle()
{
	float x = RollRandomFloatInRange(-1.0f, 1.0f);
	float y = RollRandomFloatInRange(-1.0f, 1.0f);

	return Vec2(x,y).GetNormalized();
}

Rgba8 RandomNumberGenerator::RollRandomColor(bool enableAlphaRandom)
{
	unsigned char r = (unsigned char)RollRandomIntLessThan(255);
	unsigned char g = (unsigned char)RollRandomIntLessThan(255);
	unsigned char b = (unsigned char)RollRandomIntLessThan(255);
	
	unsigned char a = (unsigned char)0xFF;

	if (enableAlphaRandom)
	{
		a = (unsigned char)RollRandomIntLessThan(255);
	}

	return Rgba8(r, g, b, a);
}
