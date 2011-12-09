
#ifndef MD3PLAYERMODEL_H
#define MD3PLAYERMODEL_H

#include "Fizz3d.h"

// Class specifically for .MD3 player models
class MD3PlayerModel : public MD3Model
{
public:

	// Constructor
	MD3PlayerModel() :
		head(NULL), upper(NULL), lower(NULL)
	{
		
	}


	// Destructor
	~MD3PlayerModel()
	{
		delete lower;
		delete upper;
		delete head;
	}


	void Fix(md3_data* mesh)
	{
		// For each component of the mesh ...
		for(int m = 0; m < mesh->pHeader->numMeshes; ++m)
		{
			int vertexIndex = mesh->currentFrame * mesh->pMesh[m].numVerticies;
		
			// For each triangle ...
			for(int i = 0; i < mesh->pMesh[m].numTriangles; ++i)
			{
				// For each vertex of the triangle ...
				for(int j = 0; j < 3; ++j)
				{
					int uvTexOffset = mesh->pMesh[m].pTriangle[i].triangle[j];
					//mesh->pMesh[m].pUVtex[uvTexOffset].texVec[1] = 1 - mesh->pMesh[m].pUVtex[uvTexOffset].texVec[1];
				}
			}
		}
	}

	// Loads the requested model
	int load(string modelName)
	{
		name = modelName;

		FILE* headFile = NULL;
		FILE* lowerFile = NULL;
		FILE* upperFile = NULL;

		// Open our file handles
		string fileName = modelName + "/head.md3";
		headFile = fopen(fileName.c_str(), "rb");

		fileName = modelName + "/lower.md3";
		lowerFile = fopen(fileName.c_str(), "rb");

		fileName = modelName + "/upper.md3";
		upperFile = fopen(fileName.c_str(), "rb");
		
		// Create our model objects
		head = new md3_data;
		lower = new md3_data;
		upper = new md3_data;

		// Read the data for each file
		readData(headFile, head);
		readData(lowerFile, lower);
		readData(upperFile, upper);

		// Close our file handles
		fclose(headFile);
		fclose(lowerFile);
		fclose(upperFile);

		// Create our model heirarchy
		attach(lower, upper, "tag_torso");
		attach(upper, head, "tag_head");

		// Load the skin information
		loadSkin(lower, modelName + "/lower_default.skin");
		loadSkin(upper, modelName + "/upper_default.skin");
		loadSkin(head, modelName + "/head_default.skin");

		// Loads the model's textures
		loadTextures(lower);
		loadTextures(upper);
		loadTextures(head);

		// Loads the model's animations
		loadAnimations();


		// Fix texture coordinates

		Fix(lower);
		Fix(upper);
		Fix(head);

		return 0;
	}


	// Sets the current animation
	int setAnimation(int whichPart, int animation)
	{
		if(whichPart == HEAD && head->currentAnim != animation)
		{
			head->currentAnim = animation;
			head->currentFrame = animStart[animation];
		}

		if(whichPart == BODY && upper->currentAnim != animation)
		{
			upper->currentAnim = animation;
			upper->currentFrame = animStart[animation];
		}

		if(whichPart == LEGS && lower->currentAnim != animation)
		{
			lower->currentAnim = animation;
			lower->currentFrame = animStart[animation];
		}

		return 0;
	}


	// Draw the entire .md3 model 
	int draw(void)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);

		// Start off by drawing the legs
		drawModel(lower);
		
		// Handle animations
		// FIXME: LERP these ...
		lower->currentFrame++;
		if(lower->currentFrame >= animEnd[lower->currentAnim])
			lower->currentFrame = animStart[lower->currentAnim];

		upper->currentFrame++;
		if(upper->currentFrame >= animEnd[upper->currentAnim])
			upper->currentFrame = animStart[upper->currentAnim];

		head->currentFrame++;
		if(head->currentFrame >= head->pHeader->numBoneFrames)
			head->currentFrame = 0;
		
		return 0;
	}


private:

	// 3 components = 3 md3_data structures
	md3_data* head;
	md3_data* upper;
	md3_data* lower;
};


#endif