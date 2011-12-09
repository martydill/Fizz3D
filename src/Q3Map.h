
// Q3Map.h
// Defines the class used for Quake 3 .bsp maps
// (c) 2002 Marty Dill


#ifndef Q3MAP_H
#define Q3MAP_H


#include "Fizz3d.h"
#include "Q3Structs.h"
#include "Entity.h"
//#include "Q3Shader.h"
#include "Map.h"
#include "TextureFactory.h"



class Q3Map : public Map
{
public:
	Q3Map();
	Q3Map(const char* fileName);
	~Q3Map();

	// Functions for reading the different structures
	int readDirEntries(void);
	int readEntities(void);
	int readVertices(void);
	int readFaces(void);
   int readEffects(void);
	int readMeshVerts(void);
	int readLeafs(void);
	int readNodes(void);
	int readVisData(void);
	int readPlanes(void);
	int readBrushes(void);
	int readBrushSides(void);

	// Functions for reading textures - require pointer to texture factory
	int readTextures(ITextureFactory* pTexFac);
	int readLightMaps(ITextureFactory* pTexFac);

	// Patch generation functions
	int genPatches(void);
	int computePatch(int index);
	int genDisplayList(int index);

	// Functions for reading different data structures from the map data
	int extractInt(const char** pData);
	float extractFloat(const char** pData);
	Vec2 extractVec2(const char** pData);
	Vec3 extractVec3(const char** pData);

	// Collision detection
	bool isTraversable(Vec3* from, Vec3* to);


	// Variables

	// File pointer
	FILE* file;

	// Magic number - 'IBSP'
	char magic[4];

	// Map's version
	int version;

	// Pointer to the file's data in memory
	char* pFileData;

	// Indicies of various structures
	DirEntry directory[17];
	
	// Shader data
	Q3MapShader* pMapShader;
	int numMapShaders;

	/*Q3Shader* pShader;
	int numShaders;*/

   Q3Effect* pEffects;
   int numEffects;

	// Vertex data
	Q3Vertex* pVertex;
	int numVertices;

	// face Data
	Q3Face* pFace;
	int numFaces;

	// Mesh data
	Q3MeshVert* pMeshVert;
	int numMeshVerts;

	// Lightmap data
	Q3LightMap* pLightMap;
	int numLightMaps;

	// Leaf data
	Q3Leaf* pLeaf;
	int numLeafs;

	// Leaf face data
	Q3LeafFace* pLeafFace;
	int numLeafFaces;

	// Leafbrush data
	Q3LeafBrush* pLeafBrush;
	int numLeafBrushes;

	// Plane data
	Q3Plane* pPlane;
	int numPlanes;

	// Node data
	Q3Node* pNode;
	int numNodes;

	// VIS data - there is only one
	Q3VisData* pVisData;

	// Brush data
	Q3Brush* pBrush;
	int numBrushes;

	// Brushside data
	Q3BrushSide* pBrushSide;
	int numBrushSides;

	std::vector<Entity*> entities;
};


// Defines for the various lumps in the mamp
namespace Q3Lumps
{
	const int Entities = 0;
	const int Textures = 1;
	const int Planes = 2;
	const int Nodes = 3;
	const int Leafs = 4; 
	const int Leaffaces = 5; 
	const int Leafbrushes = 6;
	const int Models = 7;
	const int Brushes = 8;
	const int Brushsides = 9;
	const int Vertexes = 10;
	const int Meshverts = 11;  
	const int Effects  = 12;
	const int Faces = 13;
	const int Lightmaps = 14;
	const int Lightvols = 15;  
	const int Visdata  = 16;
};


// Definitions of various face types
namespace Q3FaceType
{
	const int Poly = 1;
	const int Patch = 2;
	const int Mesh = 3;
	const int Billboard = 4;
};


#endif
