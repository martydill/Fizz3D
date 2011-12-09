
// Vector2d.h
// Header file for templated two dimentional vector class

#ifndef VECTOR2D_INC
#define VECTOR2D_INC



template< class T >
struct Vector2d
{
	Vector2d();
	Vector2d(float x, float y);
	
	void normalize(void);
	void set(float x, float y);
	
	T x, y;
};



template< class T >
Vector2d::Vector2d() :
	x(0), y(0)
{
	
}


template< class T >
Vector2d::Vector2d(float x, float y)
{
	this->x = x;
	this->y = y;
}


#endif
