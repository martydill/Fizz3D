
// GfxDriver.h
// Defines the interface for our graphics drivers
// (c) 2002 Marty Dill

// Graphics drivers are responsible for transferring geometry data to the screen. They don't do
// any kind of visibility determination - they simply take the polygons given to them, and blast
// them onto the screen as fast as possible. The rendering engine is responsible for figuring out
// what to draw.


#ifndef GFXDRIVER_H
#define GFXDRIVER_H



#include "Window.h"
#include "Camera.h"
#include "Q3Structs.h"
#include "TextureFactory.h"


// Graphics driver interface
class IGfxDriver
{
public:

	// Virtual destructor
	virtual ~IGfxDriver() {};

	// Initializes the graphics driver
	virtual int initialize(IWindow* pWindow) = 0;

	// Called at the start of each frame - e.g. it should clear the back buffer
	virtual int startFrame(void) = 0;

	// Called at the end of each frame - e.g. it should flip buffers
	virtual int endFrame(void) = 0;

	// Called when the window gets resized
	virtual int resize(int newWidth, int newHeight) = 0;

	// Called before drawing to set up the camera
	virtual int applyCamera(Camera* pCamera) = 0;

	// Draws the requested list of faces
	virtual int drawGeometryList(GeometryList* l) = 0;

	// Creates and returns a texture manager object
	virtual ITextureFactory* createTextureFactory(void) = 0;

	// Draws a text string to the screen
	virtual int drawText(int xPos, int yPos, std::string msg) = 0;
	
	// Perform any necessary post-prcessing on the map
	virtual int postProcessMap(Q3Map* mapData) = 0;

};



#endif
