
// MD3Model.cpp
// Class for loading/rendering Quake 3's .MD3 models
// (c) 2002 Marty Dill
// FIXME: d3d!!

#include "Fizz3d.h"
#include "MD3Model.h"
#include "Log.h"
#include "Config.h"
#include <IL\il.h>
#include "ShaderParser.h"
#include "Shader.h"
#include "Math.h"

const char* moduleName = "MD3Loader";


ITextureFactory* MD3Model::textureFactory;

// Default constructor
MD3Model::MD3Model()
{
}


// Default destructor
MD3Model::~MD3Model()
{
}


// Writes information about the model to the stream passed to it
int MD3Model::writeModelInfo(ofstream* out, md3_data* model)
{
	Assert(out != NULL);
	Assert(model != NULL);
	
	// Output debug information about the file
	*out << "Model Info: " << endl;
	*out << "File format ID: " << model->pHeader->id[0] << model->pHeader->id[1] << model->pHeader->id[2] << model->pHeader->id[3] << endl;
	*out << "File name: " << model->pHeader->fileName << endl;
	*out << "File version: " << model->pHeader->version << endl;
	*out << "Number of bone frames: " << model->pHeader->numBoneFrames << endl;
	*out << "Number of tags: " << model->pHeader->numTags << endl;
	*out << "Number of meshes: " << model->pHeader->numMeshes << endl;
	*out << "Max. number of skins: " << model->pHeader->numMaxSkins << endl;
	*out << "Header length: " << model->pHeader->headerLength << endl;
	*out << "Tag start: " << model->pHeader->tagStart << endl;
	*out << "Tag end: " << model->pHeader->tagEnd << endl;
	*out << "File size: " << model->pHeader->size << endl;
	*out << endl;

	for(int i = 0; i < model->pHeader->numTags; ++i)
	{
		*out << model->pTag[i].name << endl;
		*out << model->pMesh[i].name << endl;
	}

	out->flush();

	return 0;
}


// Loads the requested model
int MD3Model::load(string modelName)
{
	return 0;
}


// Loads the requested texture
int MD3Model::loadTextures(md3_data* model)
{
	Assert(model != NULL);
	
	for(int i = 0; i < model->pHeader->numMeshes; ++i)
	{	
		ILuint textureId;

		// Generate the texture space
		ilGenImages(1, &textureId);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		ilBindImage(textureId);
		ilLoadImage((char*)model->pMesh[i].textureName.c_str());

		model->pMesh[i].textureTag = ilutGLBindTexImage();

		// Generate mipmaps
		if(Config::getBoolValue("UseMipmaps"))
			ilutGLBuildMipmaps();

		ilDeleteImages(1, &textureId);
	}
	return 0;
}


// Loads the requested .skin file
int MD3Model::loadSkin(md3_data* model, string skinName)
{
	string data;
	ifstream skinFile;
	
	Assert(model != NULL);

	skinFile.open(skinName.c_str(), ios::in);
	
	// While the skin file has data ...
	while(!skinFile.eof())
	{
		skinFile >> data;
	
		// Loop through the model's meshes and check if data contains the name of a texture
		// for this particular mesh
		for(int i = 0; i < model->pHeader->numMeshes; ++i)
		{
			if(data.find(model->pMesh[i].name) != string::npos)
			{
				// We found it, so grab the texture's name and store it, along with the model's name
				int s = data.find_last_of('/');
				model->pMesh[i].textureName = name + data.substr(s, data.length());
			}
		}
	}

	skinFile.close();

	return 0;
}


