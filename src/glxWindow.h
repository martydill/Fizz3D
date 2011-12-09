
// glxWindow.h
// Definition of our GLX window class
// (c) 2002 Marty Dill


#ifndef GLXWINDOW_H
#define GLXWINDOW_H


#include "Window.h"
#include "Fizz3d.h"
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>


class glxWindow : public IWindow
{
public:
	glxWindow();
	virtual ~glxWindow();
	int create(int width, int height, int colorDepth, int fullScreen, void* data, void* messageHandler);
	int resize(int newWidth, int newHeight);
	void* getWindowHandle(void);
	int endFrame(void);

private:

};


#endif
