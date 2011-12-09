
// XMouse.h
// Defines our X-Window mouse class
// (c) 2002 Marty Dill


#ifndef XMOUSE_H
#define XMOUSE_H


#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>
#include "Mouse.h"
#include "Fizz3d.h"


class XMouse : public IMouse
{
public:
	XMouse();
	virtual ~XMouse();

	int initialize(void* initParameter);
	int update(MouseData* data);
	int destroy(void);

private:
	Display* pDisplay;
};



#endif
