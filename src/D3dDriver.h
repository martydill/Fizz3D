
// D3dDriver.h
// Defines the interface for our Direct3d 8 driver


#ifndef D3DDRIVER_H
#define D3DDRIVER_H


#include "GfxDriver.h"
#include "Directx8.h"
#include "Q3Structs.h"


class D3dDriver : public IGfxDriver
{
public:
	D3dDriver();
	~D3dDriver();
	int initialize(IWindow* pWindow);
	int startFrame(void);
	int endFrame(void);
	int resize(int newWidth, int newHeight);
	int applyCamera(Camera* pCamera);
	ITextureFactory* createTextureFactory(void);

	int drawGeometryList(GeometryList* l);
	int drawText(int xPos, int yPos, std::string msg);

	int postProcessMap(Q3Map* mapData);

private:

	ITextureFactory* textureFactory;

	IDirect3D9* pD3dObject;
	IDirect3DDevice9* pDevice;
	D3DDISPLAYMODE displayMode;
	D3DPRESENT_PARAMETERS parameters; 
};



#endif
