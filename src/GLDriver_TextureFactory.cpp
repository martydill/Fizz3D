
// GLDriver_TextureFactory.cpp
// Implementation of OpenGL texture factory. Uses DevIL for image loading.
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "GLDriver_TextureFactory.h"
#include "Config.h"
#include "Log.h"

static const char* moduleName = "TextureFactory";

using std::string;


// Standard constructor
GLTextureFactory::GLTextureFactory() :
numTextures(0)
{
}


// Standard destructor
GLTextureFactory::~GLTextureFactory()
{
}


// Loads a texture from a file and returns the corresponding id
TextureID GLTextureFactory::loadTextureFromFile(string fileName)
{
	ITexture* texture;
	TextureID i = 0;
	ILuint textureId;
	if(fileName == "")
		return i;

	//	Trace("Attempting to load texture %s ...", fileName.c_str());

	// If the texture has already been loaded, just return its id
	texture = textureList[fileName];
	if(texture != NULL)
	{
		//		Trace(" ... using cached texture");
		return texture->id;
	}

	/*///glPixelTransferf(GL_RED_SCALE, 1.9f);
	glPixelTransferf(GL_GREEN_SCALE, 1.9f);
	*///glPixelTransferf(GL_BLUE_SCALE, 1.9f);

	// Generate the texture space
	ilGenImages(1, &textureId);
	ilBindImage(textureId);

#if 1

	// Append .jpg to the file name
	string s = fileName;
	if(s.find(".tga") != 0 || s.find(".jpg") != 0)
		s = s.substr(0, s.length() - 4);

	s += ".jpg";

	// Attempt to load the texture as a .jpg
	if(ilLoadImage((char*)s.c_str()) != IL_FALSE)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		iluFlipImage(); // FI|XME - is this needed for all cards?
		i = ilutGLBindTexImage();
	}
	else
	{
		// That failed; try a .tga extension
		string t = s = s.substr(0, s.length() - 4);

		t += ".tga";

		// Load it ...
		if(ilLoadImage((char*)t.c_str()) != IL_FALSE)
		{
			// Good. It worked.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			iluFlipImage();
			i = ilutGLBindTexImage();
		}
		else
		{
			// FAILED
			Trace("failed: %s", fileName.c_str());
			return -1;
			// FIXME: add error checking code
		}
	}
#endif
	// Generate mipmaps
	if(Config::getBoolValue("UseMipmaps"))
		ilutGLBuildMipmaps();

	texture = new ITexture();
	texture->width = ilGetInteger(IL_IMAGE_WIDTH);
	texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
	texture->id = i;

	// Dispose of the image
	ilDeleteImages(1, &textureId);

	++numTextures;

	textureList[fileName] = texture;
	idToTextureMap[i] = texture;

	return i;
}


// Generates a lightmap based on the data passsed in
// FIXME: Gamma correction
TextureID GLTextureFactory::buildLightMap(const unsigned char* data)
{
	TextureID i = 0;
	GLuint lightMapTexture;

	/*	
	if(Config::getIntValue("UseHardwareGamma") == 0)
	{
	glPixelTransferf(GL_RED_SCALE, 1.5f);
	glPixelTransferf(GL_GREEN_SCALE, 1.5f);
	glPixelTransferf(GL_BLUE_SCALE, 1.5f);
	}
	*/

	glGenTextures(1, &lightMapTexture);

	glBindTexture(GL_TEXTURE_2D, lightMapTexture);		

	// Set the texture's properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Create the texture
	if(Config::getBoolValue("UseMipmaps"))
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 128, 128, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//else
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	i = lightMapTexture;
	ITexture* texture = new ITexture();
	texture->id = i;
	texture->width = ilGetInteger(IL_IMAGE_WIDTH);
	texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
	lightMapList.push_back(texture);
	++numTextures;
	idToTextureMap[i] = texture;
	return i;
}


TextureID lastTexture[2];

// Applies the specified texture
void GLTextureFactory::applyTexture(TextureID id, int textureUnit)
{
	if(id == lastTexture[textureUnit])
		return;

	//ssert(id != -1);
	glBindTexture(GL_TEXTURE_2D, id);
	lastTexture[textureUnit] = id;

	return;
}


// Destroys the specified texture
void GLTextureFactory::destroyTexture(TextureID id)
{
	return;
}


// Frees all of the lightmaps
void GLTextureFactory::freeLightMaps(void)
{
	std::list< ITexture* >::iterator i;

	for(i = lightMapList.begin(); i != lightMapList.end(); ++i)
	{

		ITexture* texId = *i;
		glDeleteTextures(1, &texId->id);
	}

	lightMapList.clear();
	return;
}


// Dumps info on all of the loaded textures
void GLTextureFactory::dumpTextureInfo(void)
{
	std::map< std::string, ITexture* > ::iterator i;

	Trace("Dumping texture store:");
	for(i = textureList.begin(); i != textureList.end(); ++i)
	{
		std::string s = i->first;
		ITexture* texture = i->second;
		Trace("Texture - ID: %u  Name: %s", texture != NULL ? texture->id : 0, s.c_str());
	}

	return;
}


ITexture* GLTextureFactory::getTexture(TextureID id)
{
	return idToTextureMap[id];	
}