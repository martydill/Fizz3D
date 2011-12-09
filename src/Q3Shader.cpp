
// Q3Shader.cpp
// Defines the Quake 3 shader class
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "Q3Shader.h"
#include "Log.h"

static const char* moduleName = "Q3Shader";


// Standard constructor
Q3Shader::Q3Shader() :
shaderName("Unnamed")
{
	shaderStages.reserve(4);
	numberOfStages = 0;
	surfaceParam = Normal;
	currentStage = 0;
	sortMode = SortOpaque;
	cullEnabled = true;
	cullFunc = GL_FRONT;
}


// Standard destructor
Q3Shader::~Q3Shader()
{
	// Delete all of the texture stages in the shader
	std::vector< Q3ShaderStage* >::iterator i;

	for(i = shaderStages.begin(); i != shaderStages.end(); ++i)
		delete *i;
}


// Returns the shader's name
std::string Q3Shader::getName(void) const
{
	return shaderName;
}


// Sets the shader's name
void Q3Shader::setName(std::string newName)
{
	shaderName = newName;
	return;
}


// Returns the shader's surface parameters
unsigned int Q3Shader::getSurfaceParam(void) const
{
	return surfaceParam;
}


// Returns the shader's surface parameters
void Q3Shader::setSurfaceParam(ShaderSurfaceParam param)
{
	surfaceParam = param;
	return;
}


// Sets the shader's waveform type
void Q3Shader::setWaveformType(ShaderWaveForm waveForm)
{
	Assert(currentStage < shaderStages.size());

	shaderStages[currentStage]->waveFormType = waveForm;
	return;
}


ShaderRGBGenType getRgbGenByName(std::string &name)
{
	if(name == "identitylighting")
		return IdentityLighting;
	else if(name == "identity")
		return Identity;
	else if(name == "wave")
		return Wave;
	else if(name == "entity" || name == "entity //")
		return MapEntity;
	else if(name == "oneminusentity")
		return OneMinusEntity;
	else if(name == "vertex")
		return Vertex;
	else if(name == "oneminusvertex")
		return OneMinusVertex;
	else if(name == "lightingdiffuse")
		return LightingDiffuse;
	else if(name == "exactvertex")
		return ExactVertex;
	else if(name == "lightingspecular")
		return LightingSpecular;
	else if(name == "portal")
		return Portal;
	else
		Assert(false);
}


// Sets the shader's RGBgen type, based on the specified string
void Q3Shader::setRgbGenByName(std::string name)
{
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->rgbType = getRgbGenByName(name);

	return;
}

// Sets the shader's RGBgen type, based on the specified string
void Q3Shader::setAlphaGenByName(std::string name)
{
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->alphaType = getRgbGenByName(name);
}


// Adds the specified parameter to the shader's surface parameters
void Q3Shader::addSurfaceParamByName(std::string paramName)
{
	if(paramName == "normal")
		surfaceParam |= Normal;

	else if(paramName == "alphashadow")
		surfaceParam |= AlphaShadow;

	else if(paramName == "areaportal")
		surfaceParam |= AreaPortal;

	else if(paramName == "clusterportal")
		surfaceParam |= ClusterPortal;

	else if(paramName == "donotenter")
		surfaceParam |= DoNotEnter;

	else if(paramName == "flesh")
		surfaceParam |= Flesh;

	else if(paramName == "fog")
		surfaceParam |= Fog;

	else if(paramName == "lava")
		surfaceParam |= Lava;

	else if(paramName == "metalsteps")
		surfaceParam |= MetalSteps;

	else if(paramName == "nodamage")
		surfaceParam |= NoDamage;

	else if(paramName == "nodraw")
		surfaceParam |= NoDraw;

	else if(paramName == "nodrop")
		surfaceParam |= NoDrop;

	else if(paramName == "noimpact")
		surfaceParam |= NoImpact;

	else if(paramName == "nomarks")
		surfaceParam |= NoMarks;

	else if(paramName == "nolightmap")
		surfaceParam |= NoLightmap;

	else if(paramName == "nosteps")
		surfaceParam |= NoSteps;

	else if(paramName == "nonsolid")
		surfaceParam |= NonSolid;

	else if(paramName == "origin")
		surfaceParam |= Origin;

	else if(paramName == "playerclip")
		surfaceParam |= PlayerClip;

	else if(paramName == "slick")
		surfaceParam |= Slick;

	else if(paramName == "slime")
		surfaceParam |= Slime;

	else if(paramName == "structural")
		surfaceParam |= Structural;

	else if(paramName == "trans")
		surfaceParam |= Trans;

	else if(paramName == "water")
		surfaceParam |= Water;

	else if(paramName == "detail")
		surfaceParam |= Detail;

	else if(paramName == "nomipmaps")
		surfaceParam |= NoMipMap;

	else if(paramName == "nodlight")
		surfaceParam |= NoDLight;

	else if(paramName == "sky")
		surfaceParam |= Sky;

	else
		Assert(false);

	return;
}


