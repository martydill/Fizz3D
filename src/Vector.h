
// Vector.h
// Definition of the vector structures
// (c) 2002 Marty Dill


#ifndef VECTOR_H
#define VECTOR_H


// Vector in 3d space
struct Vec3
{
public:
	Vec3();
	Vec3(float X, float Y, float Z);
	void normalize(void);
	bool isOpposite(const Vec3& other);
	float x, y, z;
};


// Vector in 2d space
struct Vec2
{
public:
	Vec2();
	Vec2(float x, float y);
	void normalize(void);

	float x, y;
};


// Overloaded + operator for vector addition
Vec3 operator + (Vec3& left, Vec3& right);


// Overloaded operator += for vector addition
Vec3 operator += (Vec3& left, Vec3& right);


// Overloaded *= operator for scalar multiplication
Vec3 operator *= (Vec3& left, float right);


// Creates a vector based on the specified azimuth and elevation
Vec3 pointFromSphere(float azimuth, float elevation);



#endif
