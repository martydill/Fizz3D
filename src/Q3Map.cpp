
// Q3Map.cpp
// Class for loading Quake 3 .bsp files
// (c) 2002 Marty Dill
// The .BSP specs can be found at http://graphics.stanford.edu/~kekoa/q3


#include "Fizz3d.h"
#include "Q3Map.h"
#include "Engine.h"
//#include "Shader.h"
#include "ShaderParser.h"
#include "Config.h"
#include "Log.h"
#include "Math.h"


using namespace std;

static const char* moduleName = "MapLoader";

// Constructor ...
Q3Map::Q3Map()
{
}


// FIXME: Add logging 
// Constructor, specifying name of file to load
Q3Map::Q3Map(const char* fileName)
{
	Assert(fileName != NULL);

	Trace("Opening map %s", fileName);

	// Open the file
	file = fopen(fileName, "rb");

	// Something went wrong ...
	if(file == NULL)
		CEngine::CriticalError("Unable to open map!");

	// Calculate the file's size
	long start = ftell(file);
	fseek(file, 0, SEEK_END);
	long end = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Read the entire map into memory
	pFileData = (char*)malloc(end - start);
	fread(pFileData, 1, end - start, file);
	fclose(file);

	// Fill the structures
	Trace("Reading map structures");
	readDirEntries();
	readEntities();
	readVertices();
	readMeshVerts();
	readLeafs();
	readPlanes();
	readNodes();
	readVisData();
	readBrushes();
	readBrushSides();
    readEffects();
    readFaces();
	// Do post-processing
	genPatches();
}


// Destructor
Q3Map::~Q3Map()
{
	for(int i = 0; i < numFaces; ++i)
	{
		if(pFace[i].patch != NULL)
		{
			for(int s = 0; s < pFace[i].patch->sections; ++s)
			{
				//for(int j = 0; j < pFace[i].patch->numVertices[s];  ++j)
				delete[] pFace[i].patch->vertices[s];
			}

			delete[] pFace[i].patch->numVertices;
			delete pFace[i].patch;
		}
	}

	delete[] pVisData->vec;
	delete[] pMapShader;
	//delete[] pShader;
	delete[] pVertex;
	delete[] pFace;
	delete[] pMeshVert;
	delete[] pLightMap;
	delete[] pLeaf;
	delete[] pLeafFace;
	delete[] pLeafBrush;
	delete[] pPlane;
	delete[] pNode;
	delete[] pVisData;
	delete[] pBrush;
	delete[] pBrushSide;
	delete[] pFileData;
}


// Reads the map's directory entries
int Q3Map::readDirEntries(void)
{
	const char* pData = pFileData + 8;

	// Read each directory entry
	for(int i = 0; i < 17; i++)
	{
		directory[i].offset = extractInt(&pData);
		directory[i].length = extractInt(&pData);
	}

	return 0;
}


