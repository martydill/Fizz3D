
// Texture.h
// Definition of the texture interface
// (c) 2002 Marty Dill

// All texture objects should be created by a texture factory


#ifndef TEXTURE_H
#define TEXTURE_H


typedef unsigned int TextureID;


class ITexture
{
public:
	int width;
	int height;
	TextureID id;
};



#endif
