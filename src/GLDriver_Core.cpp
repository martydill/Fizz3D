
// GLDriver.cpp
// Implementation of our OpenGL rendering device
// (c) 2002 Marty Dill

#include "Fizz3d.h"
#include "Engine.h"
#include "Q3Structs.h"
#include "Config.h"
#include "Q3Shader.h"
#include "Log.h"
#include <sstream>
#include "Math.h"
#include "Shader.h"

// FIXME >..
#include "GLDriver_Win32.h"

static const char* moduleName = "GLDriver_Core";

// Resize the viewport
int GLDriver::resize(int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Calculate perspective
	gluPerspective(60, static_cast< GLfloat > (newWidth) / newHeight, 10.0f, 2000.0f);
	glMatrixMode(GL_MODELVIEW);

	return 0;
}


Vec3 eye;
Vec3 up;
Vec3 ref;
Camera* camera;

// Applies the camera
int GLDriver::applyCamera(Camera* pCamera)
{	
	//this->pCamera = pCamera;
	// FIXME: camera updating?
	//pCamera->update();
	camera = pCamera;
	eye = pCamera->getPosition();
	pCamera->getUpVector(up);
	pCamera->getReferencePoint(ref);
	gluLookAt(eye.x, eye.y, eye.z, eye.x + ref.x, eye.y + ref.y, eye.z + ref.z , up.x, up.y, up.z);

	return 0;
}

Q3Map* qmap;
DWORD lastTime = 0;
DWORD renderTime = 0;

struct Q3Sort
{
	bool operator()(int& rpStart, int& rpEnd)
	{
		return (qmap->pLightMap[qmap->pFace[rpStart].lm_index].textureId > qmap->pLightMap[qmap->pFace[rpEnd].lm_index].textureId);
	}
};


int TransSort(const void* a, const void* b)
{
	int l = *(int*)a;
	int r = *(int*)b;
	//
	//   int lindex = qmap->pFace[l].vertex;
	//   int rindex = qmap->pFace[r].vertex;
	//
	//   int lmeshIndex = qmap->pFace[l].meshvert;
	//   int rmeshIndex = qmap->pFace[r].meshvert;
	//	
	//   int loffset = qmap->pMeshVert[lmeshIndex].offset;

	//   int roffset = qmap->pMeshVert[rmeshIndex].offset;
	//
	//   Vec3 lposition = qmap->pVertex[lindex + loffset].position;
	//   Vec3 rposition = qmap->pVertex[rindex + roffset].position;
	//
	//   float ldistance = sqrt(pow((lposition.x - eye.x), 2) + pow(lposition.y - eye.y, 2) + pow(lposition.z - eye.z, 2));
	//   float rdistance = sqrt(pow((rposition.x - eye.x), 2) + pow(rposition.y - eye.y, 2) + pow(rposition.z - eye.z, 2));
	//
	//if(ldistance < rdistance)
	//   return -1;
	//else
	//return 1;
	//   return ldistance < rdistance;

	Q3Shader* lShader = qmap->pMapShader[qmap->pFace[l].texture].pShader;
	Q3Shader* rShader = qmap->pMapShader[qmap->pFace[r].texture].pShader;

	if(lShader->getSortMode() < rShader->getSortMode())
		return 1;
	else if(rShader->getSortMode() < lShader->getSortMode())
		return -1;
	else
		return 0;
}