// Reads and parses entity data
// FIXME: Write entity parser
int Q3Map::readEntities(void)
{
	const char* pData = pFileData + directory[Q3Lumps::Entities].offset;

	char* buf = new char[directory[Q3Lumps::Entities].length];

	memcpy(buf, pData, directory[Q3Lumps::Entities].length);

	Entity* entity;
	char* token = strtok(buf, " \"\r\n");
	while(token != NULL)
	{
		string t = string(token);
		if(t == "{")
		{
			entity = new Entity();
		}
		else if(t == "}")
		{
			this->entities.push_back(entity);
		}
		else if(t == "classname")
		{
			token = strtok(NULL, " \"\r\n");
			t = string(token);

			entity->setName(t);

			if(t == "info_player_deathmatch")
			{
				entity->setEntityType(WorldSpawn);
			}
			else if(t == "weapon_plasmagun")
			{
				entity->setEntityType(PlasmaGun);
			}
			else if(t == "weapon_shotgun")
			{
				entity->setEntityType(Shotgun);
			}
			else if(t == "weapon_rocketlauncher")
			{
				entity->setEntityType(RocketLauncher);
			}
			else if(t == "item_armor_body")
			{
				entity->setEntityType(RedArmor);
			}
			else if(t == "ammo_bullets")
			{
				entity->setEntityType(Ammo_Bullets);
			}
			else if(t == "ammo_shells")
			{
				entity->setEntityType(Ammo_Shells);
			}
			else if(t == "ammo_cells")
			{
				entity->setEntityType(Ammo_Cells);
			}
			else if(t == "item_armor_shard")
			{
				entity->setEntityType(Item_Armor_Shard);
			}
				else if(t == "item_health")
			{
				entity->setEntityType(Item_Health);
			}
			else if(t == "item_health_large")
			{
				entity->setEntityType(Item_Health_Large);
			}
			else
				Trace(t.c_str());
		}
		else if(t == "music")
		{
			token = strtok(NULL, "\"\r\n");
			token = strtok(NULL, "\"\r\n");
		}
		else if(t == "ambient")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "model")
		{
			token = strtok(NULL, " \"\r\n");
			entity->loadModel(string(token));
		}
		else if(t == "light")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "message")
		{
			token = strtok(NULL, "\"\r\n");
			token = strtok(NULL, "\"\r\n|");
		}
		else if(t == "origin")
		{
			float x = atof(strtok(NULL, " \"\r\n"));
			float y = atof(strtok(NULL, " \"\r\n"));
			float z = atof(strtok(NULL, " \"\r\n"));
			entity->setPosition(Vec3(x, y, z));
		}
		else if(t == "targetname")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "target")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "wait")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "spawnflags")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "nobots")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "noise")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "angle")
		{
			token = strtok(NULL, " \"\r\n");
			entity->setAzimuth(atof(token));
		}
		else if(t == "radius")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "random")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "roll")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "speed")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "team")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "_minlight")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "lip")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "dmg")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "model2")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "notfree")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "range")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "weight")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "notteam")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "phase")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "height")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "count")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "health")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "delay")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "style")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "notsingle")
		{
			token = strtok(NULL, " \"\r\n");
		}
		else if(t == "_color")
		{
			float r = atof(strtok(NULL, " \"\r\n"));
			float g = atof(strtok(NULL, " \"\r\n"));
			float b = atof(strtok(NULL, " \"\r\n"));
		}
		else
		{
			Assert(false);
		}

		token = strtok(NULL, " \"\r\n");
	}

	return 0;
}


// Fills the texture array with texture data
int Q3Map::readTextures(ITextureFactory* pTexFac)
{
	numMapShaders = directory[Q3Lumps::Textures].length / (64 + 4 + 4);
	pMapShader = new Q3MapShader[numMapShaders];

	const char* pData = pFileData + directory[Q3Lumps::Textures].offset;

	// For each shader ...
	for(int i = 0; i < numMapShaders; ++i)
	{
		// Grab the shader's name
		memcpy(pMapShader[i].name, pData, 64);
		pData += 64;

		// Grab its flags
		pMapShader[i].flags = extractInt(&pData);

		// Grab its contents
		pMapShader[i].contents = extractInt(&pData);

		// Acquire a pointer to the required shader
		pMapShader[i].pShader = ShaderParser::mapShaderName(pMapShader[i].name);
		Trace("Searching for %s", pMapShader[i].name);
		if(pMapShader[i].pShader)
		{
			pMapShader[i].pShader->loadTextures(pTexFac);
			Trace("Found texture %s", pMapShader[i].name);
		}
		else
		{
			//Trace("Shader %s not found", pMapShader[i].name);
			//pMapShader[i].pShader = new Q3Shader();
			//pMapShader[i].pShader->setCurrentStage(1);
			//pMapShader[i].pShader->setTextureName("$lightmap");
			//pMapShader[i].pShader->setSrcBlend(GL_ONE);
			//pMapShader[i].pShader->setDestBlend(GL_ZERO);
			//pMapShader[i].pShader->setCurrentStage(2);
			//pMapShader[i].pShader->setSrcBlend(GL_DST_COLOR);
			//pMapShader[i].pShader->setDestBlend(GL_ZERO);
			//string s = pMapShader[i].name;
			//s += ".tga";
			//pMapShader[i].pShader->setTextureName(s);
			//pMapShader[i].pShader->loadTextures(pTexFac);
		}
	}

	return 0;
}


