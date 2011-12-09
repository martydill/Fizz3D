
// Camera.h
// Defines the camera class
// (c) 2002 Marty Dill


#ifndef CAMERA_H
#define CAMERA_H


#include "Fizz3d.h"
#include "Vector.h"
#include "Entity.h"


class Camera : public Entity
{
public:
	Camera();
	Camera(Vec3 position);
	~Camera();
		
	void setUpVector(Vec3 newUpVector);
	void getUpVector(Vec3& upCopy);

	void getReferencePoint(Vec3& ref);

	int attachToEntity(Entity* e, int viewMode);
	int detach(void);
	int update(void);

	static int FirstPerson;
	static int ThirdPerson;
	static int Free;

private:
	Vec3 up;
	Entity* entity;
	int viewMode;
};


// Sets the camera's up vector
inline void Camera::setUpVector(Vec3 newUpVector)
{
	up = newUpVector;
	return;
}


// Gets the camera's reference point
inline void Camera::getReferencePoint(Vec3& ref)
{
	ref = pointFromSphere(azimuth, elevation);
	return;
}


// Returns the camera's up vector
inline void Camera::getUpVector(Vec3& upCopy)
{
	upCopy = up;
	return;
}


#endif
