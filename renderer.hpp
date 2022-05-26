#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "shader.hpp"

#define UBO_BINDING 0

//Sun data.
struct Sun{
	Vec3 direction;
		char padding0[4];
	Vec3 ambient;
		char padding1[4];
	Vec3 diffuse;
		char padding2[4];
	Mat4 projViewCSM[NUM_SUN_CASCADES];
};

//Pointlight data.
struct Pointlight{
	Vec3 position;
	float radius;
	Vec3 diffuse;
		char padding1[4];
};

//Spotlight data.
struct Spotlight{
	Vec3 position;
	float radius;
	Vec3 direction;
	float cutOff;
	Vec3 diffuse;
		char padding1[4];
	Mat4 projViewCSM;
};

//Block of common uniform data.
struct UniformBlock{
	Mat4 projView;
	Vec3 cameraPosition;
	float timeElapsed;

	Sun sun;
	float numPointlights;
	float numSpotlights;
		char padding0[8];
	Pointlight pointlights[MAX_POINTLIGHTS];	
	Spotlight spotlights[MAX_SPOTLIGHTS];	
};

//Settings for the renderer.
struct RendererSettings{
	const char* windowTitle = "A Game By Jere Koivisto";
	Uint32 windowWidth = 1280;
	Uint32 windowHeight = 720;
	Uint32 windowGLMajor = 4;
	Uint32 windowGLMinor = 3;
	Uint32 windowFlags = SDL_WINDOW_RESIZABLE;
	Uint32 windowVsync = -1;

	Uint32 frameWidth = 1280;
	Uint32 frameHeight = 720;
	Uint32 frameBlurWidth = 256;
	Uint32 frameBlurHeight = 144;

	Uint32 shadowWidth = 1024;
	Uint32 shadowHeight = 1024;
};

//Renderer.
struct Renderer{
	Renderer(){};
	int init(RendererSettings settings);
	~Renderer();

	void swapBuffers();

	void bindGBuffer();
	void deferredPass();
	void applyBloom(Uint32 blurPasses);
	void applyKernel(float* kernel);
	void display();

	void updateUniforms();

	UniformBlock uniforms;

	private:
	SDL_Window* window;			//SDL Window.
	SDL_GLContext context;		//OpenGl context.

	Uint32 nullVao;

	Uint32 frameWidth, frameHeight;
	Uint32 gBuffer, gPosition, gNormal, gAlbedo, gDepth;
	Uint32 displayBuffer, displayImage;
	Uint32 postBuffer, postImage;
	Uint32 blurBuffer[2], blurImage[2];
	Shader deferredProgram, displayProgram, moveProgram, combineProgram, kernelProgram, blurProgram;

	Uint32 ubo;

	Uint32 shadowBuffer, shadowImages;
};