// Fills the vertex array with vertex data
int Q3Map::readVertices(void)
{
	numVertices = directory[Q3Lumps::Vertexes].length / (10 * sizeof(float) + 4);

	pVertex = new Q3Vertex[numVertices];

	const char* pData = pFileData + directory[Q3Lumps::Vertexes].offset;

	for(int i = 0; i < numVertices; i++)
	{
		// Position
		pVertex[i].position = extractVec3(&pData);

		// Texture coordinates
		pVertex[i].texCoord = extractVec2(&pData);

		// Lightmap coords
		pVertex[i].lmCoord = extractVec2(&pData);

		//pData += 16;

		// Normal
		pVertex[i].normal = extractVec3(&pData);

		// Vertex colors
		pVertex[i].color[0] = *pData++;
		pVertex[i].color[1] = *pData++;
		pVertex[i].color[2] = *pData++;
		pVertex[i].color[3] = *pData++;
	}

	return 0;
}


// Read the faces data
int Q3Map::readFaces(void)
{
	numFaces = directory[Q3Lumps::Faces].length / (26 * sizeof(int));

	pFace = new Q3Face[numFaces];

	const char* pData = pFileData + directory[Q3Lumps::Faces].offset;

	// For each face ..
	for(int i = 0; i < numFaces; i++)
	{
		pFace[i].texture = extractInt(&pData);
		pFace[i].effectIndex = extractInt(&pData);
		pFace[i].type = extractInt(&pData);
		pFace[i].vertex = extractInt(&pData);
		pFace[i].n_vertexes = extractInt(&pData);
		pFace[i].meshvert = extractInt(&pData); 
		pFace[i].n_meshverts = extractInt(&pData);
		pFace[i].lm_index = extractInt(&pData);
		pFace[i].lm_start[0] = extractInt(&pData);
		pFace[i].lm_start[1] = extractInt(&pData);
		pFace[i].lm_size[0] = extractInt(&pData);
		pFace[i].lm_size[1] = extractInt(&pData);
		pFace[i].lm_origin = extractVec3(&pData);

		pData += (9 * sizeof(int));

		pFace[i].size[0] = extractInt(&pData);
		pFace[i].size[1] = extractInt(&pData);
		pFace[i].patch = NULL;

      if(pFace[i].effectIndex != -1)
         pFace[i].effect = &pEffects[pFace[i].effectIndex];
      else
         pFace[i].effect = NULL;
	}

	return 0;
}


// Reads the meshvert data
int Q3Map::readMeshVerts(void)
{
	numMeshVerts = directory[Q3Lumps::Meshverts].length / sizeof(int);

	pMeshVert = new Q3MeshVert[numMeshVerts];

	const char* pData = pFileData + directory[Q3Lumps::Meshverts].offset;

	for(int i = 0; i < numMeshVerts; i++)
		pMeshVert[i].offset = extractInt(&pData);

	return 0;
}