int GLDriver::drawFog(GeometryList* l)
{
	int count = 0;

	for(int j = 0; j < l->faceCount; ++j)
	{
		int i = l->faces[j];
		//qmap->pMapShader[qmap->pFace[i].texture].pShader;
		//if(qmap->pFace[i].effect != NULL)
		//if(shader->getSurfaceParam() & Fog)
		{
			if(qmap->pFace[i].effect != NULL)
			{
				Q3Shader* shader = qmap->pFace[i].effect->shader;
				if(!(shader->getSurfaceParam() & Fog))
					continue;

				float x1, x2, y1, y2, z1, z2;

				Q3Brush* brush = qmap->pFace[i].effect->brush;
				if(brush == NULL)
					continue;

				for(int m = brush->brushSideIndex; m < brush->brushSideIndex + brush->numBrushSides; ++m)
				{
					Q3BrushSide* brushSide = &qmap->pBrushSide[m];
					Q3Plane* plane = &qmap->pPlane[brushSide->planeIndex];

					for(int n = brush->brushSideIndex; n < brush->brushSideIndex + brush->numBrushSides; ++n)
					{
						Q3BrushSide* otherBrushSide = &qmap->pBrushSide[n];
						if(otherBrushSide != brushSide)
						{
							Q3Plane* otherPlane = &qmap->pPlane[otherBrushSide->planeIndex];
							if(otherPlane->normal.isOpposite(plane->normal))
							{
								if(plane->normal.x > 0)
								{
									x1 = plane->distance;
									x2 = otherPlane->distance;
								}
								else if(plane->normal.x < 0)
								{
									x2 = plane->distance;
									x1 = otherPlane->distance;
								}
								else if(plane->normal.y > 0)
								{
									y1 = plane->distance;
									y2 = otherPlane->distance;
								}
								else if(plane->normal.y < 0)
								{
									y2 = plane->distance;
									y1 = otherPlane->distance;
								}
								else if(plane->normal.z > 0)
								{
									z1 = plane->distance;
									z2 = otherPlane->distance;
								}
								else if(plane->normal.z < 0)
								{
									z2 = plane->distance;
									z1 = otherPlane->distance;
								}
							}
						}
					}
				}

				glDisable(GL_BLEND);
glEnable(GL_ALPHA_TEST);

glDepthFunc (GL_LEQUAL);						// The Type Of Depth Testing
	glEnable (GL_DEPTH_TEST);						// Enable Depth Testing
	glShadeModel (GL_SMOOTH);						// Select Smooth Shading
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	


	float fogColor[4] = {shader->fogParms.r, shader->fogParms.g, shader->fogParms.b, 0.5};
				glEnable(GL_FOG);							// Enable Fog
	glFogi(GL_FOG_MODE, GL_LINEAR);						// Fog Fade Is Linear
	glFogfv(GL_FOG_COLOR, fogColor);					// Set The Fog Color
	glFogf(GL_FOG_START,  0.2f);						// Set The Fog Start (Least Dense)
	glFogf(GL_FOG_END,    1.0f);						// Set The Fog End (Most Dense)
	glHint(GL_FOG_HINT, GL_NICEST);						// Per-Pixel Fog Calculation
	glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);		// Set Fog Based On Vertice Coordinates




				// Now build a list of points
				std::vector<Vec3> points;
				points.push_back(Vec3(x1, y1, z1));
				points.push_back(Vec3(x2, y1, z1));
				points.push_back(Vec3(x2, y2, z1));
				points.push_back(Vec3(x1, y2, z1));
				points.push_back(Vec3(x1, y2, z2));
				points.push_back(Vec3(x2, y2, z2));
				points.push_back(Vec3(x2, y1, z2));
				points.push_back(Vec3(x1, y1, z2));
				
				glDisable(GL_TEXTURE_2D);

				/*glPointSize(20);
				glBegin(GL_POINTS);
				for(int q = 0; q < points.size(); ++q)
				glVertex3f(points[q].x, points[q].y,  points[q].z);
				glEnd();
				continue;*/
				glBegin(GL_QUADS);													// Back Wall
	 	glFogCoordfEXT( 1.0f);	glVertex3f(x1,y1,z1);
		glFogCoordfEXT( 1.3f);		glVertex3f(x2,y1,z1);
		glFogCoordfEXT( 0.2f);	glVertex3f( x2, y2,z1);
		glFogCoordfEXT( 0.3f);	glVertex3f(x1, y2,z1);
	glEnd();

	glBegin(GL_QUADS);													// Floor
	 	glFogCoordfEXT( 1.0f);	glVertex3f(x1,y1,z1);
		glFogCoordfEXT( 1.0f);	glVertex3f( x2,y1,z1);
		glFogCoordfEXT( 1.2f);	glVertex3f( x2,y1, z2);
		glFogCoordfEXT( 1.2f);	glVertex3f(x1,y1, z2);
	glEnd();

	glBegin(GL_QUADS);													// Roof
		glFogCoordfEXT( 1.0f);		glVertex3f(x1, y2,z1);
		glFogCoordfEXT( 1.4f);		glVertex3f( x2, y2,z1);
		glFogCoordfEXT( 1.0f);		glVertex3f( x2, y2, z2);
		glFogCoordfEXT( 1.2f);	glVertex3f(x1, y2, z2);
	glEnd();

	glBegin(GL_QUADS);													// Right Wall
		glFogCoordfEXT( 0.2f);	glVertex3f( x2,y1, z2);
		glFogCoordfEXT( 1.0f);	glVertex3f( x2, y2, z2);
		glFogCoordfEXT( 1.8f);		glVertex3f( x2, y2,z1);
		glFogCoordfEXT( 1.6f);		glVertex3f( x2,y1,z1);
	glEnd();

	glBegin(GL_QUADS);													// Left Wall
	 	glFogCoordfEXT( 1.0f);		glVertex3f(x1,y1, z2);
		glFogCoordfEXT( 1.0f);	glVertex3f(x1, y2, z2);
		glFogCoordfEXT( 1.4f);	glVertex3f(x1, y2,z1);
		glFogCoordfEXT( .3f);	glVertex3f(x1,y1,z1);
	glEnd();
 
			}
		}
	}

	glDisable(GL_FOG);
	return 0;
}


