
// Q3Renderer.h
// Defines the rendering engine used for Quake 3 maps
// (c) 2002 Marty Dill

#ifndef Q3RENDERER_H
#define Q3RENDERER_H


#include "Renderer.h"
#include "Q3Structs.h"
#include "Q3Map.h"


class Q3Renderer : public IRenderer
{
public:
	Q3Renderer();
	virtual ~Q3Renderer();

	// IRenderer's pure virtual methods	
	int applyCamera(Camera* pCamera);
	GeometryList* renderMap(Map* pMap, std::vector<Entity*>& entityList);
	
	// Q3-specific methods
	int findCluster(Vec3* pos);
	int walkBspTree(Q3Node* p);
	int addLeafToList(int index);
	bool leafIsInViewFrustum(int leafIndex);
	int generateViewFrustum(void);
	bool isVisible(int clusterToTest);

private:
	Q3Node* pNode;
	Q3Leaf* pLeaf;
	Q3Map* qmap;
	int eyeCluster;
	int faces[10000];
	Q3Plane frustum[6];

	GeometryList* visibleGeometryList;
};



#endif
