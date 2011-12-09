
// GLDriver.h
// Defines the interface for our OpenGL driver
// (c) 2002 Marty Dill


#ifndef GLDRIVER_GLX_H
#define GLDRIVER_GLX_H


#include "GfxDriver.h"
#include "GLDriver_TextureFactory.h"
#include "Fizz3d.h"


// OpenGL graphics driver class
class GLDriver : public IGfxDriver
{
public:

	GLDriver();
	~GLDriver();
	
	// Public IGfxDriver interface
	int initialize(IWindow* pWindow);
	int startFrame(void);
	int endFrame(void);
	int resize(int newWidth, int newHeight);
	int applyCamera(Camera* pCamera);
	int drawGeometryList(GeometryList* l);
	ITextureFactory* createTextureFactory(void);
	int drawText(int xPos, int yPos, std::string msg);
	int postProcessMap(Q3Map* mapData);

private:

	// Curved surface display list generation
	int computePatch(int index);
	int genDisplayList(int index);

	// OpenGL driver info
	std::string vendor;
	std::string renderer;
	std::string version;
	std::string extensions;

	// Function pointers for multitexturing extensions
	GLTextureFactory* textureFactory;
};


#endif