int GLDriver::drawSky(GeometryList *l, float timeDiff)
{
   //// prepare for skybox
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glDepthMask(GL_FALSE);
   //gluLookAt(
   //	0, 0, 0,
   //	camera->getPosition().x, camera->getPosition().y, camera->getPosition().z,
   //0,0,0,
   //	up.x, up.y, up.z);

   //glRotatef(camera->getAzimuth(), 0, 0, 1);
   //eye.x, eye.y, eye.z,
   //0,0,1);

   glTranslatef( 0.0, -1.0, 0.0);

   // Enable/Disable features
   //glPushAttrib(GL_ENABLE_BIT);
   glEnable(GL_TEXTURE_2D);
   glDisable(GL_DEPTH_TEST);
   //GLState::Disable(GL_DEPTH_TEST);
   //		glDisable(GL_LIGHTING);
   //GLState::Disable(GL_BLEND);
   glScalef(100, 100, 100);
   //// draw sky
   for(int j = 0; j < l->faceCount; ++j)
   {
      int i = l->faces[j];
      Q3Shader* shader = qmap->pMapShader[qmap->pFace[i].texture].pShader;

      if(shader->getSurfaceParam() & Sky)
      {
		  ShaderUtils::applyShader(shader);

         for(int stage = 1; stage <= shader->getNumberOfStages(); ++stage)
         {
            Q3ShaderStage* shaderStage = shader->getShaderStage(stage);
			ShaderUtils::applyShaderStage(shaderStage, i);
            int index = qmap->pFace[i].vertex;
            int meshIndex = qmap->pFace[i].meshvert;

            shaderStage->lastTcModRotate += shaderStage->tcModRotate * timeDiff / 1000;
            if(shaderStage->lastTcModRotate > 360)
               shaderStage->lastTcModRotate -= 360;

            // Set up vertex data

            shaderStage->lastTcModScrollS += shaderStage->tcModScrollSspeed * timeDiff / 1000;
            shaderStage->lastTcModScrollT += shaderStage->tcModScrollTspeed * timeDiff / 1000;


            int offset = qmap->pMeshVert[meshIndex + 0].offset;
            offset = qmap->pMeshVert[meshIndex].offset;

            float tx = 0.1;/*qmap->pVertex[index + offset].texCoord.x*/ //* shaderStage->tcModScaleS;
            float ty = 0.1;/* qmap->pVertex[index + offset].texCoord.y*/ //* shaderStage->tcModScaleT;

            if(shaderStage->textureName == "$lightmap")
            {
               tx = qmap->pVertex[index + offset].lmCoord.x * shaderStage->tcModScaleS;
               ty =  qmap->pVertex[index + offset].lmCoord.y * shaderStage->tcModScaleT;
            }

            if(abs(shaderStage->tcModScrollSspeed - -1) > 0.0001)
               tx = tx + shaderStage->lastTcModScrollS;//shaderStage->tcModScrollSspeed + timeDiff / 1000;
            if(abs(shaderStage->tcModScrollTspeed - -1) > 0.0001)
               ty = ty + shaderStage->lastTcModScrollT;//	ty = ty + f;//shaderStage->tcModScrollTspeed + timeDiff / 1000;

            if(abs(shaderStage->tcModRotate - -1) > 0.0001)
            {
               // rotate around center of texture
               if(shaderStage->textureId != -1 && shaderStage->textureId != 0)
               {
                  float xCenter = 0.5;
                  float yCenter = 0.5;

                  float radians = Math::degreesToRadians(shaderStage->lastTcModRotate);// + f * 100);

                  float oldX = tx;
                  float oldY = ty;
                  tx = xCenter+ ( cos(radians) * (oldX - xCenter) - sin(radians) * (oldY - yCenter) );
                  ty = yCenter + ( sin(radians) * (oldX - xCenter) + cos(radians) * (oldY- yCenter) );
               }
            }

            glBegin(GL_QUADS);
            glTexCoord2f(0 + tx, 0 + ty); glVertex3f(  0.5f, -0.5f, -0.5f );
            glTexCoord2f(1 + tx, 0 + ty); glVertex3f( -0.5f, -0.5f, -0.5f );
            glTexCoord2f(1 + tx, 1 + ty); glVertex3f( -0.5f,  0.5f, -0.5f );
            glTexCoord2f(0 + tx, 1 + ty); glVertex3f(  0.5f,  0.5f, -0.5f );
            glEnd();

            //// Render the left quad
            //// glBindTexture(GL_TEXTURE_2D, _skybox[1]);
            //glBegin(GL_QUADS);
            //glTexCoord2f(0 + tx, 0 + ty); glVertex3f(  0.5f, -0.5f,  0.5f );
            //glTexCoord2f(1 + tx, 0 + ty); glVertex3f(  0.5f, -0.5f, -0.5f );
            //glTexCoord2f(1 + tx, 1 + ty); glVertex3f(  0.5f,  0.5f, -0.5f );
            //glTexCoord2f(0 + tx, 1 + ty); glVertex3f(  0.5f,  0.5f,  0.5f );
            //glEnd();

            //// Render the back quad
            // glBindTexture(GL_TEXTURE_2D, _skybox[2]);
            /*glBegin(GL_QUADS);
            glTexCoord2f(0 + tx, 0 + ty); glVertex3f( -0.5f, -0.5f,  0.5f );
            glTexCoord2f(1 + tx, 0 + ty); glVertex3f(  0.5f, -0.5f,  0.5f );
            glTexCoord2f(1 + tx, 1 + ty); glVertex3f(  0.5f,  0.5f,  0.5f );
            glTexCoord2f(0 + tx, 1 + ty); glVertex3f( -0.5f,  0.5f,  0.5f );

            glEnd();*/

            //// Render the right quad
            //// glBindTexture(GL_TEXTURE_2D, _skybox[3]);
            //glBegin(GL_QUADS);
            //glTexCoord2f(0 + tx, 0 + ty); glVertex3f( -0.5f, -0.5f, -0.5f );
            //glTexCoord2f(1 + tx, 0  +ty); glVertex3f( -0.5f, -0.5f,  0.5f );
            //glTexCoord2f(1 + tx, 1 + ty); glVertex3f( -0.5f,  0.5f,  0.5f );
            //glTexCoord2f(0 + tx, 1 + ty); glVertex3f( -0.5f,  0.5f, -0.5f );
            //glEnd();

            // Render the top quad
            //glBindTexture(GL_TEXTURE_2D, _skybox[4]);
            //glBegin(GL_QUADS);
            //glTexCoord2f(0+ tx, 0 + ty); glVertex3f( -0.5f,  0.5f, -0.5f );
            //glTexCoord2f(1 + tx, 0 + ty); glVertex3f( -0.5f,  0.5f,  0.5f );
            //glTexCoord2f(1 + tx, 1 + ty); glVertex3f(  0.5f,  0.5f,  0.5f );
            //glTexCoord2f(0 + tx, 1 + ty); glVertex3f(  0.5f,  0.5f, -0.5f );
            //glEnd();

            // Render the bottom quad
            ////glBindTexture(GL_TEXTURE_2D, _skybox[5]);
            //glBegin(GL_QUADS);
            //glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
            //glTexCoord2f(0, 1); glVertex3f( -0.5f, -0.5f,  0.5f );
            //glTexCoord2f(1, 1); glVertex3f(  0.5f, -0.5f,  0.5f );
            //glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
            //glEnd();
         }

         break;
      }
   }

   return 0;
}