// Returns the texture stage currently being created
int Q3Shader::getCurrentStage(void) const
{
	return currentStage;
}


// Sets the texture stage currently being created
void Q3Shader::setCurrentStage(int stage)
{
	if(stage > numberOfStages)
	{
		shaderStages.push_back(new Q3ShaderStage());
		currentStage = stage - 1;
		++numberOfStages;
	}

	return;
}


// Sets the texture name for the current stage
void Q3Shader::setTextureName(std::string name)
{
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->textureName = name;
	return;
}


// Loads all of the textures that the shader uses
void Q3Shader::loadTextures(ITextureFactory* pTextureFactory)
{
	Assert(pTextureFactory);
	std::vector< Q3ShaderStage* >::iterator i;

	// Load the texture for each stage
	for(i = shaderStages.begin(); i != shaderStages.end(); ++i)
	{
		Q3ShaderStage* pStage = *i;
		if(pStage->textureName != "$lightmap")
		{
			pStage->textureId = pTextureFactory->loadTextureFromFile(pStage->textureName);
		}

		// Load textures for the animations
		if(pStage->animMapTextureNames.size() > 0)
		{
			for(int n = 0; n < pStage->animMapTextureNames.size(); ++n)
			{
				pStage->animMapTextureIds.push_back(pTextureFactory->loadTextureFromFile(pStage->animMapTextureNames.at(n)));
			}
		}
	}

	return;
}


// Returns the number of stages that the shader has
int Q3Shader::getNumberOfStages(void) const
{
	return numberOfStages;
}


// Returns the struct corresponding to a given stage
Q3ShaderStage* Q3Shader::getShaderStage(int stage) const
{
	Assert(stage <= numberOfStages && stage > 0);
	return shaderStages[stage - 1];
}


// Sets the tc rotation for the current stage
void Q3Shader::setTcModRotate(float rotation)
{
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->tcModRotate = rotation;
	return;
}


// Sets the tc scale for the given stage
void Q3Shader::setTcModScale(float sScale, float tScale)
{
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->tcModScaleS = sScale;
	shaderStages[currentStage]->tcModScaleT = tScale;
	return;
}


// Sets the tc scroll for the current stage
void Q3Shader::setTcModScroll(float sSpeed, float tSpeed)
{	
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->tcModScrollSspeed = sSpeed;
	shaderStages[currentStage]->tcModScrollTspeed = tSpeed;
	return;
}


// Sets the tc gen for the current stage
void Q3Shader::setTcGen(TcGen tc)
{	
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->tcGen = tc;
}


// Sets the tc gen for the current stage
void Q3Shader::setSrcBlend(BlendFunc tc)
{	
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->srcBlend = tc;
	if(sortMode == SortOpaque)
		sortMode = SortAdditive;
}


// Sets the tc gen for the current stage
void Q3Shader::setDestBlend(BlendFunc tc)
{	
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->destBlend = tc;
}


// Sets the tc gen for the current stage
void Q3Shader::setAnimFrequency(float freq)
{	
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->animFrequency = freq;

	return;
}


// Sets the tc gen for the current stage
void Q3Shader::addAnimMapTexture(std::string textureName)
{	
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->animMapTextureNames.push_back(textureName);

	return;
}



// Sets the tc gen for the current stage
void Q3Shader::setDepthFunc(int df)
{	
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->depthFunc = df;

	return;
}


// Sets the tc gen for the current stage
void Q3Shader::setCullFace(int df)
{	
	this->cullFunc = df;
	return;
}



// Sets the tc gen for the current stage
void Q3Shader::setCullEnable(bool df)
{	
	this->cullEnabled = df;
	return;
}


// Sets the sort mode for the current stage
void Q3Shader::setSortMode(SortMode sortMode)
{	
	this->sortMode = sortMode;
	return;
}


// Sets the sort mode for the current stage
SortMode Q3Shader::getSortMode()
{	
	return this->sortMode;
}


// Sets the tc gen for the current stage
void Q3Shader::setAlpha(int af)
{	
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->alphaFunc = af;
	shaderStages[currentStage]->alphaEnabled = true;

	return;
}

// Sets the tc gen for the current stage
void Q3Shader::setDepthWrite(bool dw)
{	
	shaderStages[currentStage]->depthWrite = dw;
	return;
}

// Sets the tc gen for the current stage
void Q3Shader::setClamp(bool c)
{	
	Assert(currentStage < shaderStages.size());
	shaderStages[currentStage]->clamp = c;

	return;
}


void Q3Shader::setFogParms(float r, float g, float b, float distance)
{
   fogParms.r = r;
   fogParms.g = g;
   fogParms.b = b;
   fogParms.distance = distance;
}