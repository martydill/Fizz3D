
// D3dDriver_TextureFactory.cpp
// Implementation of Direct3d texture factory class
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "D3dDriver_TextureFactory.h"
#include "Directx8.h"
#include "Log.h"


static const char* moduleName = "TextureFactory";


// Standard constructor
D3dTextureFactory::D3dTextureFactory() :
	numTextures(0)
{

}


// Pass in pointer to D3D object
D3dTextureFactory::D3dTextureFactory(IDirect3DDevice9* pDevice) :
	pParentDevice(pDevice), numTextures(0)
{
	Trace("Direct3D Texture factory created.");
}


// Standard destructor
D3dTextureFactory::~D3dTextureFactory()
{

}


// Loads a texture from a file and returns the corresponding id
TextureID D3dTextureFactory::loadTextureFromFile(std::string fileName)
{
	TextureID i = 0;
	IDirect3DTexture9* pTexture = NULL;
	HRESULT hr;

	// If the texture has already been loaded, just return its id
	i = textureList[fileName];
	if(i != 0)
	{
		Trace("Match: %s", fileName.c_str());
		return i;
	}

	//char name[256];
//	strcpy(name, fileName.c_str());
	std::string name = fileName + ".jpg";

	hr = D3DXCreateTextureFromFile(pParentDevice, (char*)name.c_str(), &pTexture);
	if(FAILED(hr))
	{
		name = fileName + ".tga";
		hr = D3DXCreateTextureFromFile(pParentDevice, (char*)name.c_str(), &pTexture);
		if(FAILED(hr))
			Trace("Unable to load texture: %s,   Result: %u", fileName.c_str(), hr);
	}

	textureList[fileName] = numTextures;
	pointerList[numTextures] = pTexture;
	++numTextures;

	pParentDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pParentDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	return (numTextures - 1);
}


// Builds a lightmap based on the data passed in
TextureID D3dTextureFactory::buildLightMap(const unsigned char* data)
{
	TextureID i = 0;

	return i;
}


// Applies the specified texture
void D3dTextureFactory::applyTexture(TextureID id, int textureUnit)
{
	pParentDevice->SetTexture(0, pointerList[id]);
	return;
}


// Destroys the specified texture
void D3dTextureFactory::destroyTexture(TextureID id)
{
	return;
}


// Frees all of the light maps
void D3dTextureFactory::freeLightMaps(void)
{
	return;
}


// Dumps info on all of the loaded textures
void D3dTextureFactory::dumpTextureInfo(void)
{
	return;
}