// Renders the requested list of faces
// Normal faces are rendered with vertex arrays and multitexturing. The first TMU handles the 
// lightmap, while the second TMU handles the surface texture itself.
int GLDriver::drawGeometryList(GeometryList* l)
{
	Assert(l != NULL);
	if(lastTime == 0)
		lastTime = timeGetTime();
	renderTime = timeGetTime();
	qmap = l->mapData;

	ShaderUtils::renderTime = renderTime;
	ShaderUtils::qmap = qmap;
	ShaderUtils::textureFactory = textureFactory;
	qsort(l->faces, l->faceCount, sizeof(int), TransSort);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);

	glDepthFunc(GL_LEQUAL);

	float timeDiff = renderTime - lastTime;
    drawSky(l, timeDiff);
    drawFaces(l, timeDiff);
    drawMeshes(l, timeDiff);
    drawPatches(l);
    //drawFog(l);

	lastTime = renderTime;
	return 0;
}


int GLDriver::drawFaces(GeometryList *l, float timeDiff)
{
   // Draw faces (type 1)

   // Prepare for regular faces
   glLoadIdentity();
   applyCamera(camera);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);

   // Draw regular faces
   for(int j = 0; j < l->faceCount; ++j)
   {
      int i = l->faces[j];

      Q3Shader* shader = qmap->pMapShader[qmap->pFace[i].texture].pShader;

      if(shader->getSurfaceParam() & Sky)
         continue;

	  ShaderUtils::applyShader(shader);
	  std::vector<Vec3> points;

      for(int stage = 1; stage <= shader->getNumberOfStages(); ++stage)
      {
         Q3ShaderStage* shaderStage = shader->getShaderStage(stage);

		 ShaderUtils::applyShaderStage(shaderStage, i);

         int index = qmap->pFace[i].vertex;
         int meshIndex = qmap->pFace[i].meshvert;

         shaderStage->lastTcModRotate += shaderStage->tcModRotate * timeDiff / 1000;
         if(shaderStage->lastTcModRotate > 360)
            shaderStage->lastTcModRotate -= 360;

         // Set up vertex data

         // fixme - what if 2 faces are using the same shader?
         shaderStage->lastTcModScrollS += shaderStage->tcModScrollSspeed * timeDiff / 1000;
         shaderStage->lastTcModScrollT += shaderStage->tcModScrollTspeed * timeDiff / 1000;

         glBegin(GL_TRIANGLES);
         for(int currentMesh = 0; currentMesh < qmap->pFace[i].n_meshverts; ++currentMesh)
         {
            //int offset = //qmap->pMeshVert[meshIndex + 0].offset;
            int offset = qmap->pMeshVert[meshIndex + currentMesh].offset;

            glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

            //GLubyte colors[4];
            //memcpy(&colors, qmap->pVertex[index + offset].color, 4);
            /*if(shaderStage->rgbType == Identity)
            glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
            else if(shaderStage->rgbType == IdentityLighting)
            glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
            else if(shaderStage->rgbType == Vertex)
            glColor4ubv((const GLubyte*)&qmap->pVertex[index + offset]);
            else if(shaderStage->rgbType == ExactVertex)
            {	Trace("");}
            else if(shaderStage->rgbType == OneMinusVertex)
            {		Trace("");}
            else

            {
            Trace("");
            }*/
            // glColor4ubv((GLubyte*)&qmap->pVertex[index + offset].color)

            //if(shaderStage->alphaType == Identity)
            // ;//colors[3] = 0xff;
            //else
            //   Trace("");
            //else if(shaderStage->alphaType == OneMinusVertex)
            //  Trace("");
            //else
            //   colors[3] = colors[3];

            //glColor4ubv((const GLubyte*)&colors);

            float tx = qmap->pVertex[index + offset].texCoord.x * shaderStage->tcModScaleS;
            float ty =  qmap->pVertex[index + offset].texCoord.y * shaderStage->tcModScaleT;

            if(shaderStage->textureName == "$lightmap")
            {
               tx = qmap->pVertex[index + offset].lmCoord.x * shaderStage->tcModScaleS;
               ty =  qmap->pVertex[index + offset].lmCoord.y * shaderStage->tcModScaleT;
            }

            if(abs(shaderStage->tcModScrollSspeed - -1) > 0.0001)
               tx = tx + shaderStage->lastTcModScrollS;//shaderStage->tcModScrollSspeed + timeDiff / 1000;
            if(abs(shaderStage->tcModScrollTspeed - -1) > 0.0001)
               ty = ty + shaderStage->lastTcModScrollT;//	ty = ty + f;//shaderStage->tcModScrollTspeed + timeDiff / 1000;

            if(abs(shaderStage->tcModRotate - -1) > 0.0001)
            {
               // rotate around center of texture
               if(shaderStage->textureId != -1 && shaderStage->textureId != 0)
               {
                  float xCenter = 0.5;
                  float yCenter = 0.5;

                  float radians = Math::degreesToRadians(shaderStage->lastTcModRotate);// + f * 100);

                  float oldX = tx;
                  float oldY = ty;
                  tx = xCenter+ ( cos(radians) * (oldX - xCenter) - sin(radians) * (oldY - yCenter) );
                  ty = yCenter + ( sin(radians) * (oldX - xCenter) + cos(radians) * (oldY- yCenter) );
               }
            }

            glTexCoord2f(tx, ty);
            glVertex3f(qmap->pVertex[index + offset].position.x, qmap->pVertex[index + offset].position.y, qmap->pVertex[index + offset].position.z);
		 }

         glEnd();
      }
   }

   return 0;
}


