
// Vector.cpp
// Implementation of the 2 and 3 dimensional vector classes and associated operators
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "Vector.h"
#include "Math.h"


// Vector in 3d space


// No-value constructor
Vec3::Vec3() :
	x(0), y(0), z(0)
{
}


// Constructor, specifying the values
Vec3::Vec3(float X, float Y, float Z) :
	x(X), y(Y), z(Z)
{
}


// Creates a vector based on the specified azimuth and elevation 
Vec3 pointFromSphere(float azimuth, float elevation)
{
	Vec3 v;
	float c = Math::cosine(elevation);

	v.x = c * Math::cosine(azimuth);
	v.y = c * Math::sine(azimuth);
	v.z = Math::sine(elevation);

	return v;
}


// Overloaded + operator for vector addition
Vec3 operator + (Vec3& left, Vec3& right)
{
	Vec3 v;

	v.x = left.x + right.x;
	v.y = left.y + right.y;
	v.z = left.z + right.z;

	return v;
}


// Overloaded += operator for vector addition
Vec3 operator += (Vec3& left, Vec3& right)
{
	left.x += right.x;
	left.y += right.y;
	left.z += right.z;

	return left;
}


// Normalizes the vector
void Vec3::normalize(void)
{
	float inverseLength;

	inverseLength = x * x + y * y + z * z;
	inverseLength = 1 / sqrt(inverseLength);
	
	x *= inverseLength;
	y *= inverseLength;
	z *= inverseLength;
}


// Overloaded *= operator for scalar multiplication
Vec3 operator *= (Vec3& left, float right)
{
	left.x *= right;
	left.y *= right;
	left.z *= right;

	return left;
}


// If two values are equal and the other is the other sign
bool Vec3::isOpposite(const Vec3& other)
{
	if(abs(this->x) - abs(other.x) < Math::Tolerance
		&& abs(this->y) - abs(other.y) < Math::Tolerance 
		&& abs(this->z) - abs(other.z) < Math::Tolerance)
	{
		if(this->x < 0 && other.x > 0)
			return true;
		if(this->x > 0 && other.x < 0)
			return true;
		if(this->y < 0 && other.y > 0)
			return true;
		if(this->y > 0 && other.y < 0)
			return true;
		if(this->z < 0 && other.z > 0)
			return true;
		if(this->z > 0 && other.z < 0)
			return true;
	}

	return false;
}


// Vector in 2d space


// No-value constructor. Initialize to 0.
Vec2::Vec2() : 
	x(0), y(0)
{
}


// Value-specifying constructor
Vec2::Vec2(float X, float Y) :
	x(X), y(Y)
{
}
