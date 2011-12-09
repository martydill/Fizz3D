
// Entity.cpp
// Class for managing game entities
// (c) 2002 Marty Dill
// FIXME: d3d!

#include "Fizz3d.h"
#include "Entity.h"
#include "MD3Model.h"
#include "Engine.h"



// Constructor
Entity::Entity()
{
	InitializeDefaults();
}


// Constructor; using bounding box collision
Entity::Entity(std::string entityName, Vec3 startPosition, Vec3 startVelocity, BoundingBox bBox, unsigned int flags) :
position(startPosition), velocity(startVelocity), name(entityName)
{
	this->bBox = bBox;
	InitializeDefaults();
	this->flags = flags;
	collisionType = BoxCollide;
}


// Constructor; using bounding sphere collision
Entity::Entity(std::string entityName, Vec3 startPosition, Vec3 startVelocity, BoundingSphere bSphere, unsigned int flags) :
	position(startPosition), velocity(startVelocity), name(entityName)
{
	this->bSphere = bSphere;
	InitializeDefaults();
	this->flags = flags;
	collisionType = SphereCollide;
}


void Entity::InitializeDefaults()
{
	this->lifeTime = 0;
	this->timeToLive = 1000;
	this->model = -1;
	this->firstPersonCamera = false;
	this->azimuth = 0;
	this->elevation = 0;
	this->flags = 0;
}

// Destructor
Entity::~Entity()
{
}


// Attempts to move the entity. Returns true if successful
int Entity::move(Vec3 offset)
{
	position += offset;
	return 0;
}


// Updates the entity's position based on the current velocity and the amount of time passed
int Entity::updatePosition(float timeSlice)
{
	Vec3 offset;
	
	if(flags & LimitedLifespan)
	{
		lifeTime += static_cast< unsigned int > (timeSlice);
		if(lifeTime > timeToLive)
			return 1;
	}

	offset.x = velocity.x * timeSlice / 1000;
	offset.y = velocity.y * timeSlice / 1000;
	offset.z = velocity.z * timeSlice / 1000;
	move(offset);

	return 0;
}


// Loads the requested model 
int Entity::loadModel(std::string fileName)
{
	ModelManager* pMM = ModelManager::getInstance();
	model = pMM->loadModel(fileName);
	
	flags |= Entity::HasModel;
	return 0;
}


// Draws the entity's model
int Entity::drawModel(void)
{
	// Only draw if there is something to draw, and it's not behind the camera
	// FIXME: visibility, d3d
	if(model == -1)
		return -1;

	if(!visible)
		return -1;

	IModel* pModel = ModelManager::getInstance()->getModel(model);

	if(pModel != NULL && !firstPersonCamera)
	{
		glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);
		
		// Translate viewport accordingly
		glTranslatef(position.x, position.y, position.z);
		glRotatef(azimuth, 0, 0, 1);
		glRotatef(elevation, 0, -1, 0);
		pModel->draw();
		glPopMatrix();
	}
	
	return 0;
}
