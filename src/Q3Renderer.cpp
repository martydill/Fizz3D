
// Q3Renderer.cpp
// Implementation of our Quake 3 rendering engine
// (c) 2002 Marty Dill

#include "Fizz3d.h"
#include "Q3Renderer.h"
#include "Log.h"


static const char* moduleName = "Q3Renderer";


// Standard constructor
Q3Renderer::Q3Renderer()
{
	visibleGeometryList = new GeometryList();
}


// Standard destructor
Q3Renderer::~Q3Renderer()
{
	delete visibleGeometryList;
}


// Sets up the camera
int Q3Renderer::applyCamera(Camera* pCamera)
{
	Assert(pCamera != NULL);
	
	this->pCamera = pCamera;
	return 0;
}

int visIndex = 0;

// Renders the given map
GeometryList* Q3Renderer::renderMap(Map* pMap, std::vector<Entity*>& entityList)
{
	Assert(pMap != NULL);
	
	Vec3 eyePos = pCamera->getPosition();
	qmap = static_cast< Q3Map* > (pMap);
	
	// Initialize the visible face list
	visibleGeometryList->faceCount = 0;
	visibleGeometryList->meshCount = 0;
	visibleGeometryList->patchCount = 0;
	visibleGeometryList->mapData = qmap;
	
	pNode = qmap->pNode;
	pLeaf = qmap->pLeaf;

	// Clear the 'already in list' array
	memset(faces, 0, 10000 * sizeof(int));

	eyeCluster = findCluster(&eyePos);
	
	// Fix which lets us see stuff if we are outside the level
	if(eyeCluster == -1)
		eyeCluster = 1;

	generateViewFrustum();

	visIndex = eyeCluster * qmap->pVisData->vectorSize;
	walkBspTree(pNode);
	
	for(int i = 0; i < entityList.size(); ++i)
	{
		Entity* e = entityList.at(i);
		if(e->getFlags() & Entity::HasModel)
		{
			int entityCluster = this->findCluster(&e->getPosition());
			bool visible = isVisible(entityCluster);
			e->setVisible(visible);
		}
		else
		{
			e->setVisible(false);
		}
	}

	return visibleGeometryList;
}


bool Q3Renderer::isVisible(int clusterToTest)
{
	if(clusterToTest == -1)
		return false;

	return (qmap->pVisData->vec[visIndex + clusterToTest / 8] & (1 << clusterToTest % 8));
}


