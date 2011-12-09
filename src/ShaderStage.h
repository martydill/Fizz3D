
#ifndef SHADERSTAGE_H
#define SHADERSTAGE_H

#include "Fizz3d.h"


typedef int BlendFunc;

enum TcGen
{
	Base = 0,
	Lightmap = 1,
	Environment = 2,
	Vector = 3
};


// Wave form types
enum ShaderWaveForm
{
	ShaderWaveForm_None			= 0,
	Sin				= 1,
	Triangle		= 2,
	Square			= 3,
	Sawtooth		= 4,
	InverseSawtooth = 5
};


// RGBgen types
enum ShaderRGBGenType
{
	Identity			= 1,
	IdentityLighting	= 2,
	Wave				= 3,
	MapEntity			= 4,
	OneMinusEntity		= 5,
	Vertex				= 6,
	OneMinusVertex		= 7,
	LightingDiffuse		= 8,
	ExactVertex = 9,
   LightingSpecular =10,
   Portal = 11
};


// The stage struct - holds all the info needed for a specific stage
struct Q3ShaderStage
{
	Q3ShaderStage()
	{
		textureId = -1;
		srcBlend = GL_ONE;
		destBlend = GL_ZERO;

		tcModScaleS = tcModScaleT = 1;
		tcModScrollSspeed = tcModScrollTspeed = -1;
		tcModRotate = -1;
		lastTcModRotate = 0;
		lastTcModScrollS = 0;
		lastTcModScrollT = 0;
		lastAnimFrameTime = 0;
		lastAnimFrame = 0;
		depthFunc = GL_LEQUAL;
		rgbType = Identity;
      alphaType = Identity;

		alphaEnabled = true;
		alphaFunc = GL_ALWAYS;
		depthWrite = true;
		
		clamp = false;
	}

	// Wave form type
	ShaderWaveForm waveFormType;

	// Wave parameters
	int lastAnimFrame;
	DWORD lastAnimFrameTime;
	float waveFormBase;
	float waveFormAmplitude;
	float waveFormPhase;
	float waveFormFrequency;

	// RGB type
	ShaderRGBGenType rgbType;

  	// RGB type
	ShaderRGBGenType alphaType;

	// The actual texture
	std::string textureName;

	// The texture's id
	TextureID textureId;

	// Texture coordinate modifiers
	float tcModScaleS, tcModScaleT;
	float tcModScrollSspeed, tcModScrollTspeed;
	float tcModRotate;
	float lastTcModRotate;
	float lastTcModScrollS;
	float lastTcModScrollT;

	TcGen tcGen;

	BlendFunc srcBlend;
	BlendFunc destBlend;

	float animFrequency;
	std::vector<int> animMapTextureIds;
	std::vector<std::string> animMapTextureNames;

	int depthFunc;
	int cullFunc;
	bool cullEnabled;

	bool alphaEnabled;
	int alphaFunc;

	bool depthWrite;
	bool clamp;
};

#endif
