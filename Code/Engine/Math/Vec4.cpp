#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"

Vec4::Vec4(const Vec4& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}

Vec4::Vec4(float initialX, float initialY, float initialZ, float initialW)
	:x(initialX),
	y(initialY),
	z(initialZ),
	w(initialW)
{

}

Vec4::Vec4(float initialX, float initialY, float initialZ)
	: x(initialX),
	y(initialY),
	z(initialZ),
	w(0.0f)
{

}

float Vec4::GetLength() const
{
	return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
}

float Vec4::GetLengthXYZ() const
{
	return sqrtf((x * x) + (y * y) + (z * z));
}

float Vec4::GetLengthSquared() const
{
	return (x * x) + (y * y) + (z * z) + (w * w);
}

float Vec4::GetLengthXYZSquared() const
{
	return (x * x) + (y * y) + (z * z);
}

const Vec4 Vec4::operator+(const Vec4& vecToAdd) const
{
	return Vec4(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w);
}

const Vec4 Vec4::operator-(const Vec4& vecToSubtract) const
{
	return Vec4(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w);
}

const Vec4 Vec4::operator-() const
{
	return Vec4(-x, -y, -z, -w);
}

void Vec4::operator/=(const float uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
	w /= uniformDivisor;
}

void Vec4::operator=(const Vec4& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}

const Vec4 Vec4::operator*(float uniformScale) const
{
	return Vec4(x * uniformScale, y * uniformScale, z * uniformScale, w * uniformScale);
}

const Vec4 Vec4::operator*(const Vec4& vecToMultiply) const
{
	return Vec4(x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z, w * vecToMultiply.w);
}

const Vec4 Vec4::operator/(float inverseScale) const
{
	return Vec4(x / inverseScale, y / inverseScale, z / inverseScale, w / inverseScale);
}

void Vec4::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}

void Vec4::operator-=(const Vec4& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}

void Vec4::operator+=(const Vec4& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}

bool Vec4::operator!=(const Vec4& compare) const
{
	return((x != compare.x) || (y != compare.y) || (z != compare.z) || (w != compare.w));
}

bool Vec4::operator==(const Vec4& compare) const
{
	return((x == compare.x) && (y == compare.y) && (z == compare.z) && (w == compare.w));
}