// Finds the cluster containing a given point
// FIXME: Rewrite this ...
int Q3Renderer::findCluster(Vec3* pos)
{
	Assert(pos != NULL);
	
	int cluster = -1;

	Q3Node* n = &pNode[0];
	
	while(1)
	{
		Q3Plane* p = &qmap->pPlane[n->planeIndex];

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

	return pLeaf[~cluster].cluster;
}


//std::stack< Q3Node* > nodeStack;

Q3Node* nodeStack[10000];
int nodeCount;


// Walks the BSP tree
int Q3Renderer::walkBspTree(Q3Node* p)
{
	if(p == NULL)
		return 0;

	nodeCount = 0;

	nodeStack[++nodeCount] = p;

	while(nodeCount > 0)
	{
		Q3Node* node = nodeStack[nodeCount--];

		if(node->children[0] > 0)
		{
			nodeStack[++nodeCount] = &pNode[node->children[0]];
		}
		else
		{
			addLeafToList(~ (node->children[0]) );
		}

		if(node->children[1] > 0)
		{
			nodeStack[++nodeCount] = &pNode[node->children[1]];
		}
		else
		{
			addLeafToList(~ (node->children[1]) );
		}
	}

/*
#ifdef STACK

	nodeStack.push(p);

	while(!nodeStack.empty())
	{
		Q3Node* node = nodeStack.top();
		nodeStack.pop();

		if(node->children[0] > 0)
		{
			nodeStack.push(&pNode[node->children[0]]);
		}
		else
		{
			addLeafToList(~ (node->children[0]) );
		}

		if(node->children[1] > 0)
		{
			nodeStack.push(&pNode[node->children[1]]);
		}
		else
		{
			addLeafToList(~ (node->children[1]) );
		}
	}

#else
		
	if(p->children[0] > 0)
	{
		walkBspTree(&pNode[p->children[0]]);
	}
	else
	{
		addLeafToList(~ (p->children[0]) );
	}

	if(p->children[1] > 0 )
	{stack
		walkBspTree(&pNode[p->children[1]]);
	}
	else
	{
		addLeafToList(~ (p->children[1]) );
	}


#endif
*/
	return 0;
}


// Adds a leaf to the list of polygons to draw
// This function is critical to performance of the engine, as it is called ~2000 times per frame.
// Optimize me!
int Q3Renderer::addLeafToList(int leafIndex)
{
	//// This cluster is invalid
	//if(pLeaf[leafIndex].cluster == -1)
	//	return 1;
	//
	//// This cluster isn't visible
	//if(!(qmap->pVisData->vec[visIndex + pLeaf[leafIndex].cluster / 8] & (1 << pLeaf[leafIndex].cluster % 8)))
	//	return 1;

	if(!isVisible(pLeaf[leafIndex].cluster))
		return 1;

	// Test if the leaf is in the view frustum
	if(!leafIsInViewFrustum(leafIndex))
		return 1;

	// For each face ...
	for(int k = qmap->pLeaf[leafIndex].leafFace; k < qmap->pLeaf[leafIndex].leafFace + qmap->pLeaf[leafIndex].numLeafFaces; ++k)
	{
		int i = qmap->pLeafFace[k].face;
	
		// If it has already beeh added, continue
		if(faces[i] == 1)
			continue;
		else
			faces[i] = 1;

		// Add it to the list
		if(qmap->pFace[i].type == Q3FaceType::Poly)
			visibleGeometryList->faces[visibleGeometryList->faceCount++] = i;
		
		else if(qmap->pFace[i].type == Q3FaceType::Mesh)
			visibleGeometryList->meshes[visibleGeometryList->meshCount++] = i;
	
		else if(qmap->pFace[i].type == Q3FaceType::Patch)
			visibleGeometryList->patches[visibleGeometryList->patchCount++] = i;
	}

	return 0;
}


// Generates the view frustum
// FIXME FIXME FIXME FIXME D3D FRUSTUM CULLING
int Q3Renderer::generateViewFrustum(void)
{
	float modl[16];
	float proj[16];
	float clip[16];

	/* Get the current PROJECTION matrix from OpenGL */
	glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*)proj );

	/* Get the current MODELVIEW matrix from OpenGL */
	glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*)modl);

	/* Combine the two matrices (multiply projection by modelview) */
	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	/* Extract the numbers for the RIGHT plane */
	frustum[0].normal.x = clip[ 3] - clip[ 0];
	frustum[0].normal.y = clip[ 7] - clip[ 4];
	frustum[0].normal.z = clip[11] - clip[ 8];
	frustum[0].distance = clip[15] - clip[12];
	
	/* Extract the numbers for the LEFT plane */
	frustum[1].normal.x = clip[ 3] + clip[ 0];
	frustum[1].normal.y = clip[ 7] + clip[ 4];
	frustum[1].normal.z = clip[11] + clip[ 8];
	frustum[1].distance = clip[15] + clip[12];

	/* Extract the BOTTOM plane */
	frustum[2].normal.x = clip[ 3] + clip[ 1];
	frustum[2].normal.y = clip[ 7] + clip[ 5];
	frustum[2].normal.z = clip[11] + clip[ 9];
	frustum[2].distance = clip[15] + clip[13];
	
	/* Extract the TOP plane */
	frustum[3].normal.x = clip[ 3] - clip[ 1];
	frustum[3].normal.y = clip[ 7] - clip[ 5];
	frustum[3].normal.z = clip[11] - clip[ 9];
	frustum[3].distance = clip[15] - clip[13];

	/* Extract the FAR plane */
	frustum[4].normal.x = clip[ 3] - clip[ 2];
	frustum[4].normal.y = clip[ 7] - clip[ 6];
	frustum[4].normal.z = clip[11] - clip[10];
	frustum[4].distance = clip[15] - clip[14];
	
	/* Extract the NEAR plane */
	frustum[5].normal.x = clip[ 3] + clip[ 2];
	frustum[5].normal.y = clip[ 7] + clip[ 6];
	frustum[5].normal.z = clip[11] + clip[10];
	frustum[5].distance = clip[15] + clip[14];

	return 0;
}


// Returns true if a given leaf is within the view frustum
// FIXME: Rewrite this?
bool Q3Renderer::leafIsInViewFrustum(int leafIndex)
{
	Q3Leaf* leaf = &pLeaf[leafIndex];
	
	// Test against all 6 clipping planes
	for(int i = 0; i < 6; ++i)
	{
		float min0timesNormalX = leaf->bbox.min[0] * frustum[i].normal.x;
		float max0timesNormalX = leaf->bbox.max[0] * frustum[i].normal.x;

		float min1timesNormalY = leaf->bbox.min[1] * frustum[i].normal.y;
		float max1timesNormalY = leaf->bbox.max[1] * frustum[i].normal.y;

		float min2timesNormalZ = leaf->bbox.min[2] * frustum[i].normal.z;
		float max2timesNormalZ = leaf->bbox.max[2] * frustum[i].normal.z;

		if(min0timesNormalX + min1timesNormalY
			+ min2timesNormalZ + frustum[i].distance > 0)
			continue;

		if(max0timesNormalX + max1timesNormalY
			+ max2timesNormalZ + frustum[i].distance > 0)
			continue;

		if(max0timesNormalX + min1timesNormalY
			+ min2timesNormalZ + frustum[i].distance > 0)
			continue;

		if(min0timesNormalX + max1timesNormalY
			+ min2timesNormalZ + frustum[i].distance > 0)
			continue;

		if(min0timesNormalX + min1timesNormalY
			+ max2timesNormalZ + frustum[i].distance > 0)
			continue;

		if(max0timesNormalX + max1timesNormalY
			+ min2timesNormalZ + frustum[i].distance > 0)
			continue;

		if(max0timesNormalX + min1timesNormalY
			+ max2timesNormalZ + frustum[i].distance > 0)
			continue;

		if(min0timesNormalX + max1timesNormalY
			+ max2timesNormalZ + frustum[i].distance > 0)
			continue;
	
		return false;
	}

	return true;
}
