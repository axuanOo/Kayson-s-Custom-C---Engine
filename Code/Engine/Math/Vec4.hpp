#pragma once

struct Vec2;

struct Vec4
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;
public:
	// Construction/Destruction
	~Vec4() {}															// destructor (do nothing)
	Vec4() {}															// default constructor (do nothing)
	Vec4(const Vec4& copyFrom);											// copy constructor (from another Vec4)
	explicit Vec4(float initialX, float initialY, float initialZ, float initialW);		// explicit constructor (from x, y,z,w)
	explicit Vec4(float initialX, float initialY, float initialZ);		// explicit constructor (from x, y,z,w)

	//  Accessors (const methods)
	float	   GetLength() const;
	float	   GetLengthXYZ() const;
	float	   GetLengthSquared() const;
	float	   GetLengthXYZSquared() const;

	// Operators (const)
	bool		operator==(const Vec4& compare) const;				// Vec4 == Vec4
	bool		operator!=(const Vec4& compare) const;				// Vec4 != Vec4
	const Vec4	operator+(const Vec4& vecToAdd) const;				// Vec4 + Vec4
	const Vec4	operator-(const Vec4& vecToSubtract) const;			// Vec4 - Vec4
	const Vec4	operator-() const;									// -Vec4, i.e. "unary negation"
	const Vec4	operator*(float uniformScale) const;				// Vec4 * float
	const Vec4	operator*(const Vec4& vecToMultiply) const;			// Vec4 * Vec4
	const Vec4	operator/(float inverseScale) const;				// Vec4 / float

																	// Operators (self-mutating / non-const)
	void		operator+=(const Vec4& vecToAdd);					// Vec4 += Vec4
	void		operator-=(const Vec4& vecToSubtract);				// Vec4 -= Vec4
	void		operator*=(const float uniformScale);				// Vec4 *= float
	void		operator/=(const float uniformDivisor);				// Vec4 /= float
	void		operator=(const Vec4& copyFrom);					// Vec4 = Vec4
																				// Standalone "friend" functions that are conceptually, but not actually, part of Vec4::
	friend const Vec4 operator*(float uniformScale, const Vec4& vecToScale);	// float * Vec4
};

