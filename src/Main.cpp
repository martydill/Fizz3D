
// Main.cpp
// Front-end to the engine
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "Engine.h"


// Fizz3d Entry point.
// Creates an engine object, and goes from there ...
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLineArgs, int nCmdShow)
int EngineMain(char* cmdLineArgs)
{

	try
	{
		CEngine* engine = new CEngine(cmdLineArgs);
		engine->InitEngine();
		engine->MainLoop();
		engine->Destroy();
		delete engine;
	}
	catch(exception e)
	{
		//MessageBox(NULL, e.what(), "Critical error", MB_OK);
	}

	return 0;
}
