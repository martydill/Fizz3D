
// D3dDriver_TextureFactory.h
// Definition of Direct3d texture factory class
// (c) 2002 Marty Dill


#ifndef D3DDRIVER_TEXTUREFACTORY_H
#define D3DDRIVER_TEXTUREFACTORY_H


#include "Directx8.h"
#include "TextureFactory.h"


class D3dTextureFactory : public ITextureFactory
{
public:
	D3dTextureFactory();
	D3dTextureFactory(IDirect3DDevice9* pDevice);
	~D3dTextureFactory();
	
	TextureID loadTextureFromFile(std::string fileName);
	TextureID buildLightMap(const unsigned char* data);
	void applyTexture(TextureID id, int textureUnit);
	void destroyTexture(TextureID id);
	void freeLightMaps(void);
	void dumpTextureInfo(void);

private:
	unsigned int numTextures;
	IDirect3DDevice9* pParentDevice;
	std::map< std::string, TextureID > textureList;
	std::map< TextureID, IDirect3DTexture9* > pointerList;
	// FIXME: add texture list
};



#endif