// Reads data from a single .md3 file
int MD3Model::readData(FILE* file, md3_data* md3Data)
{
	Assert(file != NULL);
	Assert(md3Data != NULL);
		
	md3Data->pHeader = new md3_header;
	md3Data->currentFrame = 0;

	// Read the file's header	
	fread(md3Data->pHeader->id, 1, 4, file);
	fread(&md3Data->pHeader->version, 4, 1, file);
	fread(&md3Data->pHeader->fileName, 1, 68, file);
	fread(&md3Data->pHeader->numBoneFrames, 4, 1, file);
	fread(&md3Data->pHeader->numTags, 4, 1, file);
	fread(&md3Data->pHeader->numMeshes, 4, 1, file);
	fread(&md3Data->pHeader->numMaxSkins, 4, 1, file);
	fread(&md3Data->pHeader->headerLength, 4, 1, file);
	fread(&md3Data->pHeader->tagStart, 4, 1, file);
	fread(&md3Data->pHeader->tagEnd, 4, 1, file);
	fread(&md3Data->pHeader->size, 4, 1, file);

	// Create our objects
	md3Data->pBoneFrame = new md3_boneframe[md3Data->pHeader->numBoneFrames];
	md3Data->pTag = new md3_tag[md3Data->pHeader->numTags * md3Data->pHeader->numBoneFrames];
	md3Data->pMesh = new md3_mesh[md3Data->pHeader->numMeshes+100];
	
	// Jump to the start of the tag section
	fseek(file, md3Data->pHeader->tagStart, SEEK_SET);

	// Read the tag data
	for(int i = 0; i < md3Data->pHeader->numTags * md3Data->pHeader->numBoneFrames; ++i)
	{
		float z[3][3];
		fread(md3Data->pTag[i].name, 64, 1, file);
		fread(md3Data->pTag[i].position, 4, 3, file);
		fread(z, 4, 9, file);

		// Construct the tag's 4x4 rotation matrix
		md3Data->pTag[i].rotation[0][0] = z[0][0];
		md3Data->pTag[i].rotation[0][1] = z[0][1];
		md3Data->pTag[i].rotation[0][2] = z[0][2];
		md3Data->pTag[i].rotation[0][3] = 0;
			
		md3Data->pTag[i].rotation[1][0] = z[1][0];
		md3Data->pTag[i].rotation[1][1] = z[1][1];
		md3Data->pTag[i].rotation[1][2] = z[1][2];
		md3Data->pTag[i].rotation[1][3] = 0;

		md3Data->pTag[i].rotation[2][0] = z[2][0];
		md3Data->pTag[i].rotation[2][1] = z[2][1];
		md3Data->pTag[i].rotation[2][2] = z[2][2];
		md3Data->pTag[i].rotation[2][3] = 0;

		md3Data->pTag[i].rotation[3][0] = 0;
		md3Data->pTag[i].rotation[3][1] = 0;
		md3Data->pTag[i].rotation[3][2] = 0;
		md3Data->pTag[i].rotation[3][3] = 1;
	}

	// Create and initialize the link data
	md3Data->links = new md3_data*[md3Data->pHeader->numTags];

	for(int i = 0; i < md3Data->pHeader->numTags; ++i)
		md3Data->links[i] = NULL;
	
	// Read the bone frame data
	for(int i = 0; i < md3Data->pHeader->numBoneFrames; ++i)
	{
		fread(md3Data->pBoneFrame[i].mins, 4, 3, file);
		fread(md3Data->pBoneFrame[i].maxs, 4, 3, file);
		fread(md3Data->pBoneFrame[i].position, 4, 3, file);
		fread(&md3Data->pBoneFrame[i].scale, 4, 1, file);
		fread(md3Data->pBoneFrame[i].creator, 1, 16, file);
	}
	
	// Jump to the start of the mesh section
	fseek(file, md3Data->pHeader->tagEnd, SEEK_SET);

	// Read the mesh data
	for(int i = 0; i < md3Data->pHeader->numMeshes; ++i)
	{
		fread(md3Data->pMesh[i].id, 1, 4, file);
		fread(md3Data->pMesh[i].name, 1, 68, file);
		fread(&md3Data->pMesh[i].numMeshFrames, 4, 1, file);
		fread(&md3Data->pMesh[i].numSkins, 4, 1, file);
		fread(&md3Data->pMesh[i].numVerticies, 4, 1, file);
		fread(&md3Data->pMesh[i].numTriangles, 4, 1, file);
		fread(&md3Data->pMesh[i].triangleStart, 4, 1, file);
		fread(&md3Data->pMesh[i].headerSize, 4, 1, file);
		fread(&md3Data->pMesh[i].texVecStart, 4, 1, file);
		fread(&md3Data->pMesh[i].vertexStart, 4, 1, file);
		fread(&md3Data->pMesh[i].meshSize, 4, 1, file);

		// Create the mesh's objects
		md3Data->pMesh[i].pSkin = new md3_skin[md3Data->pMesh[i].numSkins];
		md3Data->pMesh[i].pTriangle = new md3_triangle[md3Data->pMesh[i].numTriangles];
		md3Data->pMesh[i].pUVtex = new md3_uvtex[md3Data->pMesh[i].numMeshFrames * md3Data->pMesh[i].numVerticies];
		md3Data->pMesh[i].pVertex = new md3_vertex[md3Data->pMesh[i].numMeshFrames * md3Data->pMesh[i].numVerticies];

		// Read the mesh's skin data
		for(int j = 0; j < md3Data->pMesh[i].numSkins; ++j)
		{
			fread(md3Data->pMesh[i].pSkin[j].name, 1, 68, file);

			int zzz = strlen(md3Data->pMesh[i].pSkin[j].name);
			if(zzz > 4)
			{
				char string[70];
				memset(string, 0, sizeof(string));
				strncpy(string, md3Data->pMesh[i].pSkin[j].name, zzz - 4);
			
				md3Data->pMesh[i].pShader = ShaderParser::mapShaderName(string);
				if(md3Data->pMesh[i].pShader != NULL)
				{
					md3Data->pMesh[i].pShader->loadTextures(textureFactory);
				}
			}
			//pMapShader[i].pShader = ShaderParser::mapShaderName(pMapShader[i].name);
		}

		// Read the mesh's triangle data
		for(int j = 0; j < md3Data->pMesh[i].numTriangles; ++j)
		{
			fread(md3Data->pMesh[i].pTriangle[j].triangle, 4, 3, file);
		}

		// Read the mesh's UV texture data
		for(int j = 0; j < md3Data->pMesh[i].numVerticies; ++j)
		{
			fread(md3Data->pMesh[i].pUVtex[j].texVec, 4, 2, file);
		}

		// Read the vertex data
		for(int j = 0; j < md3Data->pMesh[i].numMeshFrames * md3Data->pMesh[i].numVerticies; ++j)
		{
			fread(md3Data->pMesh[i].pVertex[j].vec, 2, 3, file);
			fread(md3Data->pMesh[i].pVertex[j].envTex, 1, 2, file);
			
			// Adjust the model's size
			md3Data->pMesh[i].pVertex[j].vec[0] /= 64;
			md3Data->pMesh[i].pVertex[j].vec[1] /= 64;
			md3Data->pMesh[i].pVertex[j].vec[2] /= 64;
		}
	}
	
	return 0;
}


