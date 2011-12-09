
#ifndef SHADER_H
#define SHADER_H

namespace ShaderUtils
{
	extern Q3Map* qmap;
	extern ITextureFactory* textureFactory;
	extern DWORD renderTime;

	void applyShader(Q3Shader* shader);
	void applyShaderStage(Q3ShaderStage* shaderStage, int faceIndex);
};

#endif
