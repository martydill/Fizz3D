
// GLDriver_Win32.h
// Defines the interface for our OpenGL driver
// (c) 2002 Marty Dill


#ifndef GLDRIVER_H
#define GLDRIVER_H


#include "GfxDriver.h"
#include "GLDriver_TextureFactory.h"
#include "Fizz3d.h"
#include "Q3Shader.h"

#define FONT_DISPLAY_LIST_SIZE		96

typedef void* (APIENTRY * PFNWGLALLOCATEMEMORYNVPROC) (GLsizei size, GLfloat readFrequency, GLfloat writeFrequency, GLfloat priority);
typedef void (APIENTRY * PFNWGLFREEMEMORYNVPROC) (void *pointer);
typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);


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
   int drawFog(GeometryList* l);
   int drawSky(GeometryList* l, float timeDiff);
   int drawFaces(GeometryList* l, float timeDiff);
   int drawMeshes(GeometryList* l, float timeDiff);
   int drawPatches(GeometryList* l);

	ITextureFactory* createTextureFactory(void);
	int drawText(int xPos, int yPos, std::string msg);
	int postProcessMap(Q3Map* mapData);

private:

	// Curved surface display list generation
	int computePatch(int index);
	int genDisplayList(int index);

	// Screen-related stuff
	#ifdef _WIN32
	GLuint pixelFormat;
	HDC hdc;
	HGLRC hrc;
	#endif
	
	// OpenGL driver info
	std::string vendor;
	std::string renderer;
	std::string version;
	std::string extensions;

	// Function pointers for multitexturing extensions
	#ifdef _WIN32
		PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
		PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
		PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;

		PFNGLLOCKARRAYSEXTPROC glLockArraysEXT;
		PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;

		PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;

		PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV;
		PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;

		PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
		PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;
	#endif
	GLTextureFactory* textureFactory;

	// Gamma data
	WORD oldGamma[256][3];
	WORD newGamma[256][3];

	// Font-related stuff
	#ifdef _WIN32
	HFONT font;
	#endif
	GLuint fontDispListBase;
};


#endif
