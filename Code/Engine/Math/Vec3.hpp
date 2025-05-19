#pragma once

struct Vec2;

struct Vec3
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

public:
	// Construction/Destruction
	~Vec3() {}															// destructor (do nothing)
	Vec3() {}															// default constructor (do nothing)
	Vec3(const Vec3& copyFrom);											// copy constructor (from another vec3)
	explicit Vec3(float initialX, float initialY, float initialZ = 0.0f);		// explicit constructor (from x, y,z)

	//  Accessors (const methods)
	float		GetLength() const;
	float		GetLengthXY() const;
	float		GetLengthSquared() const;
	float		GetLengthXYSquared() const;
	float		GetAngleAboutZRadians() const;
	float		GetAngleAboutZDegrees() const;
	Vec3 const	GetRotatedAboutZRadians (float deltaRadians ) const;
	Vec3 const	GetRotatedAboutZDegrees (float deltaDegrees ) const;
	Vec3 const	GetClamped (float maxLength ) const;
	Vec3 const	GetNormalized() const;
	void		Normalize();
	void		SetFromText(char const* text);

	static const Vec3 ZERO;
	static const Vec3 ONE;

	const static Vec3 MakeFromPolarRadians(float latitudeRadians, float longitudeRadians, float length = 1.0f);
	const static Vec3 MakeFromPolarDegrees(float latitudeDegrees, float longitudeDegrees, float length = 1.0f);

	// Operators (const)
	bool		operator==(const Vec3& compare) const;				// Vec3 == Vec3
	bool		operator!=(const Vec3& compare) const;				// Vec3 != Vec3
	const Vec3	operator+(const Vec3& vecToAdd) const;				// Vec3 + Vec3
	const Vec3	operator-(const Vec3& vecToSubtract) const;			// Vec3 - Vec3
	const Vec3	operator-() const;									// -Vec3, i.e. "unary negation"
	const Vec3	operator*(float uniformScale) const;				// Vec3 * float
	const Vec3	operator*(const Vec3& vecToMultiply) const;			// Vec3 * Vec3
	const Vec3	operator/(float inverseScale) const;				// Vec3 / float

																	// Operators (self-mutating / non-const)
	void		operator+=(const Vec3& vecToAdd);					// Vec3 += Vec3
	void		operator-=(const Vec3& vecToSubtract);				// Vec3 -= Vec3
	void		operator*=(const float uniformScale);				// Vec3 *= float
	void		operator/=(const float uniformDivisor);				// Vec3 /= float
	void		operator=(const Vec3& copyFrom);					// Vec3 = Vec3
																				// Standalone "friend" functions that are conceptually, but not actually, part of Vec3::
	friend const Vec3 operator*(float uniformScale, const Vec3& vecToScale);	// float * Vec3
};

