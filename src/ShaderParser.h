
// ShaderParser.h
// Definitions for our shader parser
// (c) 2002 Marty Dill

#ifndef SHADERPARSER_H
#define SHADERPARSER_H


#include "Fizz3d.h"
#include "Q3Structs.h"
#include "Q3Shader.h"


struct Q3ShaderFile
{
	// The name of the shader, as it is listed in the script
	std::string shaderName;

	// The name of the shader's primary surface texture
	std::string textureName;
};


namespace ShaderParser
{
	// Master list of all shaders
	static std::map< std::string, Q3Shader* > shaderList;

	int loadShaders(void);
	int freeShaders(void);
	int extractShadersFromFile(std::ifstream& inFile);
	std::string getNextToken(char* buf);
	Q3Shader* mapShaderName(const char* name);
};



class Shader
{
public:
	static int assign(Q3Shader* p);

private:
	static bool shadersAreLoaded;
	static int loadShaders(void);
	static std::string getNextToken(std::ifstream* inFile);

	static std::vector< Q3ShaderFile > shaderList;
};



#endif
