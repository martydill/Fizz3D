
// DX8Keyboard.cpp
// Implementation of our DirectX 8 keyboard handling code
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "DX8Keyboard.h"
#include "DirectX8.h"
#include "Engine.h"
#include "Log.h"


static const char* moduleName = "DX8Keyboard";


// Constructor
DX8Keyboard::DX8Keyboard() :
	device(NULL), acquired(false)
{
	memset(lastKeys, 0, 256);
}


// Destructor
DX8Keyboard::~DX8Keyboard()
{
}


// Sets up the keyboard device
int DX8Keyboard::initialize(void* initParameter)
{
	Trace("Creating keyboard device");
	DX8::CreateInputDevice(GUID_SysKeyboard, &device);

	HRESULT hr;
	
	Trace("Setting keyboard data format");
	hr = device->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
		CEngine::CriticalError("Unable to set keyboard data format!!");
	
	Trace("Setting keyboard cooperative level");
	hr = device->SetCooperativeLevel(NULL, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(hr))
		CEngine::CriticalError("Unable to set keyboard cooperative level!!");

	return 0;
}


// Updates keyboard position
int DX8Keyboard::update(KeyboardData* data)
{
	Assert(data != NULL);

	if(!acquired)
	{
		HRESULT hr = device->Acquire();
		if(FAILED(hr))
			return 1;

		acquired = true;
	}

	//memcpy(lastKeys, keys, 256);

	HRESULT hr = device->GetDeviceState(sizeof(unsigned char) * 256, (LPVOID)&keys);
	if(FAILED(hr))
	{
		acquired = false;
		return 1;
	}

	memcpy(data->keys, keys, 256);

	for(int i = 0; i < 256; ++i)
	{
		if(keys[i] != lastKeys[i])
		{
			data->changedKeys[i] = 1;
		}
		else
		{
			data->changedKeys[i] = 0;
		}
	}
	
	memcpy(lastKeys, keys, 256);

	return 0;
}


// Destroys/releases the keyboard device
int DX8Keyboard::destroy(void)
{
	if(device != NULL)
	{
		Trace("Releasing keyboard device");
		device->Unacquire();
		device->Release();
		device = NULL;
	}

	return 0;
}


char DX8Keyboard::getCharacterForKey(int scanCode)
{
	static HKL layout = GetKeyboardLayout(0);
	static UCHAR keyboardState[256];
	if (GetKeyboardState(keyboardState) == false)
		return 0;

	//translate keyboard press scan code identifier to a char
	UINT vk = MapVirtualKeyEx(scanCode, 1, layout);
	USHORT asciiValue;
	ToAscii(vk, scanCode, keyboardState, &asciiValue, 0);

	return static_cast<char>(asciiValue);

}