
// MD3Model.h
// Definition of our MD3 class and related structures 
// (c) 2002 Marty Dill

#ifndef MD3MODEL_H
#define MD3MODEL_H


#include "Fizz3d.h"
#include "Model.h"
#include "Vector.h"
#include "Log.h"
#include "Q3Shader.h"

using namespace std;


// How many animations are there?
#define MAX_ANIMATIONS 25


// Enumerates the possible frames of animation
enum AnimFrames
{ 
	BOTH_DEATH1 = 0,
	BOTH_DEAD1,
	BOTH_DEATH2,
	BOTH_DEAD2,
	BOTH_DEATH3,
	BOTH_DEAD3,
	TORSO_GESTURE,
	TORSO_ATTACK,
	TORSO_ATTACK2,
	TORSO_DROP,
	TORSO_RAISE,
	TORSO_STAND,
	TORSO_STAND2,
	LEGS_WALKCR,
	LEGS_WALK,
	LEGS_RUN,
	LEGS_BACK,
	LEGS_SWIM,
	LEGS_JUMP,
	LEGS_LAND,
	LEGS_JUMPB,
	LEGS_IDLE,
	LEGS_IDLECR,
	LEGS_TURN
};


// Enumerates the possible model segments 
enum
{
	HEAD = 0,
	BODY,
	LEGS,
	WEAPON,
	HAND,
	BARREL
};


// The header that all MD3 files contain
struct md3_header
{
	char id[4];
	int  version;
	char fileName[68];
	int  numBoneFrames;
	int  numTags;
	int  numMeshes;
	int  numMaxSkins;
	int  headerLength;
	
	// Offset of the start of the tag structures
	int  tagStart;

	// Offset of the end of the tag structures (which is the start of the mesh structures)
	int  tagEnd;

	int  size;   
};


// A tag within an MD3 file
struct md3_tag
{
	// Name of the tag
	char name[64];

	// Position of the tag
	float position[3];
		
	// The tag's 4x4 rotation matrix - 
	// Note that .md3 files only store a 3x3 matrix; we use that to create a 4x4 matrix
	float rotation[4][4];
};


// An MD3 file boneframe
struct md3_boneframe
{
	float mins[3];
	float maxs[3];
	float position[3];
	float scale;
	char creator[16]; 
};


// Skin struct - stores name of skin
struct md3_skin
{
	char name[68];              
};


// Triangle struct - stores triangle vertex indicies
struct md3_triangle
{
	int triangle[3];
};


// Texcoord struct - stores U/V texture coordinates of a vertex
struct md3_uvtex
{
	float texVec[2];
};


// Vertex struct - stores position and environmental texture mapping coordinates
struct md3_vertex
{
	signed short vec[3];
	unsigned char envTex[2];
};


// Mesh struct
struct md3_mesh
{
	md3_mesh() : pShader(NULL)
	{
	}

	char id[4];

	char name[68];
	int numMeshFrames;
	int numSkins;
	int numVerticies;
	int numTriangles;

	int triangleStart;	//starting position of 
						//Triangle data, relative
						//to start of Mesh_Header 

	int headerSize;
	
	int texVecStart;	//starting position of
						//texvector data, relative
						//to start of Mesh_Header 

	int vertexStart;	//starting position of  
						//vertex data,relative                   
						//to start of Mesh_Header 

	int meshSize;
	
	// Structs to store various pieces of info about the mesh
	md3_skin* pSkin;
	md3_triangle* pTriangle;
	md3_uvtex* pUVtex;
	md3_vertex* pVertex;
	unsigned int textureTag;
	string textureName;
	Q3Shader* pShader;
};


// Represents the MD3 data of a single component of a model
struct md3_data
{
	md3_header*		pHeader;
	md3_tag*		pTag;
	md3_boneframe*	pBoneFrame;
	md3_mesh*		pMesh;

	md3_data** links;
	int currentFrame;
	GLuint textureTag;
	string textureName;
	int currentAnim;
};


// Base MD3 model class
class MD3Model : public IModel
{
public:
	MD3Model();
	virtual ~MD3Model();

	int load(string fileName);
	int draw(void);
	virtual int setAnimation(int whichPart, int animation);


	static ITextureFactory* textureFactory;

protected:
	int loadAnimations(void);
	int loadTextures(md3_data* model);
	int loadSkin(md3_data* model, string skinName);
	int readData(FILE* file, md3_data* md3Data);
	int attach(md3_data* base, md3_data* attachment, char* tagName);
	int drawModel(md3_data* model);
	int renderMesh(md3_data* mesh);
	int writeModelInfo(ofstream* out, md3_data* model);

	// The model's name
	string name;

	// Animation frame info
	int animStart[MAX_ANIMATIONS];
	int animEnd[MAX_ANIMATIONS];
};


// FIXME: Move child classes somewhere else?


class MD3GenericModel : public MD3Model
{
public:

	// Constructor
	MD3GenericModel() :
		model(NULL)
	{
		
	}


	// Destructor
	~MD3GenericModel()
	{
		delete model;
	}


	// Draws the model
	int draw()
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, 0);
		glEnable(GL_TEXTURE_2D);
		drawModel(model);

		return 0;
	}


	bool EndsWith(const string& a, const string& b)
	{
		if (b.size() > a.size()) return false;
		return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
	}


	// Loads the requested model
	int load(string modelName)
	{
		name = modelName;

		FILE* modelFile = NULL;
	
		// Open our file handles
		string fileName = modelName ;//+ ".md3";
		if(!EndsWith(fileName, ".md3"))
			fileName += ".md3";
		modelFile = fopen(fileName.c_str(), "rb");

		if(modelFile == NULL)
			return 1;
		
		model = new md3_data;

		// Read the data for each file
		readData(modelFile, model);
	
		// Close our file handles
		fclose(modelFile);
		
		for(int i = 0; i < model->pHeader->numMeshes; ++i)
		{
			string s = model->pMesh[i].pSkin[0].name;
			string t = s.substr(0, s.size() - 3);
			t += "jpg"; 
				
			model->pMesh[i].textureName = t;
		}

		// Loads the model's textures
		loadTextures(model);

		return 0;
	}

private:
	md3_data* model;
};



#endif
