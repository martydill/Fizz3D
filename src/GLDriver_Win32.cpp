
// GLDriver_Win32.cpp
// Implementation of our OpenGL rendering device
// (c) 2002 Marty Dill

#include "Fizz3d.h"
#include "GLDriver_Win32.h"
#include "Log.h"
#include "Engine.h"
#include "Q3Structs.h"
#include "Config.h"

static const char* moduleName = "GLDriver";


// Standard constructor
GLDriver::GLDriver()
{
}

#if 0
unsigned char* mem;
#endif

// Standard destructor
GLDriver::~GLDriver()
{
	Trace("Shutting down");

	if(Config::getBoolValue("UseHardwareGamma"))
		SetDeviceGammaRamp(hdc, &oldGamma);
#if 0
	wglFreeMemoryNV(mem);
#endif
}


// Initialize our rendering device
int GLDriver::initialize(IWindow* pWindow)
{
	HWND hwnd;

	Trace("Initializing OpenGL graphics driver");

	hwnd = static_cast< HWND > (pWindow->getWindowHandle());
	
	// Create an OpenGL pixel format descriptor
	PIXELFORMATDESCRIPTOR pfd =
	{	
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,	// Format Must Support OpenGL and double buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		pWindow->getBpp(),							// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	

	// Initialise stuff, and check for errors
	
	Trace("Acquiring device context");
	if (!(hdc = GetDC(hwnd)))
		CEngine::CriticalError("Unable to create GL device context!");

	Trace("Finding pixel format");
	if (!(pixelFormat = ChoosePixelFormat(hdc, &pfd)))
		CEngine::CriticalError("Unable to find a pixel format!");
	
	Trace("Setting pixel format");
	if(!SetPixelFormat(hdc, pixelFormat, &pfd))
		CEngine::CriticalError("Unable to set the pixel format!");
	
	Trace("Acquiring rendering context");
	if(!(hrc = wglCreateContext(hdc)))
		CEngine::CriticalError("Unable to create GL rendering context!");
	
	Trace("Setting current GL window");
	if(!wglMakeCurrent(hdc, hrc))
		CEngine::CriticalError("Unable to activate GL rendering context!");
	
	resize(pWindow->getWidth(), pWindow->getHeight());

	// Set up initial gl parameters
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0,0.0,0.0,0.0);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	// Grab GL vendor information
	Trace("Getting OpenGL driver info ...");
	char* str;

	str = (char*)glGetString(GL_VENDOR);
	vendor.assign(str);
	Trace("OpenGL Vendor: %s", vendor.c_str());

	str = (char*)glGetString(GL_RENDERER);
	renderer.assign(str);
	Trace("OpenGL Renderer: %s", renderer.c_str());

	str = (char*)glGetString(GL_VERSION);
	version.assign(str);
	Trace("OpenGL Version: %s", version.c_str());

	str = (char*)glGetString(GL_EXTENSIONS);
	extensions.assign(str);
	
	// Replace all spaces with newlines
	for(int i = 0; i < extensions.length(); ++i)
	{
		if(extensions[i] == ' ')
			extensions[i] = '\n';
	}


	// Acquire pointer to extension functions

	// Multitexturing
	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
	glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
	
	// CVAs
	glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC)wglGetProcAddress("glLockArraysEXT");
	glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC)wglGetProcAddress("glUnlockArraysEXT");

	glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC) wglGetProcAddress("glFogCoordfEXT");

#if 0
	glFlushVertexArrayRangeNV = (PFNGLFLUSHVERTEXARRAYRANGENVPROC)wglGetProcAddress("glFlushVertexArrayRangeNV");
	glVertexArrayRangeNV = (PFNGLVERTEXARRAYRANGENVPROC)wglGetProcAddress("glVertexArrayRangeNV");
	wglAllocateMemoryNV = (PFNWGLALLOCATEMEMORYNVPROC)wglGetProcAddress("wglAllocateMemoryNV");
    wglFreeMemoryNV = (PFNWGLFREEMEMORYNVPROC)wglGetProcAddress("wglFreeMemoryNV");

	mem = NULL;
	mem = (unsigned char*)wglAllocateMemoryNV(1024 * 1024 * 5, 0.2f, 0.0f, 0.7f);
	
	if(!mem)
		CEngine::CriticalError("Can't allocate memory");
	
	glVertexArrayRangeNV(1024 * 1024 * 5, mem);
	glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
#endif


	// Set up hardware gamma, if we are using it
	if(Config::getBoolValue("UseHardwareGamma"))
	{

		GetDeviceGammaRamp(hdc, &oldGamma);
		float gamma = 2.0f;

		for(int i = 0; i < 3; i++)
		{
			for(int j = 0;j < 256; j++)
			{
				DWORD d = oldGamma[j][i] * gamma;
				if(d > 65535)
					d = 65535;
				newGamma[j][i] = d;
			}
		}

		SetDeviceGammaRamp(hdc, &newGamma);
	}
	
	// Set up the font
	HFONT	oldfont;

	fontDispListBase = glGenLists(FONT_DISPLAY_LIST_SIZE);

	font = CreateFont(-12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, 
						CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Courier New");

	oldfont = (HFONT)SelectObject(hdc, font);
	wglUseFontBitmaps(hdc, 32, FONT_DISPLAY_LIST_SIZE, fontDispListBase);
	SelectObject(hdc, oldfont);
	DeleteObject(font);

	return 0;
}


// Called at the start of each frame
int GLDriver::startFrame(void)
{
	// Clear the depth buffer, but since we draw over the entire screen, don't clear the color buffer
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	
	glLoadIdentity();
	return 0;
}


// Called at the end of each frame
int GLDriver::endFrame(void)
{
	SwapBuffers(hdc);
	return 0;
}
