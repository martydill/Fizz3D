
// ShaderParser.cpp
// The shader parser
// (c) 2002 Marty Dill

#include "Fizz3d.h"
#include "ShaderParser.h"
#include "Q3Shader.h"
#include "Engine.h"
#include "Log.h"
#include <sstream>

using namespace std;


static const char* moduleName = "ShaderParser";


// Our master list of shaders
vector< Q3ShaderFile > Shader::shaderList;
bool Shader::shadersAreLoaded = false;

const int NUM_SHADER_FILES = 27;


// List of shader files
string shaderFiles[] =
{
	"models.shader",
	"sfx.shader", "gfx.shader", "base.shader",          "base_wall.shader",     "gothic_light.shader",  "organics.shader",
	"base_button.shader",   "common.shader",        "gothic_trim.shader",   "sfx.shader",
	"base_floor.shader",    "ctf.shader",           "gothic_wall.shader",   "shrine.shader",
	"base_light.shader",    "eerie.shader",         "hell.shader",          "skin.shader",
	"base_object.shader",   "liquid.shader",        "sky.shader",
	"base_support.shader",  "gothic_block.shader",  "test.shader",
	"base_trim.shader",     "gothic_floor.shader", "menu.shader", "",
};



int ShaderParser::loadShaders(void)
{
	Trace("Loading shaders ...");
	shaderList.clear();

	// For each shader file ...
	for(int i = 0; i < NUM_SHADER_FILES; ++i)
	{
		string name = "scripts/" + shaderFiles[i];

		//Trace("Parsing shader script %s", name.c_str());

		// Try to open ...
		ifstream inFile;
		inFile.open(name.c_str());

		// Couldn't open - abort
		if(inFile.fail())
			CEngine::CriticalError("Could not load shader script " + name);

		// Now read in all of the individual shaders from the file
		extractShadersFromFile(inFile);

		inFile.close();
	}

	Trace("Done loading shaders.");

	return 0;
}


char *trimwhitespace(char *str)
{
	char *end;

	end = str + strlen(str) - 1;

	// Trim leading space
	while(isspace(*str) && str < end) str++;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;

	// Write new null terminator
	*(end + 1) = 0;

	return str;
}


