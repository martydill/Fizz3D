
// GLDriver_TextureFactory.h
// Implementation of OpenGL texture factory
// (c) 2002 Marty Dill


#ifndef GLDRIVER_TEXTUREFACTORY_H
#define GLDRIVER_TEXTUREFACTORY_H


#include "TextureFactory.h"


class GLTextureFactory : public ITextureFactory
{
public:
	GLTextureFactory();
	virtual ~GLTextureFactory();

	TextureID loadTextureFromFile(std::string fileName);
	TextureID buildLightMap(const unsigned char* data);
	void applyTexture(TextureID id, int textureUnit);
	void destroyTexture(TextureID id);
	void freeLightMaps(void);
	void dumpTextureInfo(void);
	ITexture* getTexture(TextureID id);

private:
	unsigned int numTextures;
	std::map< std::string, ITexture* > textureList;
	std::list< ITexture* > lightMapList;

	std::map<TextureID, ITexture*> idToTextureMap;
};



#endif