int GLDriver::drawPatches(GeometryList *l)
{
   // Draw patches (type 3)
   
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glActiveTextureARB(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
   glDisable(GL_ALPHA_TEST);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_FRONT);
   glColor3f(1,1,1);

   // For each patch ...
   for(int j = 0; j < l->patchCount; ++j)
   {
      int i = l->patches[j];
      glCallList(qmap->pFace[i].patch->dispList);
   }

   //	glDisable(GL_TEXTURE_2D);
   //	glClientActiveTextureARB(GL_TEXTURE0_ARB);
   //	glActiveTextureARB(GL_TEXTURE0_ARB);

   glActiveTextureARB(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_2D);

   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   return 0;
}


int GLDriver::drawMeshes(GeometryList *l, float timeDiff)
{
   // Draw meshes (type 2)

   // Set up ...
   //glDisable(GL_BLEND);
   //glDisable(GL_ALPHA_TEST);
   //glDepthFunc(GL_LEQUAL);

   // For each mesh ...
   for(int j = 0; j < l->meshCount; ++j)
   {
      int i = l->meshes[j];

      int index = qmap->pFace[i].vertex;
      int meshIndex = qmap->pFace[i].meshvert;
      int offset;

      // Apply the mesh texture
      Q3Shader* shader = qmap->pMapShader[qmap->pFace[i].texture].pShader;
	  ShaderUtils::applyShader(shader);

      for(int stage = 1; stage <= shader->getNumberOfStages(); ++stage)
      {
         Q3ShaderStage* shaderStage = shader->getShaderStage(stage);
		 ShaderUtils::applyShaderStage(shaderStage, i);

         shaderStage->lastTcModScrollS += shaderStage->tcModScrollSspeed * timeDiff / 1000;
         shaderStage->lastTcModScrollT += shaderStage->tcModScrollTspeed * timeDiff / 1000;

         glBegin(GL_TRIANGLES);

         // For each vertex in the mesh ...
         for(int currentMesh = 0; currentMesh < qmap->pFace[i].n_meshverts; ++currentMesh)
         {
            offset = qmap->pMeshVert[meshIndex + currentMesh].offset;

            if(shaderStage->rgbType == Identity)
               glColor4ub(0xff, 0xff, 0xff, 0xff);
            else if(shaderStage->rgbType == Vertex)
               glColor4ubv((const GLubyte*)&qmap->pVertex[index + offset].color);
            else
               glColor3f(1, 1, 1);
            //glColor3ub(qmap->pVertex[index + offset].color[0], qmap->pVertex[index + offset].color[1], qmap->pVertex[index + offset].color[2]);

            float tx = qmap->pVertex[index + offset].texCoord.x * shaderStage->tcModScaleS;
            float ty =  qmap->pVertex[index + offset].texCoord.y * shaderStage->tcModScaleT;

            if(abs(shaderStage->tcModScrollSspeed - -1) > 0.0001)
               tx = tx + shaderStage->lastTcModScrollS;//shaderStage->tcModScrollSspeed + timeDiff / 1000;
            if(abs(shaderStage->tcModScrollTspeed - -1) > 0.0001)
               ty = ty + shaderStage->lastTcModScrollT;//	ty = ty + f;//shaderStage->tcModScrollTspeed + timeDiff / 1000;

            if(abs(shaderStage->tcModRotate - -1) > 0.0001)
            {
               // rotate around center of texture
               if(shaderStage->textureId != -1 && shaderStage->textureId != 0)
               {
                  float xCenter = 0.5;
                  float yCenter = 0.5;

                  float radians = Math::degreesToRadians(shaderStage->lastTcModRotate);// + f * 100);

                  float oldX = tx;
                  float oldY = ty;
                  tx = xCenter+ ( cos(radians) * (oldX - xCenter) - sin(radians) * (oldY - yCenter) );
                  ty = yCenter + ( sin(radians) * (oldX - xCenter) + cos(radians) * (oldY- yCenter) );
               }
            }

            glTexCoord2f(tx, ty);

            glVertex3f(qmap->pVertex[index + offset].position.x, qmap->pVertex[index + offset].position.y, qmap->pVertex[index + offset].position.z);
         }

         glEnd();
      }
   }

   return 0;
}