// Reads all of the shaders from a single file
int ShaderParser::extractShadersFromFile(ifstream& inFile)
{
	inFile >> noskipws;

	// As long as there is stuff to read, read it ...
	while(!inFile.eof())
	{
		string shaderName;
		char buf1[512];
		char* buf;
		int currentStage = -1;
		stack< string > st;
		Q3Shader* pShader = new Q3Shader();

		// Read the shader's name
		do
		{
			inFile >> ws;
			inFile.getline(buf1, sizeof(buf1));
		}while(buf1[0] == '/' && buf1[1] == '/');


		shaderName = buf1;
		std::transform(shaderName.begin(), shaderName.end(), shaderName.begin(), ::tolower);
		pShader->setName(shaderName);
		//Trace("Parsing shader %s", shaderName.c_str());

		// Each shader is represented by a block
		// Push and pop braces until we are finished this block
		do
		{
loop:

			do
			{
				//inFile >> ws;
				if(inFile.eof())
					break;

				inFile.getline(buf1, sizeof(buf1));
				buf = trimwhitespace(buf1);
				if(buf[0] == '{' && buf[1] != 0 && buf[2] != 0)
				{
					int i = strlen(buf);
					buf = buf + 1;
					++currentStage;
					if(currentStage > 0)
						pShader->setCurrentStage(currentStage);
					st.push("{");
					break;
				}

			}while((buf[0] == '/' && buf[1] == '/') || strlen(buf) == 0);

			char* word = strtok(buf, " \t");
			if(word == NULL)
			{
				if(inFile.eof())
					break;
				else
					goto loop;
			}

			string token = word;
			token.erase(remove_if(token.begin(), token.end(), isspace), token.end());

			if(token[0] == '{' && token.size() > 1)
				token = token.substr(1, token.size() - 1);

			std::transform(token.begin(), token.end(), token.begin(), ::tolower);

			// This is where the bulk of the script parsing code goes ...


			// Start of a new block or stage
			if(token == "{")
			{
				++currentStage;
				if(currentStage > 0)
					pShader->setCurrentStage(currentStage);

				st.push(token);
			}


			// End of a block
			else if(token == "}")
			{
				st.pop();
			}


			// RGBGEN
			else if(token == "rgbgen")
			{
				string rgb = getNextToken(buf);
				pShader->setRgbGenByName(rgb);
			}

			else if(token == "alphagen")
			{
				string rgb = getNextToken(buf);
				pShader->setAlphaGenByName(rgb);
			}

			else if(token == "animmap")
			{
				string frequency = getNextToken(buf);
				pShader->setAnimFrequency(atof(frequency.c_str()));

				while(true)
				{
					string textureName = getNextToken(buf);
					if(textureName.find("texture") == -1)
						break;

					pShader->addAnimMapTexture(textureName);
				}
			}

			// MAP
			else if(token == "map")
			{
				string name = getNextToken(buf);
				if(name == "textures/effects/tinfxb.tga")
					Trace("");
				pShader->setTextureName(name);
			}


			// BLENDFUNC
			else if(token == "blendfunc")
			{
				string token1 = getNextToken(buf);

				if(token1 == "blend")
				{
					pShader->setSrcBlend(GL_SRC_ALPHA);
					pShader->setDestBlend(GL_ONE_MINUS_SRC_ALPHA);
				}
				else if(token1 == "filter")
				{
					pShader->setSrcBlend(GL_DST_COLOR);
					pShader->setDestBlend(GL_ZERO);
				}
				else if(token1 == "add" || token1 == "gl_add")
				{
					pShader->setSrcBlend(GL_ONE);
					pShader->setDestBlend(GL_ONE);
				}
				else
				{
					string token2 = getNextToken(buf);

					if(token1 == "gl_dst_color")
						pShader->setSrcBlend(GL_DST_COLOR);
					else if(token1 == "gl_src_alpha")
						pShader->setSrcBlend(GL_SRC_ALPHA) ;
					else if(token1 == "gl_one")
						pShader->setSrcBlend(GL_ONE) ;
					else if(token1 == "gl_one_minus_src_alpha")
						pShader->setSrcBlend(GL_ONE_MINUS_SRC_ALPHA) ;
					else if(token1 == "gl_zero")
						pShader->setSrcBlend(GL_ZERO) ;
					else if(token1 == "gl_add")
						pShader->setSrcBlend(GL_ADD) ;
					else if(token1 == "gl_one_minus_dst_color")
						pShader->setSrcBlend(GL_ONE_MINUS_DST_COLOR) ;
					else
						Assert(false);

					if(token2 == "gl_dst_color")
						pShader->setDestBlend(GL_DST_COLOR);
					else if(token2 == "gl_src_alpha")
						pShader->setDestBlend(GL_SRC_ALPHA) ;
					else if(token2 == "gl_src_color")
						pShader->setDestBlend(GL_SRC_COLOR) ;
					else if(token2 == "gl_one")
						pShader->setDestBlend(GL_ONE) ;
					else if(token2 == "gl_one_minus_src_alpha")
						pShader->setDestBlend(GL_ONE_MINUS_SRC_ALPHA) ;
					else if(token2 == "gl_one_minus_src_color")
						pShader->setDestBlend(GL_ONE_MINUS_SRC_COLOR) ;
					else if(token2 == "gl_zero")
						pShader->setDestBlend(GL_ZERO) ;
					else if(token2 == "gl_add")
						pShader->setDestBlend(GL_ADD) ;
					else if(token2 == "gl_one_minus_dst_color")
						pShader->setDestBlend(GL_ONE_MINUS_DST_COLOR) ;
					else if(token2 == "gl_one_minus_dst_alpha")
						pShader->setDestBlend(GL_ONE_MINUS_DST_ALPHA) ;
					else
						Assert(false);
				}
			}


			// SURFACEPARM
			else if(token == "surfaceparm")
			{
				string s = getNextToken(buf);
				pShader->addSurfaceParamByName(s);
			}


			// TCMOD
			else if(token == "tcmod")
			{
				string s = getNextToken(buf);

				if(s == "scroll")
				{
					string param1 = getNextToken(buf);
					string param2 = getNextToken(buf);
					pShader->setTcModScroll(atof(param1.c_str()), atof(param2.c_str()));
				}

				else if(s == "scale")
				{
					string param1 = getNextToken(buf);
					string param2 = getNextToken(buf);
					pShader->setTcModScale(atof(param1.c_str()), atof(param2.c_str()));
				}

				else if(s == "rotate")
				{
					string param = getNextToken(buf);
					pShader->setTcModRotate(atof(param.c_str()));
				}

				else if(s == "turb")
				{
					string param1 = getNextToken(buf);
					string param2 = getNextToken(buf);
					string param3 = getNextToken(buf);
					string param4 = getNextToken(buf);
				}
				else if(s == "stretch")
				{
					string param1 = getNextToken(buf);
					string param2 = getNextToken(buf);
					string param3 = getNextToken(buf);
					string param4 = getNextToken(buf);
					string param5 = getNextToken(buf);
				}
				else if(s == "transform")
				{
					string param1 = getNextToken(buf);
					string param2 = getNextToken(buf);
					string param3 = getNextToken(buf);
					string param4 = getNextToken(buf);
					string param5 = getNextToken(buf);
					string param6 = getNextToken(buf);
				}
				else
					Assert(false);
			}
			else if(token == "tcgen")
			{
				string s = getNextToken(buf);

				if(s == "base")
				{
					pShader->setTcGen(TcGen::Base);
				}
				else if(s == "lightmap")
				{
					pShader->setTcGen(TcGen::Lightmap);
				}
				else if(s == "environment")
				{
					pShader->setTcGen(TcGen::Environment);
				}
				else if(s == "vector")
				{
					pShader->setTcGen(TcGen::Vector);
				}
				else
					Assert(false);
			}
			else if(token == "depthfunc")
			{
				string s = getNextToken(buf);
				if(s == "equal")
					pShader->setDepthFunc(GL_EQUAL);
				else
					Assert(false);
			}
			else if(token == "cull")
			{
				string s = getNextToken(buf);
				if(s == "front" || s == "backsided")
					pShader->setCullFace(GL_FRONT);
				else if(s == "back")
					pShader->setCullFace(GL_BACK);
				else if(s == "disable" || s == "none" || s == "twosided")
					pShader->setCullEnable(false);
				else
					Assert(false);
			}
			else if(token == "sort")
			{
				string s = getNextToken(buf);
				if(s == "portal" || s == "1")
					pShader->setSortMode(SortPortal);
				else if(s == "sky" || s == "2")
					pShader->setSortMode(SortSky);
				else if(s == "opaque" || s == "3")
					pShader->setSortMode(SortOpaque);
				else if(s == "banner" || s == "6")
					pShader->setSortMode(SortBanner);
				else if(s == "underwater" || s == "8")
					pShader->setSortMode(SortUnderwater);
				else if(s == "additive" || s == "9" || s == "5") // fixme?
					pShader->setSortMode(SortAdditive);
				else if(s == "nearest" || s == "16")
					pShader->setSortMode(SortNearest);
				else
					Assert(false);

			}
			else if(token == "alphafunc")
			{
				string s = getNextToken(buf);
				if(s == "gt0")
					pShader->setAlpha(GL_GREATER);
				else if(s == "lt128")
					pShader->setAlpha(GL_LESS);
				else if(s == "ge128")
					pShader->setAlpha(GL_GEQUAL);
				else
					Assert(false);
			}
			else if(token == "depthwrite")
			{
				pShader->setDepthWrite(true);
			}
			else if(token == "clampmap")
			{
				string name = getNextToken(buf);
				pShader->setTextureName(name);
				pShader->setClamp(true);
			}
			 else if(token == "fogparms")
			 {
				getNextToken(buf);// (
				float r = atof(getNextToken(buf).c_str());
				float g = atof(getNextToken(buf).c_str());
				float b = atof(getNextToken(buf).c_str());
				getNextToken(buf);// )
				float distance = atof(getNextToken(buf).c_str());
				pShader->setFogParms(r, g, b, distance);
			 }
			else
			{
				//Trace("Unknown: %s", token.c_str());
			}
		}
		while(!st.empty() && !inFile.eof());

		// Add the shader to our master list
		shaderList[shaderName] = pShader;
		//Trace("Added shader %s", shaderName.c_str());
	}

	return 0;
}


