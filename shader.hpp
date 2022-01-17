#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <string>

#include "3Dmaths.hpp"

#define UBO_COMMON_BASE 0
#define UBO_LIGHT_BASE 1

#define SUN_SHADOW_BASE 3

#define SUN_NUM_SHADOW_CASCADES 4
#define MAX_POINTLIGHTS 32
#define MAX_SPOTLIGHTS 32

//Shader program for hardware accelerated drawing.
struct Shader{
	Shader(){};
	void init(
		const char* vertexSources,
		const char* fragmentSources
	);
	void init(
		const char* vertexSources,
		const char* geometrySources,
		const char* fragmentSources
	);
	~Shader();

	void use();

	private:
	Uint32 program;			//Shader program handler.
};

//Header in glsl.
std::string glsl_header();

//Empty shaders.
std::string glsl_emptyShader();

//Common uniforms ubo in glsl.
std::string glsl_commonUniforms();

//Common light structs and ubo in glsl.
std::string glsl_commonLightStructs();

//Common lights calculations in glsl.
std::string glsl_lightCalculations();

//Shader program for static models.
std::string glsl_deferredStaticModelVertex();
std::string glsl_deferredStaticModelFragment();

std::string glsl_staticModelShadowVertex();
std::string glsl_staticModelShadowGeometry();

//Simple shaders for drawing a screen sized quad.
std::string glsl_displayQuadVertex();
std::string glsl_displayQuadFragment();

//Shaders for calculating light data.
std::string glsl_deferredLightPassFragment();
