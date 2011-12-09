
// Engine.h
// Definition of our engine class
// (c) 2002 Marty Dill


#ifndef ENGINE_H
#define ENGINE_H



#include "Fizz3d.h"
#include "Renderer.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Audio.h"
#include "Entity.h"
#include "Camera.h"
#include "Q3Map.h"
#include "Window.h"
#include "GfxDriver.h"
#include "TextureFactory.h"
#include "Console.h"


using namespace std;


class CEngine
{
public:

	CEngine();
	CEngine(char* cmdLineArgs);
	virtual ~CEngine();

	static int CriticalError(string message);

	int ResizeWindow(int width, int height);
	int InitEngine(void);
	int Destroy(void);
	int MainLoop(void);
	int HandleTime(void);
	int GameCycle(void);
	
	int LoadMap(string fileName);

	int ProcessMouse(void);
	int ProcessKeyboard(void);

	int ProcessConsoleCommand();
	
private:

	// Command-line arguments
	char* args;
	
	// Our list of game entities
	vector< Entity* > entityList;
	
	// The entity which we are currently controlling
	Entity* controlledEntity;

	// Rendering device
	IRenderer* pRenderer;

	// Mouse device and associated data
	IMouse* pMouse;
	MouseData mouseData;

	// Keyboard device and associated data
	IKeyboard* pKeyboard;
	KeyboardData keyData;

	// Sound device
	IAudio* pAudio;

	// Camera
	Camera* pCamera;

	// Map
	Q3Map* pMap;

	// Window
	IWindow* pWindow;

	// Rendering device
	IGfxDriver* pGfxDriver;

	// Graphics driver's texture factory
	ITextureFactory* pTextureFactory;

	std::auto_ptr<Console> _console;

	// Used for timekeeping
	#ifdef _WIN32
	LARGE_INTEGER currentTime, lastTime, frequency;
	#endif
};



#endif
