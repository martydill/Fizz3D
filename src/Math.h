
// Math.h
// Definitions of all math-related functions
// (c) 2002 Marty Dill

#ifndef MATH_H
#define MATH_H



#include "Vector.h"
#include "Fizz3d.h"


namespace Math
{
	float degreesToRadians(float deg);
	float radiansToDegrees(float rad);
	float sine(float angle);
	float cosine(float Angle);
	float bernsteinPoly(Vec3 vertex, Vec3 coefficient);

	const float Pi = 3.14159265f;
	const float PiOver180 = 0.01745329f;
	const float One80OverPi = 57.2957795f;
	const float Tolerance = 0.0001f;
}



#endif