// Reads the lightmap data
int Q3Map::readLightMaps(ITextureFactory* pTexFac)
{
	char* pData = pFileData + directory[Q3Lumps::Lightmaps].offset;

	numLightMaps = directory[Q3Lumps::Lightmaps].length / (128 * 128 * 3);

	// Allocate space for the lightmaps and lightmap textures
	pLightMap = new Q3LightMap[numLightMaps];

	// For each lightmap ...
	for(int i = 0; i < numLightMaps; ++i)
	{
		//	for(int j = 0; j < 128 * 128 * 3; j += 3)
		//	{
		//		BYTE* c = (BYTE*)&pData[j];
		//		unsigned int top, r, g, b;
		//		   top =
		//       r = (unsigned int) ((float) c[0] * 4);
		//       g = (unsigned int) ((float) c[1] * 4);
		//       b = (unsigned int) ((float) c[2] * 4);
		//       if (g > top) top = g;
		//       if (b > top) top = b;
		//       if (top > 255)
		//       {
		//         top = (255 << 8) / top;
		//         r = (r * top) >> 8;
		//         g = (g * top) >> 8;
		//         b = (b * top) >> 8;
		//       }

		//	pData[j] = (unsigned char)r;
		//	pData[j + 1] = (unsigned char)g;
		//	pData[j + 2] = (unsigned char)b;
		//	}

		unsigned char* foo = new unsigned char[128*128*4];
		int k = 0;
		for(int j = 0; j < 128 * 128 * 4; j+=4)
		{

			BYTE* c = (BYTE*)&pData[k];
			unsigned int top, r, g, b;
			top =
				r = (unsigned int) ((float) c[0] * 4);
			g = (unsigned int) ((float) c[1] * 4);
			b = (unsigned int) ((float) c[2] * 4);
			if (g > top) top = g;
			if (b > top) top = b;
			if (top > 255)
			{
				top = (255 << 8) / top;
				r = (r * top) >> 8;
				g = (g * top) >> 8;
				b = (b * top) >> 8;
			}



			foo[j] = (unsigned char)r;//pData[k++];
			foo[j + 1] = (unsigned char)g;//pData[k++];
			foo[j + 2] = (unsigned char)b;//pData[k++];
			foo[j + 3] = 0xFF;
			k += 3;
		}

		pLightMap[i].textureId = pTexFac->buildLightMap((unsigned char*)foo);
		pData += (128 * 128 * 3);
	}

	return 0;
}


// Read leaf data
int Q3Map::readLeafs(void)
{
	const char* pData = pFileData + directory[Q3Lumps::Leafs].offset;

	numLeafs = directory[Q3Lumps::Leafs].length / (12 * sizeof(int));
	numLeafFaces = directory[Q3Lumps::Leaffaces].length / (1 * sizeof(int));
	numLeafBrushes = directory[Q3Lumps::Leafbrushes].length / (1 * sizeof(int));

	pLeaf = new Q3Leaf[numLeafs];
	pLeafFace = new Q3LeafFace[numLeafFaces];
	pLeafBrush = new Q3LeafBrush[numLeafBrushes];

	// Read in all of the data for our leaf structure
	for(int i = 0; i < numLeafs; ++i)
	{
		pLeaf[i].cluster = extractInt(&pData);
		pLeaf[i].area = extractInt(&pData);

		for(int j = 0; j < 3; ++j)
			pLeaf[i].bbox.min[j] = extractInt(&pData);

		for(int j = 0; j < 3; ++j)
			pLeaf[i].bbox.max[j] = extractInt(&pData);

		pLeaf[i].leafFace = extractInt(&pData);
		pLeaf[i].numLeafFaces = extractInt(&pData);
		pLeaf[i].leafBrush = extractInt(&pData);
		pLeaf[i].numLeafBrushes = extractInt(&pData);
	}

	pData = pFileData + directory[Q3Lumps::Leaffaces].offset;

	// Read in all of the data for our leaf face structure
	for(int i = 0; i < numLeafFaces; ++i)
		pLeafFace[i].face = extractInt(&pData);

	pData = pFileData + directory[Q3Lumps::Leafbrushes].offset;

	// Read in all of the data for our leafbrush structure
	for(int i = 0; i < numLeafBrushes; ++i)
		pLeafBrush[i].brush = extractInt(&pData);

	return 0;
}


// Read plane data
int Q3Map::readPlanes(void)
{
	const char* pData = pFileData + directory[Q3Lumps::Planes].offset;

	numPlanes = directory[Q3Lumps::Planes].length / (4 * sizeof(int));
	pPlane = new Q3Plane[numPlanes];

	// For each plane ...
	for(int i = 0; i < numPlanes; ++i)
	{
		pPlane[i].normal = extractVec3(&pData);
		pPlane[i].distance = extractFloat(&pData);
	}

	return 0;
}


// Read effect data
int Q3Map::readEffects(void)
{
   const char* pData = pFileData + directory[Q3Lumps::Effects].offset;

	numEffects = directory[Q3Lumps::Effects].length / (72);
   pEffects = new Q3Effect[numEffects];

	// For each plane ...
	for(int i = 0; i < numEffects; ++i)
	{
      char shaderName[64];
      memcpy(shaderName, pData, sizeof(shaderName));
      pData += 64;

      pEffects[i].shaderName = shaderName;
      pEffects[i].brushIndex = extractInt(&pData);
      pData += 4; // skip over unknown

      pEffects[i].brush = &pBrush[pEffects[i].brushIndex];
	  pEffects[i].shader = ShaderParser::mapShaderName(pEffects[i].shaderName.c_str());
	}

	return 0;
}


