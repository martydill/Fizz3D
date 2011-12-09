
// TextureFactory.h
// Definition of the texture factory interface
// (c) 2002 Marty Dill


#ifndef TEXTUREFACTORY_H
#define TEXTUREFACTORY_H

#include "Texture.h"
#include "string"



class ITextureFactory
{
public:

	// Loads a texture from a file and returns a corresponding id
	virtual TextureID loadTextureFromFile(std::string fileName) = 0;

	// Builds a light map from the specified data
	virtual TextureID buildLightMap(const unsigned char* data) = 0;

	// Applys the specified texture
	virtual void applyTexture(TextureID id, int textureUnit) = 0;

	// Destroys the specified texture
	virtual void destroyTexture(TextureID id) = 0;

	// Frees all of the lightmaps
	virtual void freeLightMaps(void) = 0;

	// Dumps info on all loaded texture to the log
	virtual void dumpTextureInfo(void) = 0;
};



#endif