//8s
// Returns the next valid 'token' from the input stream
//string ShaderParser::getNextToken(ifstream& inFile)
string ShaderParser::getNextToken(char* buf)
{
	char* p = strtok(NULL, " ");
	if(p == NULL)
		return "";

	char* start = p;
	char* end;
	while(*p == ' ' || *p == '\t')
	{
		start = p;
		p++;
	}

	end = start + strlen(start);
	while(*end == ' ' || *end == '\t' )
	{
		*end = 0;
		end--;

	}
p = start;
while(*p)
{
	*p = tolower(*p);
	if(*p == '\t')
		*p = ' ';
	p++;
}
	
if(*start == ' ')
start++;
return string(start);

	/*string s = p;
	s.erase(remove_if(s.begin(), s.end(), isspace), s.end());

	std::transform(s.begin(), s.end(), s.begin(), ::tolower);*/
	//return s;
}


// Deletes all of the shaders
int ShaderParser::freeShaders(void)
{
	map< string, Q3Shader* >::iterator i;
	for(i = shaderList.begin(); i != shaderList.end(); ++i)
		delete i->second;

	return 0;
}


bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() > ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}


// Initializes the shader of the given name, and returns a pointer to it
Q3Shader* ShaderParser::mapShaderName(const char* name)
{
	Q3Shader* pShader = NULL;
	std::map< std::string, Q3Shader* > ::iterator i;
	Assert(name);
	
	// Look for the shader's name in the list of shaders
	i = shaderList.find(name);
	if(i != shaderList.end())
		pShader = i->second;

	// Create default shader if it doesn't exist
	if(pShader == NULL)
	{
		pShader = new Q3Shader();
		pShader->setCurrentStage(1);
		pShader->setTextureName("$lightmap");
		pShader->setSrcBlend(GL_ONE);
		pShader->setDestBlend(GL_ZERO);
		pShader->setCurrentStage(2);
		pShader->setSrcBlend(GL_DST_COLOR);
		pShader->setDestBlend(GL_ZERO);
				
		string s = name;
		s = s + ".tga";
		pShader->setTextureName(s);
	}

	return pShader;
}
