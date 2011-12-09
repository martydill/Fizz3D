
#include "Fizz3d.h"
#include "GLState.h"

bool GLState::cullEnabled;
bool GLState::alphaTestEnabled;
bool GLState::blendEnabled;
bool GLState::depthTestEnabled;
GLuint GLState::srcBlendMode;
GLuint GLState::destBlendMode;

GLState::GLState(void)
{
}

GLState::~GLState(void)
{
}


void GLState::Enable(GLenum flag)
{
	switch(flag)
	{
		case GL_CULL_FACE:
			if(!cullEnabled)
			{
				cullEnabled = true;
				glEnable(GL_CULL_FACE);
			}
			break;

		case GL_DEPTH_TEST:
			if(!depthTestEnabled)
			{
				depthTestEnabled = true;
				glEnable(GL_DEPTH_TEST);
			}
			break;

		case GL_BLEND:
			if(!blendEnabled)
			{
				blendEnabled = true;
				glEnable(GL_BLEND);
			}
			break;

		case GL_ALPHA_TEST:
			if(!alphaTestEnabled)
			{
				alphaTestEnabled = true;
				glEnable(GL_ALPHA_TEST);
			}
			break;
	}
}

void GLState::Disable(GLenum flag)
{
	switch(flag)
	{
		case GL_CULL_FACE:
			if(cullEnabled)
			{
				cullEnabled = false;
				glDisable(GL_CULL_FACE);
			}
			break;

		case GL_DEPTH_TEST:
			if(depthTestEnabled)
			{
				depthTestEnabled = false;
				glDisable(GL_DEPTH_TEST);
			}
			break;

		case GL_BLEND:
			if(blendEnabled)
			{
				blendEnabled = false;
				glDisable(GL_BLEND);
			}
			break;

		case GL_ALPHA_TEST:
			if(alphaTestEnabled)
			{
				alphaTestEnabled = false;
				glDisable(GL_ALPHA_TEST);
			}
			break;
	}
}


void GLState::Reset()
{
	glDisable(GL_ALPHA_TEST);
	alphaTestEnabled = false;

	glDisable(GL_BLEND);
	blendEnabled = false;

	glDisable(GL_DEPTH_TEST);
	depthTestEnabled = false;

	glDisable(GL_CULL_FACE);
	cullEnabled = false;
}