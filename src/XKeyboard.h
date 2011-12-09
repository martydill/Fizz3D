
// XKeyboard.h
// Defines our X-Window keyboard class
// (c) 2002 Marty Dill


#ifndef XKeyboard_H
#define XKeyboard_H


#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>
#include "Keyboard.h"
#include "Fizz3d.h"


class XKeyboard : public IKeyboard
{
public:
	XKeyboard();
	virtual ~XKeyboard();

	int initialize(void* initData);
	int update(KeyboardData* data);
	int destroy(void);

private:
	Display* pDisplay;
};


#endif
