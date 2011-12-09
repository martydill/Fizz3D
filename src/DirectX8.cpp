
// DirectX8.cpp
// DirectX 8 helper functions
// (c) 2002 Marty Dill

#include "Fizz3d.h"
#include "DirectX8.h"
#include "Log.h"

static const char* moduleName = "DX8";


namespace DX8
{
	// Our primary DI8 device
	IDirectInput8* di;
	

	// Flag to indicate whether the device has already been created
	bool isCreated = false;


	// Creates the requested device (keyboard/mouse)
	int CreateInputDevice(REFGUID rguid, IDirectInputDevice8** directInputDevice)
	{
		if(isCreated == false)
		{
			Trace("Initializing DirectInput8");
			
			HRESULT hr;
			hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&di, NULL);
			
			if(FAILED(hr))
				return 1;
			
			isCreated = true;
		}

		HRESULT hr;

		hr = di->CreateDevice(rguid, directInputDevice, NULL);
		if(FAILED(hr))
			return 1;
			
		return 0;
	}
}