// Loads the model's animation informatioin
int MD3Model::loadAnimations()
{
	ifstream animFile;
	string data;
	string fullPath = name + "/animation.cfg";
	vector<string> v;

	animFile.open(fullPath.c_str(), ios::in);

	// While there is data to read ...
	while(!animFile.eof())
	{
		animFile >> data;
		v.push_back(data);
		
		// We've reached the start of the wanted data ...
		if(data.find("BOTH_DEATH1") != string::npos)
		{
			v.pop_back();
			v.pop_back();
			v.pop_back();
			v.pop_back();
			animStart[0] = atoi(v.back().c_str());
			v.pop_back();
			animEnd[0] = animStart[0] + atoi(v.back().c_str());

			// Read in all of the animation data
			for(int i = 1; i < MAX_ANIMATIONS; ++i)
			{
				int j;
				animFile >> j;

				int k;
				animFile >> k;

				// Eat data until we reach the end of the line

				while(!animFile.eof() && animFile.get() != '\n')
				{
				}

				animStart[i] = j;
				animEnd[i] = j + k;
			}

			break;
		}
	}
	
	int diff = animStart[LEGS_WALKCR] - animStart[TORSO_GESTURE];

	// Adjust for torso vs. legs offset
	for(int i = LEGS_WALKCR; i < MAX_ANIMATIONS; ++i)
	{
		animStart[i] -= diff;
		animEnd[i] -= diff;
	}

	// Set starting animation
	setAnimation(BODY, TORSO_STAND);
	setAnimation(LEGS, LEGS_IDLECR);

	animFile.close();

	return 0;
}


// Attach a model to another - create a model heirarchy
int MD3Model::attach(md3_data* base, md3_data* attachment, char* tagName)
{
	Assert(base != NULL);
	Assert(attachment != NULL);
	Assert(tagName != NULL);
	
	for(int i = 0; i < base->pHeader->numTags; ++i)
	{
		// If the tags match, attach ...
		if(!strcmp(base->pTag[i].name, tagName))
		{
			base->links[i] = attachment;
		}
	}
	
	return 0;
}


// Draw the entire .md3 model 
int MD3Model::draw(void)
{
	return 0;
}