// Creates a GLTextureFactory object
ITextureFactory* GLDriver::createTextureFactory(void)
{
	textureFactory = new GLTextureFactory();
	return textureFactory;
}


// Displays text on the screen
// FIXME: Work properly at any resolution ...
// FIXME: This isn't portable ...
int GLDriver::drawText(int xPos, int yPos, string msg)
{
	glLoadIdentity();
	glTranslatef(0,0,-10.1f);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
	glRasterPos2f(-7.2f + (static_cast< float > (xPos) / Config::getIntValue("Width") * 14.4f),
		5.2f - (static_cast< float > (yPos) / Config::getIntValue("Height") * 10.4f));
	glPushAttrib(GL_LIST_BIT);
	glListBase(fontDispListBase - 32);
	glCallLists(strlen(msg.c_str()), GL_UNSIGNED_BYTE, msg.c_str());
	glPopAttrib();	

	return 0;
}


Q3Face* pFace;
Q3Vertex* pVertex;
Q3MapShader* pMapShader;
Q3LightMap* pLightMap;
GLuint minList = FONT_DISPLAY_LIST_SIZE, maxList = 0;

// Processing we do on the map after it has been loaded
// For the OpenGL driver, we generate display lists from the bezier patches.
int GLDriver::postProcessMap(Q3Map* mapData)
{
	Assert(mapData != NULL);

	pFace = mapData->pFace;
	pVertex = mapData->pVertex;
	//pShader = mapData->pShader;
	pMapShader = mapData->pMapShader;
	pLightMap = mapData->pLightMap;
		
	if(maxList > FONT_DISPLAY_LIST_SIZE)
		glDeleteLists(minList, maxList);

	for(int i = 0; i < mapData->numFaces; ++i)
	{
		if(mapData->pFace[i].type == Q3FaceType::Patch)
		{
			// FIXME: here or map?
			computePatch(i);
			genDisplayList(i);
		}
	}

	return 0;
}



