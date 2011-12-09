
// Platform_Linux.cpp
// The Linux version of the platform-specific support code
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "Config.h"
#include "Platform.h"


extern int EngineMain(void);



// Entry point
int main(int argc, char* argv[])
{
	EngineMain();
	return 0;
}



// Platform namespace contains all of our functions
namespace Platform
{

	// Static variables used for keeping track of time
	static long currentTime, lastTime;


	// Returns a graphics device for the current platform
	IGfxDriver* getGfxDevice(void)
	{
		// Since Linux only supports OpenGL, we have no choice here ...
		return new GLDriver();
	}
	

	// Returns a keyboard device for the current platform
	IKeyboard* getKeyboardDevice(void)
	{
		return new XKeyboard();
	}


	// Returns a mouse device for the current platform
	IMouse* getMouseDevice(void)
	{
		return new XMouse();
	}


	// Returns an audio device for the current platform
	IAudio* getAudioDevice(void)
	{
		// FIXME: add linux audio support
		return NULL;
	}


	// Returns a window for the current platform
	IWindow* getWindow(void)
	{
		return new glxWindow();
	}


	// Sets up the timer
	void timeInit(void)
	{
		timeval initialTime;
		gettimeofday(&initialTime, NULL) ;
		currentTime = initialTime.tv_sec * 1000 + (initialTime.tv_usec / 1000);
	}


	// Returns the difference, in milliseconds, since the previous call
	long getTimeDifference(void)
	{
		long timeDiff;
		timeval time;
		lastTime = currentTime;
		gettimeofday(&time, NULL);
		currentTime = time.tv_sec * 1000 + (time.tv_usec / 1000);
		timeDiff = currentTime - lastTime;
		return timeDiff;
	}

}