// Read node data
int Q3Map::readNodes(void)
{
	const char* pData = pFileData + directory[Q3Lumps::Nodes].offset;

	numNodes = directory[Q3Lumps::Nodes].length / (9 * sizeof(int));
	pNode = new Q3Node[numNodes];

	// For each node ...
	for(int i = 0; i < numNodes; ++i)
	{
		pNode[i].planeIndex = extractInt(&pData);
		pNode[i].children[0] = extractInt(&pData);
		pNode[i].children[1] = extractInt(&pData);

		for(int j = 0; j < 3; ++j)
			pNode[i].bbox.min[j] = extractInt(&pData);

		for(int j = 0; j < 3; ++j)
			pNode[i].bbox.max[j] = extractInt(&pData);
	}

	return 0;
}


// Read visibility data
int Q3Map::readVisData(void)
{
	const char* pData = pFileData + directory[Q3Lumps::Visdata].offset;

	pVisData = new Q3VisData;

	pVisData->numVectors = extractInt(&pData);
	pVisData->vectorSize = extractInt(&pData);

	pVisData->vec = new unsigned char[pVisData->numVectors * pVisData->vectorSize];

	memcpy(pVisData->vec, pData, pVisData->numVectors * pVisData->vectorSize);

	return 0;
}


// Read brush data
int Q3Map::readBrushes(void)
{
	const char* pData = pFileData + directory[Q3Lumps::Brushes].offset;
	numBrushes = directory[Q3Lumps::Brushes].length / (3 * sizeof(int));

	pBrush = new Q3Brush[numBrushes];

	// For each brush ...
	for(int i = 0; i < numBrushes; ++i)
	{
		pBrush[i].brushSideIndex = extractInt(&pData);
		pBrush[i].numBrushSides = extractInt(&pData);
		pBrush[i].textureIndex = extractInt(&pData);
	}

	return 0;
}


// Read brushside data
int Q3Map::readBrushSides(void)
{
	const char* pData = pFileData + directory[Q3Lumps::Brushsides].offset;
	numBrushSides = directory[Q3Lumps::Brushsides].length / (2 * sizeof(int));

	pBrushSide = new Q3BrushSide[numBrushSides];

	// For each brushside ...
	for(int i = 0; i < numBrushSides; ++i)
	{
		pBrushSide[i].planeIndex = extractInt(&pData);
		pBrushSide[i].textureIndex = extractInt(&pData);
	}

	return 0;
}


// Utility functions


// Precomputes all of the patches
int Q3Map::genPatches(void)
{
	for(int i = 0; i < numFaces; ++i)
	{
		if(pFace[i].type == Q3FaceType::Patch)
		{
			computePatch(i);
			genDisplayList(i);
		}
	}

	return 0;
}


