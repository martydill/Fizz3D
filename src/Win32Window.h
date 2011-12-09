
// Win32Window.h
// Definition of our Win32-specific window class

#ifndef WIN32WINDOW_H
#define WIN32WINDOW_H


#include "Window.h"


class Win32Window : public IWindow
{
public:
	Win32Window();
	virtual ~Win32Window();
	int create(int width, int height, int colorDepth, bool fullScreen, void* data, void* messageHandler);
	int resize(int newWidth, int newHeight);
	void* getWindowHandle(void);
	int endFrame(void);
	
private:
	HWND hwnd;
	HINSTANCE hInstance;
	WNDCLASS wc;
};



#endif
