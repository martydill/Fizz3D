
// DX8Mouse.cpp
// Implementation of our DirectX 8 mouse handling code
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "DX8Mouse.h"
#include "DirectX8.h"
#include "Engine.h"
#include "Log.h"


static const char* moduleName = "DX8Mouse";


// Constructor
DX8Mouse::DX8Mouse() :
	device(NULL), acquired(false)
{
}


// Destructor
DX8Mouse::~DX8Mouse()
{
}


// Sets up the mouse device
int DX8Mouse::initialize(void* initParameter)
{
	// This -must- be the HWND of the main window ...
	Assert(initParameter != NULL);
	
	HWND hwnd = reinterpret_cast< HWND > (initParameter);
	SetFocus(hwnd);

	Trace("Creating mouse device");
	DX8::CreateInputDevice(GUID_SysMouse, &device);

	HRESULT hr; 

	Trace("Setting mouse data format");
	hr = device->SetDataFormat(&c_dfDIMouse);
	if(FAILED(hr))
		CEngine::CriticalError("Unable to set mouse data format!!");
	
	Trace("Setting mouse cooperative level");
	hr = device->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(hr))
		CEngine::CriticalError("Unable to set mouse cooperative level!!");

	return 0;
}


// Updates mouse position
int DX8Mouse::update(MouseData* data)
{
	Assert(data != NULL);

	if(!acquired)
	{
		HRESULT hr = device->Acquire();
		if(FAILED(hr))
			return 1;

		acquired = true;
	}

	HRESULT hr = device->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&state);
	if(FAILED(hr))
	{
		acquired = false;
		return 1;
	}

	data->dx = state.lX;
	data->dy = state.lY;
	data->dz = state.lZ;
	data->leftButton = state.rgbButtons[0];
	data->rightButton = state.rgbButtons[1];
	data->middleButton = state.rgbButtons[2];

	return 0;
}


// Destroys/releases the mouse device
int DX8Mouse::destroy(void)
{
	if(device != NULL)
	{
		Trace("Releasing mouse device");
		device->Unacquire();
		device->Release();
		device = NULL;
		acquired = false;
	}

	return 0;
}