// Private functions



// Generate a single patch.
// Breaks the m x n control point matrix into a series of 3x3 matricies, then tesselates them individually
// Could probably use some optimization, but hey, this isn't a critical path
int GLDriver::computePatch(int index)
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
int GLDriver::genDisplayList(int i)
{
	pFace[i].patch->dispList = glGenLists(1);

	glNewList(pFace[i].patch->dispList, GL_COMPILE);
	//if(pFace[i].patch->dispList < minList)
	//	minList = pFace[i].patch->dispList;
	if(pFace[i].patch->dispList > maxList)
		maxList = pFace[i].patch->dispList;

	// For each section of the patch ...
	for(int n = 0; n < pFace[i].patch->sections; ++n)
	{
		Q3Vertex* pv = pFace[i].patch->vertices[n];
		int numSteps = pFace[i].patch->numSteps;

		// Set up first TMU				
		glActiveTextureARB(GL_TEXTURE0_ARB);
		//textureFactory->applyTexture(pLightMap[pFace[i].lm_index].textureId, 0);
		//textureFactory->applyTexture(pLightMap[pFace[i].lm_index].textureId, 0);
		glBindTexture(GL_TEXTURE_2D, pLightMap[pFace[i].lm_index].textureId);	

		// Set up second TMU
		glActiveTextureARB(GL_TEXTURE1_ARB);
		//textureFactory->applyTexture(pShader[pFace[i].texture].textureId, 1);
		//textureFactory->applyTexture(pShader[pFace[i].texture].textureId, 1);

		Q3Shader* shader = pMapShader[pFace[i].texture].pShader;
		for(int stage = 1; stage <= shader->getNumberOfStages(); ++stage)
		{
			Q3ShaderStage* shaderStage = shader->getShaderStage(stage);
			if(shaderStage->textureName != "$lightmap")
				glBindTexture(GL_TEXTURE_2D, shaderStage->textureId);
		}
		//textureFactory->applyTexture(shader->getShaderStage(shader->getNumberOfStages())->textureId, 0);

		//		glBindTexture(GL_TEXTURE_2D, pShader[pFace[i].texture].textureId); fixme

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
	}

	glEndList();

	return 0;
}
