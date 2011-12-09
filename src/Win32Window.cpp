
// Win32Window.cpp
// Implementation of our Win32 window class


#include "Fizz3d.h"
#include "Win32Window.h"
#include "Log.h"
#include "Engine.h"

static const char* moduleName = "Win32Window";
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam,LPARAM lParam);


// Standard constructor
Win32Window::Win32Window()
{
	
}


// Standard destructor
Win32Window::~Win32Window()
{

}


// Create our window
int Win32Window::create(int width, int height, int colorDepth, bool fullScreen, void* data, void* messageHandler)
{
	DWORD dwExStyle;
	DWORD dwStyle;
	DEVMODE dmScreenSettings;
	RECT WindowRect;	

	// Sanity check
	Assert(width > 0 && width <= 1600);
	Assert(height > 0 && height <= 1200);
	Assert(colorDepth == 8 || colorDepth == 16 || colorDepth == 24 || colorDepth == 32);

	// Set up the size of our window
	WindowRect.left = (long)0;
	WindowRect.right = (long)width;
	WindowRect.top = (long)0;
	WindowRect.bottom = (long)height;

	// Save copies of the properties
	this->width = width;
	this->height = height;
	this->bpp = colorDepth;
	this->fullScreen = fullScreen;

	// Set up the Window Class
	Trace("Creating and registering Window Class");

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;				// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 4;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "Fizz3dClass";

	// Register the class
	if(!RegisterClass(&wc))									
	{
		CEngine::CriticalError("Unable to register Window Class!");
	}

	// Start fullscreen mode
	if(fullScreen)
	{
		// Initialize our ScreenSettings structure
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = width;
		dmScreenSettings.dmPelsHeight = height;
		dmScreenSettings.dmBitsPerPel = bpp;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		Trace("Switching to fullscreen mode");

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			CEngine::CriticalError("Unable to start fullscreen!");
			
	}

	// Set a few window style flags
	if(fullScreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(FALSE);
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
		ShowCursor(FALSE);
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	Trace("Creating Window");

	// Create our window, finally ...
	hwnd = CreateWindowEx(
		dwExStyle,										// Extended Style For The Window
		"Fizz3dClass",									// Class Name
		"Fizz3D",										// Window Title
		dwStyle | WS_CLIPSIBLINGS |	WS_CLIPCHILDREN,	// Defined Window Style
		CW_USEDEFAULT, CW_USEDEFAULT,											// Window Position
		WindowRect.right - WindowRect.left,				// Calculate Window Width
		WindowRect.bottom - WindowRect.top,				// Calculate Window Height
		NULL,											// No Parent Window
		NULL,											// No Menu
		hInstance,										// Instance
		data
	);						
	
	// Make sure that it was created properly
	if(!hwnd)
		CEngine::CriticalError("Unable to create window!");
	
	// Display the window
	ShowWindow(hwnd, SW_SHOWNORMAL);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	resize(width, height);
	SetCursorPos(width / 2, height / 2);

	return 0;
}


// Resizes the window
int Win32Window::resize(int newWidth, int newHeight)
{
	Trace("Resizing window");

	// Check for programmer errors ...
	Assert(newWidth > 0 && newWidth <= 1600);
	Assert(newHeight > 0 && newHeight <= 1200);

	// Watch for divide by zero
	if(newHeight == 0)
		newHeight = 1;
	
	width = newWidth;
	height = newHeight;

	return 0;
}


// Returns the window's handle
void* Win32Window::getWindowHandle(void)
{
	return hwnd;
}


// Ends the current frame
int Win32Window::endFrame(void)
{
	return 0;
}


// Window callback procedure
// FIXME: move into Window class
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam,LPARAM lParam)
{
	//static CEngine* pEngine = (CEngine*)GetWindowLong(hWnd, GWL_USERDATA);

	/*switch(msg)
	{
		case WM_CREATE:
		{
			HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
			pEngine = (CEngine *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)pEngine);
			break;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);	
			return 0;
		}

		case WM_SIZE:
		{
			pEngine->ResizeWindow(LOWORD(lParam), HIWORD(lParam));
			return 0;
		}
	}
*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


int WindowProcedure(CEngine* pEngine)
{
	MSG msg;

	GetMessage(&msg, NULL, 0, 0);

	switch(msg.message)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);	
			return 0;
		}

		case WM_SIZE:
		{
			pEngine->ResizeWindow(LOWORD(msg.lParam), HIWORD(msg.lParam));
			return 0;
		}
		case WM_QUIT:
		{
			exit(1);
		}
	}

	return 0;
}
