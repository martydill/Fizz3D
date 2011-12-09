
// Engine.cpp
// The bulk of our game code goes here
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "Engine.h"
#include "MD3Model.h"
#include "Q3Map.h"
#include "Math.h"
#include "Config.h"
#include "Log.h"
#include "ShaderParser.h"
#include "Q3Renderer.h"
#include "Platform.h"
#include "ModelManager.h"


static const char* moduleName = "Engine";


// Static functions

// Displays an error message and quits
int CEngine::CriticalError(string message)
{
	Trace("*** Critical Error - %s", message.c_str());
	#ifdef _WIN32
		MessageBox(NULL, message.c_str(), "Critical error!", MB_OK | MB_ICONEXCLAMATION);
	#endif
	exit(1);

	return 0;
}


// Constructors and destructors

// Default constructor
CEngine::CEngine() :
	pRenderer(NULL),
	pMouse(NULL),
	pKeyboard(NULL),
	pAudio(NULL),
	pCamera(NULL),
	pMap(NULL),
	pWindow(NULL),
	pGfxDriver(NULL)
{

}


// Better constructor
CEngine::CEngine(char* cmdLineArgs) :
	pRenderer(NULL),
	pMouse(NULL),
	pKeyboard(NULL),
	pAudio(NULL),
	pCamera(NULL),
	pMap(NULL),
	pWindow(NULL),
	pGfxDriver(NULL)
{
	args = cmdLineArgs;
}


// CEngine destructor
CEngine::~CEngine()
{
	delete pRenderer;
	delete pKeyboard;
	delete pMouse;
	delete pAudio;
	delete pMap;
	delete pGfxDriver;
	delete pWindow;
}


// Member functions

// Sets up all game-related settings
int CEngine::InitEngine(void)
{
	Trace("Initializing engine ...");

	// Generate a random seed
	srand((unsigned int) time(0));

	// Read configuration data
	Trace("Reading configuration data");
	Config::readConfig("Config.cfg");

	// Create console
	_console.reset(new Console);

	// Create window
	Trace("Creating window");
	pWindow = Platform::getWindow();
	pWindow->create(
					Config::getIntValue("Width"),
					Config::getIntValue("Height"),
					Config::getIntValue("ColorDepth"),
					Config::getBoolValue("Fullscreen"),
					this,
					NULL//WndProc
					);

	// Create the graphics driver
	Trace("Creating graphics driver");
	pGfxDriver = Platform::getGfxDevice();
	pGfxDriver->initialize(pWindow);

	// Create the graphics driver's texture factory
	Trace("Creating texture factory.");
	pTextureFactory = pGfxDriver->createTextureFactory();

	MD3Model::textureFactory = pTextureFactory;

	// Create the rendering engine
	Trace("Creating renderering engine");
	pRenderer = new Q3Renderer();

	// Initialize mouse
	Trace("Initializing mouse");
	pMouse = Platform::getMouseDevice();
	pMouse->initialize(pWindow->getWindowHandle());

	// Initialize keyboard
	Trace("Initializing keyboard");
	pKeyboard = Platform::getKeyboardDevice();
	pKeyboard->initialize(pWindow->getWindowHandle());

	// Initialize audio
	if(Config::getBoolValue("UseAudio"))
	{
		Trace("Initializing audio");
		pAudio = Platform::getAudioDevice();
		pAudio->initialize();
	}
	else
	{
		Trace("Audio disabled.");
	}

	// Initialize DevIL
	Trace("Initializing DevIL");
	ilInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL); 

	Trace("Engine inititialization finished.");

	// Load shader data
	ShaderParser::loadShaders();
	
	// Load the map
	LoadMap("q3dm1.bsp");

	return 0;
}

int CEngine::LoadMap(string fileName)
{
	if(pMap == NULL)
		delete pMap;

    string path = "maps/";
    path = path.append(fileName);
	pMap = new Q3Map(path.c_str());

	pMap->readLightMaps(pTextureFactory);
	pMap->readTextures(pTextureFactory);
	
	pGfxDriver->postProcessMap(pMap);

	return 0;
}

// Shuts everything down
int CEngine::Destroy(void)
{
	Trace("Shutting down engine");
	
	// Free the shader data
	ShaderParser::freeShaders();

	if(pKeyboard != NULL)
		pKeyboard->destroy();

	if(pMouse != NULL)
		pMouse->destroy();

	return 0;
}


// Resize the window.
int CEngine::ResizeWindow(int width, int height)
{
	pWindow->resize(width, height);

	if(pGfxDriver != NULL)
		pGfxDriver->resize(width, height);

	return 0;
}