// Draws a single part of a model, and recursively draws all attached parts
int MD3Model::drawModel(md3_data* model)
{
	if(model == NULL)
		return -1;
	Assert(model != NULL);	
	//glEnable(GL_ALPHA_TEST);
	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	renderMesh(model);

	// For each tag in the model ...
	for(int i = 0; i < model->pHeader->numTags; ++i)
	{
		md3_data* link = model->links[i];
	
		// If the link exists, save the current state and follow the link
		if(link != NULL)
		{
			glPushMatrix();
	
			glTranslatef(
						model->pTag[model->currentFrame * model->pHeader->numTags + i].position[0],
						model->pTag[model->currentFrame * model->pHeader->numTags + i].position[1],
						model->pTag[model->currentFrame * model->pHeader->numTags + i].position[2]
						);
			
			glMultMatrixf(reinterpret_cast< float* > (model->pTag[model->currentFrame * model->pHeader->numTags + i].rotation));
			
			drawModel(link);
	
			glPopMatrix();
		}
	}

	return 0;
}


// Draws a single mesh
// FIXME: render to FaceLists ...
int MD3Model::renderMesh(md3_data* mesh)
{
	Assert(mesh != NULL);
	
	// For each component of the mesh ...
	for(int m = 0; m < mesh->pHeader->numMeshes; ++m)
	{
		int vertexIndex = mesh->currentFrame * mesh->pMesh[m].numVerticies;
			
		if(mesh->pMesh[m].pShader != NULL)
		{
			Q3Shader* shader = mesh->pMesh[m].pShader;
			ShaderUtils::applyShader(shader);

			for(int stage = 1; stage <= shader->getNumberOfStages(); ++stage)
			{
				Q3ShaderStage* shaderStage = shader->getShaderStage(stage);
				ShaderUtils::applyShaderStage(shaderStage, -1);

			//	shaderStage->lastTcModScrollS += shaderStage->tcModScrollSspeed * timeDiff / 1000;
			//	shaderStage->lastTcModScrollT += shaderStage->tcModScrollTspeed * timeDiff / 1000;

				glBegin(GL_TRIANGLES);
				for(int i = 0; i < mesh->pMesh[m].numTriangles; ++i)
				{	
					// For each vertex of the triangle ...
					for(int j = 0; j < 3; ++j)
					{
						int n = mesh->pMesh[m].pTriangle[i].triangle[j] + vertexIndex;
						int uvTexOffset = mesh->pMesh[m].pTriangle[i].triangle[j];

					    float tx = mesh->pMesh[m].pUVtex[uvTexOffset].texVec[0] * shaderStage->tcModScaleS;
						float ty = (mesh->pMesh[m].pUVtex[uvTexOffset].texVec[1]) * shaderStage->tcModScaleT;

						if(abs(shaderStage->tcModScrollSspeed - -1) > 0.0001)
						   tx = tx + shaderStage->lastTcModScrollS;//shaderStage->tcModScrollSspeed + timeDiff / 1000;
						if(abs(shaderStage->tcModScrollTspeed - -1) > 0.0001)
						   ty = ty + shaderStage->lastTcModScrollT;//	ty = ty + f;//shaderStage->tcModScrollTspeed + timeDiff / 1000;

						if(abs(shaderStage->tcModRotate - 1) > 0.0001)
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
						glVertex3sv((const GLshort*)&mesh->pMesh[m].pVertex[n].vec);
					}
				}
				glEnd();
			}
		}
		else
		{
			glDisable(GL_BLEND);
			glBindTexture(GL_TEXTURE_2D, mesh->pMesh[m].textureTag);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glBegin(GL_TRIANGLES);

			// For each triangle ...
			for(int i = 0; i < mesh->pMesh[m].numTriangles; ++i)
			{	
	    		// For each vertex of the triangle ...
				for(int j = 0; j < 3; ++j)
				{
					int n = mesh->pMesh[m].pTriangle[i].triangle[j] + vertexIndex;
					int uvTexOffset = mesh->pMesh[m].pTriangle[i].triangle[j];

					//glTexCoord2fv((const GLfloat*)&mesh->pMesh[m].pUVtex[uvTexOffset].texVec);
					glTexCoord2f(mesh->pMesh[m].pUVtex[uvTexOffset].texVec[0],
						1 - mesh->pMesh[m].pUVtex[uvTexOffset].texVec[1]);
					glVertex3sv((const GLshort*)&mesh->pMesh[m].pVertex[n].vec);
				}
			}

			glEnd();
		}
	}

	return 0;
}


// Sets the current animation
int MD3Model::setAnimation(int whichPart, int animation)
{
	// TODO
	return 0;
}
