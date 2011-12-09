
// glxWindow.cpp
// Implementation of the GLX window class
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "glxWindow.h"
#include "Log.h"

static const char* moduleName = "GLXWindow";


Display* pDisplay;
int screen;
XF86VidModeModeInfo **vidModeInfo;
XVisualInfo* visualInfo;
XSetWindowAttributes windowAttributes;
GLXContext glContext;
Colormap colorMap;
Window xWindow;
int width, height;


// Standard constructor
glxWindow::glxWindow()
{

}


// Standard destructor
glxWindow::~glxWindow()
{

}

/* attributes for a double buffered visual in RGBA format with at least
 * 4 bits per color and a 16 bit depth buffer */
static int attrListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER,
    GLX_RED_SIZE, 4,
    GLX_GREEN_SIZE, 4,
    GLX_BLUE_SIZE, 4,
    GLX_DEPTH_SIZE, 16,
    None };


// Creates the window
int glxWindow::create(int width, int height, int bpp, int fullScreen, void* data, void* mesageHandler)
{
	int numVideoModes;
	int selectedMode;

	pDisplay = XOpenDisplay(0);
	screen = DefaultScreen(pDisplay);

	Trace("Getting modelines");
	XF86VidModeGetAllModeLines(pDisplay, screen, &numVideoModes, &vidModeInfo);

	for(int i = 0; i < numVideoModes; ++i)
		if(vidModeInfo[i]->hdisplay == width && vidModeInfo[i]->vdisplay == height)
			selectedMode = i;
	
	Trace("Initializing GLX");
	visualInfo = glXChooseVisual(pDisplay, screen, attrListDbl);
	glContext = glXCreateContext(pDisplay, visualInfo, 0, GL_TRUE);
	
	colorMap = XCreateColormap(pDisplay, RootWindow(pDisplay, visualInfo->screen), visualInfo->visual, AllocNone);
	windowAttributes.event_mask = ExposureMask | KeyPressMask | ButtonPressMask| StructureNotifyMask | PointerMotionMask;

	Trace("Creating window");
	xWindow = XCreateWindow(
											pDisplay,
											RootWindow(pDisplay, visualInfo->screen),
											0,
											0,
											width,
											height,
											0,
											visualInfo->depth,
											InputOutput,
											visualInfo->visual,
											CWBorderPixel | CWColormap | CWEventMask,
											&windowAttributes
											);
											
	Atom wmDelete = XInternAtom(pDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(pDisplay, xWindow, &wmDelete, 1);
	XSetStandardProperties(pDisplay, xWindow, "Fizz3d", "Fizz3d", None, NULL, 0, NULL);
	XMapRaised(pDisplay, xWindow);
	
	glXMakeCurrent(pDisplay, xWindow, glContext);


/*


        GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
            StructureNotifyMask;
        GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
            0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
            CWBorderPixel | CWColormap | CWEventMask, &GLWin.attr);

        wmDelete = XInternAtom(GLWin.dpy, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(GLWin.dpy, GLWin.win, &wmDelete, 1);
        XSetStandardProperties(GLWin.dpy, GLWin.win, title,
            title, None, NULL, 0, NULL);
        XMapRaised(GLWin.dpy, GLWin.win);
    }

    glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
    XGetGeometry(GLWin.dpy, GLWin.win, &winDummy, &GLWin.x, &GLWin.y,
        &GLWin.width, &GLWin.height, &borderDummy, &GLWin.depth);
	
	*/

	::width = width;
	::height = height;
	this->width = width;
	this->height = height;
	
	Cursor pointer;

    char bm[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    Pixmap pix = XCreateBitmapFromData( pDisplay, xWindow, bm, 8, 8 );
    XColor black;
    memset( &black, 0, sizeof( XColor ) );
    black.flags = DoRed | DoGreen | DoBlue;
    pointer = XCreatePixmapCursor( pDisplay, pix, pix, &black, &black, 0, 0 );
    XFreePixmap( pDisplay, pix );

	XDefineCursor( pDisplay, xWindow, pointer );
	XSync( pDisplay, False );
	
	return 0;
}


// Resizes the window
int glxWindow::resize(int newWidth, int newHeight)
{
	return 0;
}


// Returns a pointer to the window handle
void* glxWindow::getWindowHandle(void)
{
	return pDisplay;
}

// Called to end the current frame
int glxWindow::endFrame(void)
{
	glXSwapBuffers(pDisplay, xWindow);
	return 0;
}


int WindowProcedure(CEngine* pEngine)
{
	//XEvent event;
/*
	Grab events from the queue
	while (XPending(pDisplay) > 0)
	{
		XNextEvent(pDisplay, &event);
		switch (event.type)
		{
			case Expose:
				if (event.xexpose.count != 0)
					break;
				//drawGLScene();
				break;
		case ConfigureNotify:

			if ((event.xconfigure.width !=width) ||
				(event.xconfigure.height != height))
			{
			width = event.xconfigure.width;
				height = event.xconfigure.height;
					printf("Resize event\n");
				//resizeGLScene(event.xconfigure.width,
				// event.xconfigure.height);
			}
			break;
		case KeyPress:
			if (XLookupKeysym(&event.xkey, 0) == XK_Escape)
			{
			// done = True;
			}

		case ClientMessage:
			if (*XGetAtomName(pDisplay, event.xclient.message_type) ==
				*"WM_PROTOCOLS")
			{
				printf("Exiting sanely...\n");
				//done = True;
			}
			break;
		default:
			break;
		}
	}
*/
	//glXSwapBuffers(pDisplay, xWindow);

	return 0;
}

