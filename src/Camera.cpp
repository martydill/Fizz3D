
// Camera.cpp
// Implementation of the camera class
// (c) 2002 Marty Dill

#include "Fizz3d.h"
#include "Camera.h"


// Initialization of static variables
int Camera::Free = 1;
int Camera::FirstPerson = 2;
int Camera::ThirdPerson = 4;


// Standard constructor
Camera::Camera() :
	entity(NULL)
{
	up.x = 0;
	up.y = 0;
	up.z = 1;
	name = "Camera";
}


// Constructor, specifying position
Camera::Camera(Vec3 position) :
	entity(NULL)
{
	this->position = position;
	up.x = 0;
	up.y = 0;
	up.z = 1;
	name = "Camera";
}


// Destructor
Camera::~Camera()
{

}


// Attaches the camera to the requested entity, using the requested view mode
int Camera::attachToEntity(Entity* e, int viewMode)
{
	Assert(e != NULL);
	Assert(viewMode == FirstPerson || viewMode == ThirdPerson);
	
	this->entity = e;
	this->viewMode = viewMode;
	this->up.x = 0;
	this->up.y = 0;
	this->up.z = 1;
	entity->enableFirstPersonCamera();

	return 0;
}


// Detaches the camera from an entity
int Camera::detach(void)
{
	// If we are attached, detach
	if(entity != NULL)
	{
		entity->disableFirstPersonCamera();
		entity = NULL;
		viewMode = Free;
	}

	return 0;
}


// Updates the camera's position based on the entity it is currently tracking
int Camera::update(void)
{
	// If we are tracking an entity ...
	if(entity != NULL)
	{
		if(viewMode == FirstPerson)
		{
			position = entity->getPosition();
			elevation = entity->getElevation();
			azimuth = entity->getAzimuth();
		}
		else if(viewMode == ThirdPerson)
		{
			// FIXME: Implement third person camera
		}
	}

	return 0;
}
