
// DX8Audio.cpp
// Our DirectSound 8 audio handler
// (c) 2002 Marty Dill



#include "Fizz3d.h"
#include "DX8Audio.h"
#include "DirectX8.h"
#include "Engine.h"
#include "Log.h"


static const char* moduleName = "DX8Audio";


// Constructor
DX8Audio::DX8Audio() //:
	//performance(NULL), loader(NULL)
{
}


// Destructor
DX8Audio::~DX8Audio()
{
}


// Sets up DirectAudio
int DX8Audio::initialize(void)
{
	/*CoInitialize(NULL);

	Trace("Creating DirectMusicPerformance8 object");
	HRESULT hr = CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, 
									IID_IDirectMusicPerformance8, (void**)&performance);
	if(FAILED(hr))
		CEngine::CriticalError("Unable to create DirectMusicPerformance object!");

	
	Trace("Creating DirectMusicLoader8 object");
	hr = CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC,
								IID_IDirectMusicLoader8, (void**)&loader);
	if(FAILED(hr))
		CEngine::CriticalError("Unable to create DirectMusicLoader object!");


	Trace("Initializing audio device");
	hr = performance->InitAudio(NULL, NULL, NULL, DMUS_APATH_SHARED_STEREOPLUSREVERB,
							64, DMUS_AUDIOF_ALL, NULL);
	if(FAILED(hr))
		CEngine::CriticalError("Unable to initialize audio device!");*/

	return 0;
}


// Loads a single audio file
int DX8Audio::loadFile(std::string fileName)
{
	//WCHAR name[255];
	//MultiByteToWideChar(CP_ACP, 0, fileName.c_str(), -1, name, 255);
	//
	//HRESULT hr = loader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, name, (void**)&segment);
	//
	//segment->Download(performance);

	//if(FAILED(hr))
	//	CEngine::CriticalError("Unable to initialize audio device!");

	//performance->PlaySegmentEx(
 //       segment,	 // Segment to play.
 //       NULL,        // Used for songs; not implemented.
 //       NULL,        // For transitions. 
 //       0,           // Flags.
 //       0,           // Start time; 0 is immediate.
 //       NULL,        // Pointer that receives segment state.
 //       NULL,        // Object to stop.
 //       NULL         // Audiopath, if not default.
 //   );  

	return 0;
}


// Destroys/releases the audio device
int DX8Audio::destroy(void)
{
	/*CoUninitialize();
	if(performance != NULL)
	{
		performance->CloseDown();
		loader->Release();
		performance->Release();
	}*/

	return 0;
}