// Generate a single patch.
// Breaks the m x n control point matrix into a series of 3x3 matricies, then tesselates them individually
// Could probably use some optimization, but hey, this isn't a critical path
int Q3Map::computePatch(int index)
{
	// Grab matrix dimensions
	int mSteps = pFace[index].size[0];
	int nSteps = pFace[index].size[1];

	pFace[index].patch = new Q3Patch;

	int i = pFace[index].vertex;

	int numSteps = Config::getIntValue("PatchDetail");

	float step = (float)1 / numSteps; 

	int points = 0;

	Q3Vertex* pv;

	int sections = ((nSteps - 1) / 2) * ((mSteps - 1) / 2);
	pFace[index].patch->vertices = new Q3Vertex*[sections];
	pFace[index].patch->numVertices = new int[sections];
	pFace[index].patch->sections = sections;
	pFace[index].patch->numSteps = numSteps;

	int currentSection = -1;

	// For each 3x3 patch ...
	for(int n = 0; n < nSteps - 1; n += 2)
	{
		for(int m = 0; m < mSteps - 1; m += 2)
		{
			currentSection++;
			pv = new Q3Vertex[(numSteps + 1) * (numSteps + 1)];

			// Loop through each step value and calculate corresponding points
			for(float currentUStep = 0; currentUStep <= 1; currentUStep += step)
			{	
				for(float currentVStep = 0; currentVStep <= 1; currentVStep += step)
				{
					// Compute vertex data
					float b0 = (1 - currentVStep) * (1 - currentVStep);
					float b1 = 2 * currentVStep * (1 - currentVStep);
					float b2 = currentVStep * currentVStep;

					float x1 = pVertex[i + m + n * mSteps].position.x * b0 + pVertex[i + m + n * mSteps + 1].position.x * b1 + pVertex[i + m + n * mSteps + 2].position.x * b2;
					float y1 = pVertex[i + m + n * mSteps].position.y * b0 + pVertex[i + m + n * mSteps + 1].position.y * b1 + pVertex[i + m + n * mSteps + 2].position.y * b2;
					float z1 = pVertex[i + m + n * mSteps].position.z * b0 + pVertex[i + m + n * mSteps + 1].position.z * b1 + pVertex[i + m + n * mSteps + 2].position.z * b2;

					float x2 = pVertex[i + m + (n + 1) * mSteps].position.x * b0 + pVertex[i + m + (n + 1) * mSteps + 1].position.x * b1 + pVertex[i + m + (n + 1) * mSteps + 2].position.x * b2;
					float y2 = pVertex[i + m + (n + 1) * mSteps].position.y * b0 + pVertex[i + m + (n + 1) * mSteps + 1].position.y * b1 + pVertex[i + m + (n + 1) * mSteps + 2].position.y * b2;
					float z2 = pVertex[i + m + (n + 1) * mSteps].position.z * b0 + pVertex[i + m + (n + 1) * mSteps + 1].position.z * b1 + pVertex[i + m + (n + 1) * mSteps + 2].position.z * b2;

					float x3 = pVertex[i + m + (n + 2) * mSteps].position.x * b0 + pVertex[i + m + (n + 2) * mSteps + 1].position.x * b1 + pVertex[i + m + (n + 2) * mSteps + 2].position.x * b2;
					float y3 = pVertex[i + m + (n + 2) * mSteps].position.y * b0 + pVertex[i + m + (n + 2) * mSteps + 1].position.y * b1 + pVertex[i + m + (n + 2) * mSteps + 2].position.y * b2;
					float z3 = pVertex[i + m + (n + 2) * mSteps].position.z * b0 + pVertex[i + m + (n + 2) * mSteps + 1].position.z * b1 + pVertex[i + m + (n + 2) * mSteps + 2].position.z * b2;

					float c0 = (1 - currentUStep) * (1 - currentUStep);
					float c1 = 2 * currentUStep * (1 - currentUStep);
					float c2 = currentUStep * currentUStep;

					float a1 = x1 * c0 + x2 * c1 + x3 * c2;
					float a2 = y1 * c0 + y2 * c1 + y3 * c2;
					float a3 = z1 * c0 + z2 * c1 + z3 * c2;

					pv[points].position.x = a1;
					pv[points].position.y = a2;
					pv[points].position.z = a3;


					// Compute lightmap data
					x1 = pVertex[i + m + n * mSteps].lmCoord.x * b0 + pVertex[i + m + n * mSteps + 1].lmCoord.x * b1 + pVertex[i + m + n * mSteps + 2].lmCoord.x * b2;
					y1 = pVertex[i + m + n * mSteps].lmCoord.y * b0 + pVertex[i + m + n * mSteps + 1].lmCoord.y * b1 + pVertex[i + m + n * mSteps + 2].lmCoord.y * b2;

					x2 = pVertex[i + m + (n + 1) * mSteps].lmCoord.x * b0 + pVertex[i + m + (n + 1) * mSteps + 1].lmCoord.x * b1+ pVertex[i + m + (n + 1) * mSteps + 2].lmCoord.x * b2;
					y2 = pVertex[i + m + (n + 1) * mSteps].lmCoord.y * b0 + pVertex[i + m + (n + 1) * mSteps + 1].lmCoord.y * b1+ pVertex[i + m + (n + 1) * mSteps + 2].lmCoord.y * b2;

					x3 = pVertex[i + m + (n + 2) * mSteps].lmCoord.x * b0 + pVertex[i + m + (n + 2) * mSteps + 1].lmCoord.x * b1 + pVertex[i + m + (n + 2) * mSteps + 2].lmCoord.x * b2;
					y3 = pVertex[i + m + (n + 2) * mSteps].lmCoord.y * b0 + pVertex[i + m + (n + 2) * mSteps + 1].lmCoord.y * b1 + pVertex[i + m + (n + 2) * mSteps + 2].lmCoord.y * b2;

					a1 = x1 * c0 + x2 * c1 + x3 * c2;
					a2 = y1 * c0 + y2 * c1 + y3 * c2;

					pv[points].lmCoord.x = a1;
					pv[points].lmCoord.y= a2;


					// Compute texcoord data
					x1 = pVertex[i + m + n * mSteps].texCoord.x * b0 + pVertex[i + m + n * mSteps + 1].texCoord.x * b1 + pVertex[i + m + n * mSteps + 2].texCoord.x * b2;
					y1 = pVertex[i + m + n * mSteps].texCoord.y * b0 + pVertex[i + m + n * mSteps + 1].texCoord.y * b1 + pVertex[i + m + n * mSteps + 2].texCoord.y * b2;

					x2 = pVertex[i + m + (n + 1) * mSteps].texCoord.x * b0 + pVertex[i + m + (n + 1) * mSteps + 1].texCoord.x * b1+ pVertex[i + m + (n + 1) * mSteps + 2].texCoord.x * b2;
					y2 = pVertex[i + m + (n + 1) * mSteps].texCoord.y * b0 + pVertex[i + m + (n + 1) * mSteps + 1].texCoord.y * b1+ pVertex[i + m + (n + 1) * mSteps + 2].texCoord.y * b2;

					x3 = pVertex[i + m + (n + 2) * mSteps].texCoord.x * b0 + pVertex[i + m + (n + 2) * mSteps + 1].texCoord.x * b1 + pVertex[i + m + (n + 2) * mSteps + 2].texCoord.x * b2;
					y3 = pVertex[i + m + (n + 2) * mSteps].texCoord.y * b0 + pVertex[i + m + (n + 2) * mSteps + 1].texCoord.y * b1 + pVertex[i + m + (n + 2) * mSteps + 2].texCoord.y * b2;

					a1 = x1 * c0 + x2 * c1 + x3 * c2;
					a2 = y1 * c0 + y2 * c1 + y3 * c2;

					pv[points].texCoord.x = a1;
					pv[points].texCoord.y = a2;

					points++;
				}
			}

			pFace[index].patch->vertices[currentSection] = pv;
			pFace[index].patch->numVertices[currentSection] = points;

			points = 0;
		}
	}

	return 0;
}


