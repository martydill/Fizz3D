
// Entity.h
// Definition of our entity class
// (c) 2002 Marty Dill

#ifndef ENTITY_H
#define ENTITY_H


#include "Vector.h"
#include "Q3Structs.h"
#include "ModelManager.h"

enum EntityType
{
	Other,
	WorldSpawn,
	PlasmaGun,
	RocketLauncher,
	Shotgun,
	BFG,
	Railgun,
	GrenadeLauncher,
	MachineGun,
	LightningGun,
	RedArmor,
	YellowArmor,
	Ammo_Bullets,
	Ammo_Shells,
	Ammo_Cells,
	Item_Armor_Shard,
	Item_Health,
	Item_Health_Large
};

class Entity
{
public:

	enum EntityProperties
	{
		DieOnCollision = 0x01,
		LimitedLifespan = 0x02,
		HasModel = 0x04
	};

	enum EntityUpdateResults
	{
		Ok = 0,
		Dead = 1,
		DeadFromCollision = 2,
		DeadFromExpiry = 3
	};

	Entity();
	Entity(std::string name, Vec3 position, Vec3 velocity, BoundingBox bBox, unsigned int flags);
	Entity(std::string name, Vec3 position, Vec3 velocity, BoundingSphere bSphere, unsigned int flags);
	~Entity();

	int updatePosition(float timeSlice);
	Vec3 getPosition(void);
	void setPosition(Vec3 newPosition);

	BoundingBox getBoundingBox(void);
	BoundingSphere getBoundingSphere(void);

	void setBoundingBox(BoundingBox bBox);
	void setBoundingSpehre(BoundingSphere bSphere);

	int move(Vec3 offset);

	void setElevation(float newElevation);
	void setAzimuth(float newAzimuth);

	float getElevation(void);
	float getAzimuth(void);

	int loadModel(std::string fileName);
	int drawModel(void);

	void enableFirstPersonCamera(void);
	void disableFirstPersonCamera(void);
	bool getFirstPersonCamera();

	unsigned int getFlags(void);
	void setFlags(unsigned int newFlags);

	void setEntityType(EntityType type);
	EntityType getEntityType();

	std::string getName();
	void setName(std::string name);

	void setVisible(bool visible);

protected:

	bool visible;
	Vec3 position;
	Vec3 velocity;
	float elevation;
	float azimuth;
	std::string name;
//	IModel* pModel;
	ModelID model;
	bool firstPersonCamera;

	// Bitfield storing the properties of the entity
	unsigned int flags;

	// Number of milliseconds that the entity will live for
	unsigned int timeToLive;

	// Amount of time the entity has lived for
	unsigned int lifeTime;

	// What type of collision detection should be performed on this entity (box or sphere)
	CollisionType collisionType;

	// Stores the bounding box and bounding sphere for collision detection
	BoundingBox bBox;
	BoundingSphere bSphere;

	EntityType entityType;

private:
	void InitializeDefaults();
};


// Returns the entity's position
inline Vec3 Entity::getPosition(void)
{
	return position;
}


inline std::string Entity::getName(void)
{
	return name;
}

inline void Entity::setName(std::string name)
{
	this->name = name;
}

inline void Entity::setVisible(bool visible)
{
	this->visible = visible;
}


// Sets the entity's position
inline void Entity::setPosition(Vec3 newPosition)
{
	position = newPosition;
}


// Returns the entity's bounding box
inline BoundingBox Entity::getBoundingBox(void)
{
	return bBox;
}


// Returns the entity's bounding sphere
inline BoundingSphere Entity::getBoundingSphere(void)
{
	return bSphere;
}


// Sets the entity's elevation
inline void Entity::setElevation(float newElevation)
{
	elevation = newElevation;
}


// Sets the entity's azimuth
inline void Entity::setAzimuth(float newAzimuth)
{
	azimuth = newAzimuth;
}


// Gets the entity's elevation
inline float Entity::getElevation(void)
{
	return elevation;
}


// Gets the entity's azimuth
inline float Entity::getAzimuth(void)
{
	return azimuth;
}


// Enables the first person camera for this entity
inline void Entity::enableFirstPersonCamera(void)
{
	firstPersonCamera = true;
}


// Disables the first person camera for this entity
inline void Entity::disableFirstPersonCamera(void)
{
	firstPersonCamera = false;
}


inline bool Entity::getFirstPersonCamera()
{
	return firstPersonCamera;
}


// Returns the entity's flags
inline unsigned int Entity::getFlags(void)
{
	return flags;
}


// Sets the entity's flags
inline void Entity::setFlags(unsigned int newFlags)
{
	flags = newFlags;
}

inline void Entity::setEntityType(EntityType type)
{
	entityType = type;

	if(type == PlasmaGun)
		loadModel("models/weapons2/plasma/plasma");
	else if(type == RocketLauncher)
		loadModel("models/weapons2/rocketl/rocketl");
	else if(type == Shotgun)
		loadModel("models/weapons2/shotgun/shotgun");
	else if(type == RedArmor)
		loadModel("models/powerups/armor/armor_red");
	else if(type == Ammo_Bullets)
		loadModel("models/powerups/ammo/machinegunam");
	else if(type == Ammo_Shells)
		loadModel("models/powerups/ammo/shotgunam");
	else if(type == Ammo_Cells)
		loadModel("models/powerups/ammo/plasmaam");
	else if(type == Item_Armor_Shard)
		loadModel("models/powerups/armor/shard");
	else if(type == Item_Health)
		loadModel("models/powerups/health/medium_cross");
	else if(type == Item_Health_Large)
		loadModel("models/powerups/health/large_cross");
}

inline EntityType Entity::getEntityType()
{
	return entityType;
}

#endif
