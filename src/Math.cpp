
// Math.cpp
// Implementation of our math functions
// (c) 2002 marty Dill

#include "Fizz3d.h"
#include "Math.h"


// Namespace where all of our math-related functions go
namespace Math
{
	// Convertes degrees to radians
	float degreesToRadians(float deg)
	{
		return deg * PiOver180;
	}


	// Convertes radiands to degrees
	float radiansToDegrees(float rad)
	{
		return rad * One80OverPi;
	}


	// Calculates a bernstein basis for the required coordinates, using the reqired coefficients
	float berensteinPoly(Vec3 vertex, Vec3 coordinates)
	{
		float result = 0;

		return result;
	}

	
	// Calculate the sin of an angle, in degrees
	float sine(float angle)
	{
		return sin(angle * PiOver180);
	}


	// Calculate the cosine of an angle, in degrees
	float cosine(float angle)
	{
		return cos(angle * PiOver180);
	}
}