// Generate a display list for the specified bezier patch
int Q3Map::genDisplayList(int i)
{
	/*	pFace[i].patch->dispList = glGenLists(1);

	glNewList(pFace[i].patch->dispList, GL_COMPILE);

	for(int n = 0; n < pFace[i].patch->sections; ++n)
	{
	Q3Vertex* pv = pFace[i].patch->vertices[n];
	int numSteps = pFace[i].patch->numSteps;

	glDisable(GL_BLEND);

	// Set up first TMU				
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, pLightMap[pFace[i].lm_index].textureNum);	
	glEnable(GL_TEXTURE_2D);

	// Set up second TMU
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, pShader[pFace[i].texture].glTag);
	glEnable(GL_TEXTURE_2D);

	// FIXME: Optimize this ...
	for(int a = 0; a < numSteps; ++a)
	{
	for(int b = 0; b < numSteps; ++b)
	{
	glBegin(GL_TRIANGLE_STRIP);

	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, pv[b * (numSteps + 1) + a].lmCoord.x, pv[b * (numSteps + 1) + a].lmCoord.y); 
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, pv[b * (numSteps + 1) + a].texCoord.x, pv[b * (numSteps + 1) + a].texCoord.y); 
	glVertex3f(pv[b * (numSteps + 1) + a].position.x, pv[b * (numSteps + 1) + a].position.y, pv[b * (numSteps + 1) + a].position.z);

	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, pv[(b + 1) * (numSteps + 1) + a].lmCoord.x, pv[(b + 1) * (numSteps + 1) + a].lmCoord.y);
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, pv[(b + 1) * (numSteps + 1) + a].texCoord.x, pv[(b + 1) * (numSteps + 1) + a].texCoord.y);
	glVertex3f(pv[(b+1) * (numSteps + 1) + a].position.x, pv[(b+1) * (numSteps + 1) + a].position.y, pv[(b+1) * (numSteps + 1) + a].position.z);

	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, pv[b * (numSteps + 1) + a+1].lmCoord.x, pv[b * (numSteps + 1) + a+1].lmCoord.y); 
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, pv[b * (numSteps + 1) + a+1].texCoord.x, pv[b * (numSteps + 1) + a+1].texCoord.y); 
	glVertex3f(pv[(b) * (numSteps + 1) + a+1].position.x, pv[(b) * (numSteps + 1) + a+1].position.y, pv[(b) * (numSteps + 1) + a+1].position.z);

	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, pv[(b + 1) * (numSteps + 1) + a + 1].lmCoord.x, pv[(b + 1) * (numSteps + 1) + a + 1].lmCoord.y); 
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, pv[(b + 1) * (numSteps + 1) + a + 1].texCoord.x, pv[(b + 1) * (numSteps + 1) + a + 1].texCoord.y); 
	glVertex3f(pv[(b+1) * (numSteps + 1) + a+1].position.x, pv[(b+1) * (numSteps + 1) + a+1].position.y, pv[(b+1) * (numSteps + 1) + a+1].position.z);

	glEnd();
	}
	}

	glDisable(GL_TEXTURE_2D);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	}

	glEndList();
	*/
	return 0;
}


