
// Fizz3d.h
// Main project include file; all other files should include this.
// All global includes and definitions should go here.
// This is a precompiled header.
// (c) 2002 Marty Dill


#ifndef FIZZ3D_H
#define FIZZ3D_H


// Windows specific stuff
#ifdef _WIN32
	#pragma warning(disable: 4786)
	#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#ifdef _LINUX
	#include <GL/glx.h>
#endif

// LibC includes
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

// STL includes
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include <list>
#include <algorithm>
#include <map>
#include <memory>


// OpenIL includes
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

// Define our own assert macro, just for the sake of neatness
#define Assert assert

#ifndef _WIN32
	typedef unsigned int DWORD;
	typedef unsigned short WORD;
	typedef unsigned char BYTE;
#endif

#endif
