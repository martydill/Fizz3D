
#ifndef GLSTATE_H
#define GLSTATE_H

#include "Fizz3d.h"

class GLState
{
public:
	GLState(void);
	~GLState(void);

	static void Enable(GLenum flag);
	static void Disable(GLenum flag);

	static void Reset();
private:
	static bool cullEnabled;
	static bool alphaTestEnabled;
	static bool blendEnabled;
	static bool depthTestEnabled;
	static GLuint srcBlendMode;
	static GLuint destBlendMode;
};

#endif
