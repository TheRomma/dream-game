#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "shader.hpp"
#include "models.hpp"

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
	float radius = 10.0;
	Vec3 diffuse;
		char padding1[4];
};

//Spotlight data.
struct Spotlight{
	Vec3 position;
	float radius = 10.0;
	Vec3 direction;
	float cutOff = 1.5;
	Vec3 diffuse;
		char padding1[4];
	Mat4 projViewCSM;
};

//Common uniforms.
struct CommonUniforms{
	Mat4 projView;
	Vec3 camPosition;
	float time = 0.0;
};

//Light uniforms.
struct LightUniforms{
	Sun sun;
	float numPointlights = 0.0;
		char padding1[4];
	float numSpotlights = 0.0;
		char padding0[4];
	Pointlight pointlights[MAX_POINTLIGHTS];	
	Spotlight spotlights[MAX_SPOTLIGHTS];	
};

//Block of common uniform data.
struct UniformBlock{
	CommonUniforms common;
	LightUniforms lights;
};

//Draw request packet.
struct DrawRequest{
	Uint32 gProgram = 0;
	Uint32 shadowProgram = 0;
	Uint32 vao = 0;	
	Uint32 numVertices = 0;
	Uint32 diffuse = 0;
	Uint32 metalRough = 0;
	Animation* anim = nullptr;
	Uint32 numBones = 0;
	float animTime = 0.0;
	Mat4 model;
	Vec3 centroid;
	float cullRadius = 1.0;
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
	Uint32 frameBloom = 5;
	float frameHFov = 1.7;

	Uint32 shadowWidth = 1024;
	Uint32 shadowHeight = 1024;

	Uint32 rendererDrawQueueSize = 128;
};

//Renderer.
struct Renderer{
	Renderer(){};
	int init(RendererSettings settings);
	~Renderer();

	void toggleWindowFullscreen();
	Mat4 getWindowProjection(float hFov);//Temp?

	void bindDisplay();//Temp?
	void deferredPass();
	void applyBloom(Uint32 blurPasses);
	void displayFrame();

	void pushLight(Pointlight light);
	void pushLight(Spotlight light);

	void drawModel(StaticModel* mesh, Mat4 model);
	void drawModel(AnimatedModel* mesh, Mat4 model, Animation* anim, float animTime);

	UniformBlock uniforms;
	RendererSettings settings;

	private:
	SDL_Window* window;			//SDL Window.
	SDL_GLContext context;		//OpenGl context.

	Uint32 nullVao;

	Uint32 gBuffer, gPosition, gNormal, gAlbedo, gDepth;
	Uint32 displayBuffer, displayImage;
	Uint32 postBuffer, postImage;
	Uint32 blurBuffer[2], blurImage[2];
	Shader deferredProgram, displayProgram, moveProgram, combineProgram, kernelProgram, blurProgram;

	Uint32 ubo;

	Uint32 shadowBuffer, shadowImages;

	Uint32 numRequests, mostBones;
	DrawRequest* drawQueue = nullptr;
};
