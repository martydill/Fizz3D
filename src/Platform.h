
// Platform.h
// Contains per-platform definitions
// (c) 2002 Marty Dill

#ifndef PLATFORM_H
#define PLATFORM_H


#include "Audio.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GfxDriver.h"

#ifdef _WIN32
	#include "D3dDriver.h"
	#include "GLDriver_Win32.h"
	#include "DX8Mouse.h"
	#include "DX8Keyboard.h"
	#include "DX8Audio.h"
	#include "Win32Window.h"
#endif

#ifdef _LINUX
	#include "GLDriver_GLX.h"
	#include "glxWindow.h"
#endif



#ifdef _WIN32

	// Key codes
	#define KEY_UP		DIK_UP
	#define KEY_DOWN	DIK_DOWN
	#define KEY_LEFT	DIK_LEFT
	#define KEY_RIGHT	DIK_RIGHT
	#define KEY_SPACE	DIK_SPACE
	#define KEY_ESC		DIK_ESCAPE
	#define KEY_ENTER		DIK_RETURN
	#define KEY_BACKSPACE	DIK_BACKSPACE

	// Case-insensitive string comparison function
	#define	STRCMPI(s1, s2) strcmpi(s1, s2)

#endif


#ifdef _LINUX

	// Data types
	typedef unsigned int DWORD;
	typedef unsigned short WORD;

	// Case-insensitive string comparison function
	#define	STRCMPI(s1, s2) ststrcasecmp(s1, s2)

#endif



namespace Platform
{
	IGfxDriver* getGfxDevice(void);
	IAudio* getAudioDevice(void);
	IKeyboard* getKeyboardDevice(void);
	IMouse* getMouseDevice(void);
	IWindow* getWindow(void);
	void timeInit(void);
	long getTimeDifference(void);
}


#endif
