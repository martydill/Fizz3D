
#include "Fizz3d.h"
#include "Engine.h"
#include "Q3Structs.h"
#include "Q3Map.h"
#include "Q3Shader.h"
#include "TextureFactory.h"
#include "Shader.h"

namespace ShaderUtils
{
	Q3Map* qmap;
	ITextureFactory* textureFactory;
	DWORD renderTime;

	void applyShader(Q3Shader* shader)
	{
		if(shader->cullEnabled)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(shader->cullFunc);
		}
		else
			glDisable(GL_CULL_FACE);
	}

	void applyShaderStage(Q3ShaderStage* shaderStage, int faceIndex)
	{	
		if(shaderStage->alphaEnabled)
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(shaderStage->alphaFunc, 0.5f);
		}
		else
			glDisable(GL_ALPHA_TEST);

		glDepthFunc(shaderStage->depthFunc);
		//
		//	if(shaderStage->srcBlend == -1 || shaderStage->destBlend == -1)
		//	{
		//		glDisable(GL_BLEND);
		//	}
		if(shaderStage->srcBlend == GL_ONE && shaderStage->destBlend == GL_ZERO)
		{
			glDisable(GL_BLEND);//
		}
		else
		{
			glBlendFunc(shaderStage->srcBlend, shaderStage->destBlend);
			glEnable(GL_BLEND);
		}

		if(shaderStage->textureName == "$lightmap")
		{
			unsigned int texture;
			if(faceIndex == -1)
			{
				unsigned char* data = new unsigned char[128*128*4];
				memset(data, 255, 128*128*4);
				texture = textureFactory->buildLightMap(data);
			}
			else
				texture = qmap->pLightMap[qmap->pFace[faceIndex].lm_index].textureId;

			textureFactory->applyTexture(texture, 0);
		}
		else
		{
			if(shaderStage->animMapTextureIds.size() == 0)
			{
				textureFactory->applyTexture(shaderStage->textureId, 0);
			}
			else
			{
				if(((float)renderTime - shaderStage->lastAnimFrameTime) / 1000 > (1 / shaderStage->animFrequency))
				{
					shaderStage->lastAnimFrame++;
					if(shaderStage->lastAnimFrame >= shaderStage->animMapTextureIds.size())
						shaderStage->lastAnimFrame = 0;

					shaderStage->lastAnimFrameTime = renderTime;
				}

				textureFactory->applyTexture(shaderStage->animMapTextureIds.at(shaderStage->lastAnimFrame), 0);
			}
		}
	}
};