// FIXME: No, these shouldn't be global ...
float walkSpeed = 400.0f;
float mouseElevation = 360, mouseAngle = 90;
long ticksPerFrame = 25;
long timeDiff = 0;
unsigned long ticks;
Entity* player;
Entity* blah;


// Enter the program's main loop
int CEngine::MainLoop(void)
{
	bool done = false;
	Vec3 origin;

	for(std::vector<Entity*>::iterator i = pMap->entities.begin(); i < pMap->entities.end(); ++i)
	{
		Entity* e = *i;
		if(e->getEntityType() == EntityType::WorldSpawn)
		{
			origin = e->getPosition();
			origin.z += 50;
			mouseAngle = e->getAzimuth();
		}
	}

	BoundingBox b;
	b.max[0] = 50;
	b.max[1] = 50;
	b.max[2] = 50;
	b.min[0] = -50;
	b.min[1] = -50;
	b.min[2] = -50;

	Vec3 velocity;

	player = new Entity("Player", origin, velocity, b, Entity::HasModel);
	
	player->loadModel("models/players/bones");
	
	entityList = pMap->entities;
	entityList.push_back(player);

	pCamera = new Camera(origin);
	pCamera->attachToEntity(player, Camera::FirstPerson);
	entityList.push_back(pCamera);

	controlledEntity = player;

	// Set up time-related stuff
	Platform::timeInit();
	HandleTime();

	while(!done)
	{
		WindowProcedure(this);

		if(pWindow->getWindowHandle() == GetActiveWindow()) // fixme - use better method of 
		{		
			HandleTime();

			pMouse->update(&mouseData);
			ProcessMouse();

			pKeyboard->update(&keyData);
			ProcessKeyboard();

			if(keyData.keys[KEY_ESC])
				done = true;

			pGfxDriver->startFrame();
			GameCycle();
			pGfxDriver->endFrame();
			pWindow->endFrame();
		}
	}

	return 0;
}


// Process a mouse movement event
// FIXME: Optimize - cache mouse speed value
int CEngine::ProcessMouse(void)
{
	if(Config::getBoolValue("InvertMouse"))
		mouseElevation += mouseData.dy * Config::getIntValue("MouseSpeed") * MOUSE_SCALE_FACTOR;
	else
		mouseElevation -= mouseData.dy * Config::getIntValue("MouseSpeed") * MOUSE_SCALE_FACTOR;

	if(mouseElevation > MAX_MOUSE_ELEVATION)
		mouseElevation = MAX_MOUSE_ELEVATION;
	if(mouseElevation < MIN_MOUSE_ELEVATION)
		mouseElevation = MIN_MOUSE_ELEVATION;

	// Fire rocket
	if(mouseData.leftButton)
	{
		Entity* rocket;
		Vec3 origin = controlledEntity->getPosition();
		BoundingBox b;
		Vec3 velocity = pointFromSphere(controlledEntity->getAzimuth(), controlledEntity->getElevation());
		velocity.normalize();
		velocity *= 200;
		rocket = new Entity("Rocket", origin, velocity, b, Entity::HasModel | Entity::LimitedLifespan);
		rocket->setAzimuth(controlledEntity->getAzimuth());
		rocket->setElevation(controlledEntity->getElevation());
		rocket->loadModel("models/ammo/rocket/rocket");
		entityList.push_back(rocket);
	}

	mouseAngle -= mouseData.dx * Config::getIntValue("MouseSpeed") * MOUSE_SCALE_FACTOR;

	controlledEntity->setAzimuth(mouseAngle);
	controlledEntity->setElevation(mouseElevation);

	return 0;
}


bool attached = true;

