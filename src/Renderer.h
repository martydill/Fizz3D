
// Renderer.h
// Definition of the Renderer interface
// (c) 2002 Marty Dill


// Essentially, the renderer is responsible for deciding what to draw. It performs BSP Tree traversal,
// PVS, frustum culling, and whatever else is necessary to create a list of geometric data.
// This data is then passed to the graphics driver (IGfxDriver) to be displayed


#ifndef RENDERER_H
#define RENDERER_H


#include "Fizz3d.h"
#include "Map.h"
#include "Vector.h"
#include "Camera.h"
#include "Q3Structs.h"



// Our renderer interface
class IRenderer
{
public:
	virtual GeometryList* renderMap(Map* pMap, std::vector<Entity*>& entityList) = 0;
	virtual int applyCamera(Camera* pCamera) = 0;

protected:
	Camera* pCamera;
};



#endif
