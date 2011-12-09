
// DirectX8.h
// Defines functions needed to support DirectX
// (c) 2002 Marty Dill


#ifndef DIRECTX8_H
#define DIRECTX8_H



#define DIRECTINPUT_VERSION	0x0800

#include <dinput.h>
#include <d3d9.h>
#include <d3dx9.h>

namespace DX8
{
	int CreateInputDevice(REFGUID rguid, IDirectInputDevice8** directInputDevice);
}



#endif