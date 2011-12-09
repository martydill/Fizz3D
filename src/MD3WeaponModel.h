
#ifndef MD3WEAPONMODEL_H
#define MD3WEAPONMODEL_H

#include "Fizz3d.h"


// Class specifically for .MD3 weapon models
class MD3WeaponModel : public MD3Model
{

public:

	// Constructor
	MD3WeaponModel() :
		weapon(NULL), hand(NULL)
	{
		
	}


	// Destructor
	~MD3WeaponModel()
	{
		delete weapon;
		delete hand;
	}


	// Draws the model
	int draw()
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, 0);
		glEnable(GL_TEXTURE_2D);
		drawModel(weapon);
		return 0;
	}


	// Loads the requested model
	int load(string modelName)
	{
		name = modelName;

		FILE* weaponFile = NULL;
		
		// Open our file handles
		string fileName = modelName + ".md3";
		weaponFile = fopen(fileName.c_str(), "rb");
		if(weaponFile == NULL)
		{
			return 1;
		}

		// Create our model objects
		weapon = new md3_data;
		
		// Read the data for each file
		readData(weaponFile, weapon);
		
		// Close our file handles
		fclose(weaponFile);
		
		// FIXME: jpg or tga
		weapon->pMesh[0].textureName = modelName + ".jpg";
		
		// Loads the model's textures
		loadTextures(weapon);

		// Fix up the texture coordinates
		//int textureCoordCount = weapon->pMesh[0].numMeshFrames * weapon->pMesh[0].numVerticies;
		//for(int i = 0; i < textureCoordCount; ++i)
		//	weapon->pMesh[0].pUVtex[i].texVec[1] = 1 - weapon->pMesh[0].pUVtex[i].texVec[1];

		return 0;
	}

private:

	// A single component for single models
	md3_data* weapon;
	md3_data* hand;
};

#endif