// Reads an integer from the given data stream
int Q3Map::extractInt(const char** pData)
{
	int i;

	Assert(pData != NULL);
	Assert(*pData != NULL);

	memcpy(&i, *pData, sizeof(float));
	*pData += sizeof(float);

	return i;
}


// Reads a float from the given data stream
float Q3Map::extractFloat(const char** pData)
{
	float f;

	Assert(pData != NULL);
	Assert(*pData != NULL);

	memcpy(&f, *pData, sizeof(float));
	*pData += sizeof(float);

	return f;
}


// Reads a 2 element vector from the given data stream
Vec2 Q3Map::extractVec2(const char** pData)
{
	Vec2 v;

	Assert(pData != NULL);
	Assert(*pData != NULL);

	memcpy(&v, *pData, 2 * sizeof(float));
	*pData += 2 * sizeof(float);

	return v;
}


// Reads a 3 element vector from the given data stream
Vec3 Q3Map::extractVec3(const char** pData)
{
	Vec3 v;

	Assert(pData != NULL);
	Assert(*pData != NULL);

	memcpy(&v, *pData, 3 * sizeof(float));
	*pData += 3 * sizeof(float);

	return v;
}


// Collision detection - checks whether it is possible to move from a to b
// FIXME: fix collision detection
bool Q3Map::isTraversable(Vec3* from, Vec3* pos)
{
	int cluster = -1;

	Q3Node* n = &pNode[0];

	while(1)
	{
		Q3Plane* p = &pPlane[n->planeIndex];

		float dist = pos->x * p->normal.x + pos->y * p->normal.y + pos->z * p->normal.z - p->distance;

		if(dist >= 0)
		{
			if(n->children[0] > 0)
			{
				n = &pNode[n->children[0]];
			}
			else
			{
				cluster = n->children[0];
				break;
			}
		}
		else
		{
			if(n->children[1] > 0)
			{
				n = &pNode[n->children[1]];
			}
			else
			{
				cluster = n->children[1];
				break;
			}
		}
	}

	if(pLeaf[~cluster].cluster < 0)
		return false;

	return true;
}
