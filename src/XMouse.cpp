
// XMouse.cpp
// Implementation of our X-Window mouse handling code
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "XMouse.h"
#include "Engine.h"
#include "Log.h"
#include "glxWindow.h"


static const char* moduleName = "XMouse";


// Constructor
XMouse::XMouse()
{
}


// Destructor
XMouse::~XMouse()
{
}


static int lastX, lastY;

// Sets up the mouse device
int XMouse::initialize(void* initParameter)
{
	// This -must- be the HWND of the main window ...
	Assert(initParameter != NULL);
	Trace("Initializing X-Window mouse");

	pDisplay = static_cast< Display* > (initParameter);
	lastX = 0;
	lastY = 0;

	return 0;
}

extern Window xWindow;
// Updates mouse position
int XMouse::update(MouseData* data)
{
	Assert(data != NULL);

	XEvent event;
	Window root_return, child_return;
    int root_x_return, root_y_return;
    int win_x_return, win_y_return;
    unsigned int mask_return;

     XQueryPointer(	pDisplay,
	 						xWindow,
							&root_return,
      						&child_return,
							&root_x_return,
							&root_y_return,
      						&win_x_return,
							&win_y_return,
							&mask_return);

	data->dx = root_x_return - lastX;
	data->dy  = root_y_return - lastY;
	lastX = root_x_return;
	lastY = root_y_return;
	
/*
	while(XCheckMaskEvent(pDisplay, PointerMotionMask, &event) == True)
	{
		data->dx = event.xmotion.x_root - lastX;
		data->dy = event.xmotion.y_root - lastY;
		lastX = event.xmotion.x_root;
		lastY = event.xmotion.y_root;
	}
*/
		/*switch (event.type)
		{
			case ButtonPress:
				break;

			case ButtonRelease:
				break;
		}*/

	return 0;
}


// Destroys/releases the mouse device
int XMouse::destroy(void)
{
	Trace("Shutting down X-Window mouse");
	return 0;
}
