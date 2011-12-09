
// XKeyboard.cpp
// Implementation of our X-Window keyboard handling code
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "XKeyboard.h"
#include "Engine.h"
#include "Log.h"


static const char* moduleName = "XKeyboard";


// Constructor
XKeyboard::XKeyboard()
{
}


// Destructor
XKeyboard::~XKeyboard()
{
}


// Sets up the keyboard device
int XKeyboard::initialize(void* initParameter)
{
	// We need to get the handle to the window
	Assert(initParameter != NULL);
	pDisplay = static_cast< Display* > (initParameter);

	Trace("Initializing X-Window keyboard");
	
	return 0;
}


// Updates keyboard position
int XKeyboard::update(KeyboardData* data)
{
	Assert(data != NULL);

	XEvent event;
	memset(data->keys, 0, 255);

	while(XCheckMaskEvent(pDisplay, KeyPressMask, &event) == True)
	{
		int i = XLookupKeysym(&event.xkey, 0);
		data->keys[(i & 0x00FF)] = 1;
	}

	return 0;
}


// Destroys/releases the keyboard device
int XKeyboard::destroy(void)
{
	Trace("Shutting down X-Window keyboard");
	return 0;
}
