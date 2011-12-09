
// Q3Shader.h
// Definition of the Quake 3 shader class
// (c) 2002 Marty Dill


#ifndef Q3SHADER_H
#define Q3SHADER_H

#include "TextureFactory.h"
#include "ShaderStage.h"


struct FogParms
{
   float r, g, b;
   float distance;
};


// Sort modes
enum SortMode
{
   SortDefault,
   SortPortal,
   SortSky,
   SortOpaque,
   SortBanner,
   SortUnderwater,
   SortAdditive,
   SortNearest
};


// Surface parameters
enum ShaderSurfaceParam
{
	Normal			= 0x0,
	AlphaShadow		= 0x01,
	AreaPortal		= 0x02,
	ClusterPortal	= 0x04,
	DoNotEnter		= 0x08,
	Flesh			= 0x10,
	Fog				= 0x20,
	Lava			= 0x40,
	MetalSteps		= 0x80,
	NoDamage		= 0x100,
	NoDraw			= 0x200,
	NoDrop			= 0x400,
	NoImpact		= 0x800,
	NoMarks			= 0x1000,
	NoLightmap		= 0x2000,
	NoSteps			= 0x4000,
	NonSolid		= 0x8000,
	Origin			= 0x10000,
	PlayerClip		= 0x20000,
	Slick			= 0x40000,
	Slime			= 0x80000,
	Structural		= 0x100000,
	Trans			= 0x200000,
	Water			= 0x400000,
	Detail			= 0x800000,
	NoMipMap		= 0x1000000,
	NoDLight		= 0x2000000,
	Sky				= 0x4000000
};


// The shader class
class Q3Shader
{
public:
	Q3Shader();
	~Q3Shader();

	std::string getName(void) const;
	void setName(std::string newName);

	int getCurrentStage(void) const;
	void setCurrentStage(int stage);

	unsigned int getSurfaceParam(void) const;
	void setSurfaceParam(ShaderSurfaceParam);

	ShaderWaveForm getWaveformType(void) const;
	void setWaveformType(ShaderWaveForm waveForm);

   void setAlphaGenByName(std::string name);
	void setRgbGenByName(std::string name);
	void addSurfaceParamByName(std::string paramName);

	void setTextureName(std::string name);

	void loadTextures(ITextureFactory* pTextureFactory);

	int getNumberOfStages(void) const;

	Q3ShaderStage* getShaderStage(int stage) const;

	void setTcModScale(float sScale, float tScale);
	void setTcModScroll(float sSpeed, float tSpeed);
	void setTcModRotate(float rotation);

	void setTcGen(TcGen tc);

	void setSrcBlend(BlendFunc bf);
	void setDestBlend(BlendFunc bf);

	void setAnimFrequency(float frequency);
	void addAnimMapTexture(std::string textureName);

	void setDepthFunc(int df);
	void setCullFace(int cf);
	void setCullEnable(bool enabled);
	void setDepthWrite(bool dw);
	void setAlpha(int af);
	void setClamp(bool c);

   void setSortMode(SortMode sortMode);
   SortMode getSortMode();

   void setFogParms(float r, float g, float b, float distance);

	// fixme private
	bool cullEnabled;
	int cullFunc;
   
	FogParms fogParms;
private:

	// The name of the shader
	std::string shaderName;

	// Surface parameters 
	unsigned int surfaceParam;

	// Stores the individual stages 
	std::vector< Q3ShaderStage* > shaderStages;

	// The number of stages the texture has
	int numberOfStages;

	// The stage we are currently building
	int currentStage;

   SortMode sortMode;

};



#endif

