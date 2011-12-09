
// Platform_Win32.cpp
// The Windows version of the platform-specific support code
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "Config.h"
#include "Platform.h"


extern int EngineMain(char*);



// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLineArgs, int nCmdShow)
{
	EngineMain(cmdLineArgs);
	return 0;
}



// Platform namespace contains all of our functions
namespace Platform
{

	// Static variables used for timing
	static LARGE_INTEGER currentTime, lastTime, frequency;

	
	// Returns a graphics device for the current platform
	IGfxDriver* getGfxDevice(void)
	{
		// Use the rendering device listed in the config file
		if(Config::getStringValue("RenderingDevice") == "OpenGL")
			return new GLDriver();

		else if(Config::getStringValue("RenderingDevice") == "D3d")
			return new D3dDriver();

		else
			return NULL;
	}
	

	// Returns a keyboard device for the current platform
	IKeyboard* getKeyboardDevice(void)
	{
		return new DX8Keyboard();
	}


	// Returns a mouse device for the current platform
	IMouse* getMouseDevice(void)
	{
		return new DX8Mouse();
	}


	// Returns an audio device for the current platform
	IAudio* getAudioDevice(void)
	{
		return new DX8Audio();
	}


	// Returns a window for the current platform
	IWindow* getWindow(void)
	{
		return new Win32Window();
	}


	// Sets up the timer.
	void timeInit(void)
	{
		QueryPerformanceFrequency(&frequency);
	}


	// Returns the difference, in milliseconds, since the previous call
	long getTimeDifference(void)
	{
		long timeDiff;
		lastTime = currentTime;
		QueryPerformanceCounter(&currentTime);
		timeDiff = ((float)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart) * 1000;
		return timeDiff;
	}

}