// Process keyboard data
// To handle movement, we calculate the angle of the movement, calculate the displacement vector,
// and add it to the original posiiton
int CEngine::ProcessKeyboard(void)
{
	Vec3 delta;
	Vec3 currentPos;
	float moveAngle = 0;
	float dist = 0;
	bool isMoving = true;

	if(!_console->isVisible())
	{
		// Swap between first and third person
		if(keyData.keys[KEY_SPACE])
		{
			if(attached == true)
			{
				pCamera->detach();
				attached = false;
			}
			else
			{
				pCamera->attachToEntity(player, Camera::FirstPerson);
				attached = true;
			}
		}

		moveAngle = controlledEntity->getAzimuth();
		
		if(keyData.keys[KEY_UP] && keyData.keys[KEY_RIGHT])
			moveAngle += 315;
		
		else if(keyData.keys[KEY_UP] && keyData.keys[KEY_LEFT])
			moveAngle += 45;
		else if(keyData.keys[KEY_DOWN] && keyData.keys[KEY_RIGHT])
			moveAngle += 235;
		else if(keyData.keys[KEY_DOWN] && keyData.keys[KEY_LEFT])
			moveAngle += 135;
		else if(keyData.keys[KEY_DOWN])
			moveAngle += 180;
		else if(keyData.keys[KEY_UP])
			moveAngle += 0;
		else if(keyData.keys[KEY_LEFT])
			moveAngle += 90;
		else if(keyData.keys[KEY_RIGHT])
			moveAngle += 270;
		else
			isMoving = false;
		
		if(keyData.keys[DIK_A])
		{
			delta.z += walkSpeed * ((float)timeDiff / 1000);
			isMoving = true;
		}
		
		else if(keyData.keys[DIK_Z])
		{
			delta.z -= walkSpeed * ((float)timeDiff / 1000);
			isMoving = true;
		}

		if(isMoving)
		{
			/*dist = ((float)timeDiff / 1000) * walkSpeed + 25;
			//dist = 25;
			currentPos = controlledEntity->getPosition();
			delta.x = dist * Math::cosine(moveAngle);
			delta.y = dist * Math::sine(moveAngle);
			*/
			Vec3 destination;// = currentPos + delta;
			
			// FIXME: Add collision detection again
			//if(pMap->isTraversable(&currentPos, &destination))
			//{
				dist = ((float)timeDiff / 1000) * walkSpeed;
				//dist = 25;
				currentPos = controlledEntity->getPosition();
				delta.x = dist * Math::cosine(moveAngle);
				delta.y = dist * Math::sine(moveAngle);
				
				destination = currentPos + delta;
				controlledEntity->setPosition(destination);
			//}
		}
	}
	else
	{
		if(this->keyData.keys[KEY_BACKSPACE] && this->keyData.changedKeys[KEY_BACKSPACE])
		{
			_console->subtractCharacter();
		}
		else if(this->keyData.keys[KEY_ENTER])
		{
			ProcessConsoleCommand();
		}
		else
		{
			for(int i = 0; i < 256; ++i)
			{
				if(this->keyData.changedKeys[i] && this->keyData.keys[i])
				{
					char c = pKeyboard->getCharacterForKey(i);
					_console->addCharacter(c);
				}
			}
		}
	}

	if(this->keyData.changedKeys[DIK_GRAVE])
	{
		if(this->keyData.keys[DIK_GRAVE])
			_console->setVisible(!_console->isVisible());
	}

	if(this->keyData.changedKeys[KEY_ESC] && _console->isVisible())
	{
		_console->setVisible(false);
	}

	return 0;
}


// The actual game code goes here
int CEngine::GameCycle(void)
{
	GeometryList* geometry;
	
	for(int i = 0; i < entityList.size(); ++i)
	{
		Entity* e = entityList.at(i);
		if(e->updatePosition(timeDiff) == Entity::Dead)
		{
			entityList.erase(entityList.begin() + i);
			--i;
		}
	}

	pCamera->update();
	pGfxDriver->applyCamera(pCamera);
	
	pRenderer->applyCamera(pCamera);
	geometry = pRenderer->renderMap(pMap, entityList);
	
	pGfxDriver->drawGeometryList(geometry);
	
	std::for_each(entityList.begin(), entityList.end(), std::mem_fun(&Entity::drawModel));

	// Draw framerate in corner
	if(Config::getBoolValue("ShowFramerate"))
	{
		char fps[12];
		sprintf(fps, "FPS: %3.1f", ((float)1000 / timeDiff));
		pGfxDriver->drawText(1, 1, fps);
	}

	// Draw crosshair
	if(Config::getBoolValue("DrawCrosshair"))
		pGfxDriver->drawText(Config::getIntValue("Width") / 2 - 2, Config::getIntValue("Height") / 2 - 2, "+");

	// Draw console
	if(_console->isVisible())
		pGfxDriver->drawText(1, 32, "> " + _console->getConsoleText());

	return 0;
}


// Handles framerate calculations
int CEngine::HandleTime(void)
{
	timeDiff = Platform::getTimeDifference();
	return 0;
}


int CEngine::ProcessConsoleCommand()
{
	string text = _console->getConsoleText();
	string command;
	int spaceIndex = text.find(" ");
	if(spaceIndex == string::npos)
		command = text;
	else
		command = text.substr(0, spaceIndex);

	if(command == "map")
	{
		LoadMap(text.substr(command.length() + 1, text.length() - command.length() - 1));
	}

	_console->clear();
	_console->setVisible(false);
	return 0;
}
