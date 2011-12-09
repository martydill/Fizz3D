
// Window.h
// Definition of our Window interface
// (c) 2002 Marty Dill

#ifndef WINDOW_H
#define WINDOW_H


class CEngine;
int WindowProcedure(CEngine* pEngine);


class IWindow
{
public:
	virtual int create(int width, int height, int colorDepth, bool fullScreen, void* data, void* messageHandler) = 0;
	virtual int resize(int newWidth, int newHeight) = 0;
	int getWidth(void) { return width; }
	int getHeight(void) { return height; }
	int getBpp(void) { return bpp; }
	bool isFullscreen(void) { return fullScreen; }
	virtual void* getWindowHandle(void) = 0;
	virtual int endFrame(void) = 0;
	
protected:
	int width, height;
	int bpp;
	bool fullScreen;
};



#endif
