
// Q3Structs.h
// Defines common Quake 3 engine structures
// (c) 2002 Marty Dill


#ifndef Q3STRUCTS_H
#define Q3STRUCTS_H


#include "Vector.h"
#include "Fizz3d.h"
#include "Texture.h"

class Q3Map;
class Q3Shader;


// TODO: Create Q3StructSizes enum ...


//#define D3DVERTEXTYPE (D3DFVF_XYZ | D3DFVF_TEX2)
#define D3DVERTEXTYPE2 (D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1))
#define D3DVERTEXTYPE (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct Vertex
{
	float x, y, z;
	DWORD color;
//	Vec2 texCoord;
//	Vec2 lmCoord;

};


// Bounding box structure used for collision detection
struct BoundingBox
{
	Vec3 centre;
	int min[3];
	int max[3];
};


// Bounding sphere strucutre used for collision detection
struct BoundingSphere
{
	Vec3 centre;
	int radius;
};


struct DirEntry
{
	int offset;
	int length;
};


struct Q3LightMap
{
	TextureID textureId;
};


struct Q3MeshVert
{
	int offset;
};


struct Q3MapShader
{
	char name[64];
	int flags;
	int contents;
	Q3Shader* pShader;
};

//
//struct Q3Shader
//{
//	char name[64];
//	int flags;
//	int contents;
//	GLuint glTag;
//	int shaderIndex;
//	TextureID textureId;
//};


struct Q3Vertex
{
	Vec3 position; 
	Vec2 texCoord;
	Vec2 lmCoord;
	Vec3 normal;
	BYTE color[4];
};


struct Q3Leaf
{
	int cluster;
	int area;
	BoundingBox bbox;
	int leafFace;
	int numLeafFaces;
	int leafBrush;
	int numLeafBrushes;  
};


struct Q3LeafFace
{
	int face;
};


struct Q3LeafBrush
{
	int brush;
};


struct Q3Patch
{
	int* numVertices;
	Q3Vertex** vertices;
	int sections;
	int numSteps;
	GLuint dispList;
};


struct Q3Brush
{
	int brushSideIndex;
	int numBrushSides;
	int textureIndex;
};


struct Q3Effect
{
	Q3Effect() : brush(NULL), brushIndex(-1), unknown(-1) { }

   std::string shaderName;
   int brushIndex;
   int unknown;
   Q3Brush* brush;
   Q3Shader* shader;
};


struct Q3Face
{
	Q3Face() : patch(NULL), effect(NULL) { }

	int texture;
	int effectIndex;
	int type; 
	int vertex;
	int n_vertexes;
	int meshvert;
	int n_meshverts;
	int lm_index;
	int lm_start[2];
	int lm_size[2];
	Vec3 lm_origin;
	Vec2 lm_vecs[3];
	Vec3 normal;
	int size[2];
	Q3Patch* patch;
   Q3Effect* effect;
};


struct Q3Plane
{
	Vec3 normal;
	float distance;
};


struct Q3Node
{
	int planeIndex; 
	int children[2];  
	BoundingBox bbox;
};


struct Q3VisData
{
	int numVectors;  
	int vectorSize;
	unsigned char* vec;		// numVectors * vectorSize
};


struct Q3BrushSide
{
	int planeIndex;
	int textureIndex;
};


struct GeometryList
{
	int faces[10000];
	int meshes[10000];
	int patches[10000];
	int faceCount;
	int meshCount;
	int patchCount;

	Q3Map* mapData;
};


enum CollisionType { NoCollide = 1, BoxCollide = 2, SphereCollide = 3 };


#endif
