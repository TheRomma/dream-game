#pragma once

#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "3Dmaths.hpp"

#define UNIFORM_COMMON_BASE 0
#define UNIFORM_LIGHTING_BASE 1

#define SUN_SHADOW_BASE 3
#define SUN_NUM_SHADOW_CASCADES 4

#define MAX_POINTLIGHTS 32
#define MAX_SPOTLIGHTS 32

//Find and replace (the FIRST OCCURENCE of) a word in a cpp string.
void findAndReplace(std::string& str, std::string before, std::string after);

//Shader.
struct Shader{
	Shader(){};
	void init(const char* vertexSource, const char* fragmentSource);
	void init(const char* vertexSource, const char* geometrySource, const char* fragmentSource);
	~Shader();

	void use();

	private:
	Uint32 program;
};

//Vertex buffer.
struct VertexBuffer{
	VertexBuffer(){};
	void init(Uint32 stride, Uint32 length, char* data);
	~VertexBuffer();

	void bind();
	void setAttribute(Uint32 location, Uint32 size);

	Uint32 numVertices;

	private:
	Uint32 vao, vbo;
	Uint32 stride;
	long attribLength;
};

//2D texture.
struct Texture{
	Texture(){};
	void init(Uint32 width, Uint32 height, Uint32 filter, char* data);
	~Texture();

	void bind(Uint32 slot);

	private:
	Uint32 texture;
};

//Array texture.
struct ArrayTexture{
	ArrayTexture(){};
	void init(Uint32 width, Uint32 height, Uint32 depth, Uint32 filter, char* data);
	~ArrayTexture();

	void bind(Uint32 slot);

	private:
	Uint32 texture;
};

//UBOs & SSBOs etc.
struct ShaderBuffer{
	ShaderBuffer(){};
	void init(Uint32 type, Uint32 base, Uint32 length, char* data);
	~ShaderBuffer();

	void bind();
	void write(Uint32 offset, Uint32 length, char* data);
	void bindBase(Uint32 base);

	private:
	Uint32 buffer, type, base;
};

//Common uniform data.
struct CommonBlock{
	Mat4 projView;
	Vec3 position;
	float time;
};

//Common uniforms.
struct CommonUniforms{
	CommonUniforms(){};
	void init();
	~CommonUniforms(){};

	void bind();
	void write();

	CommonBlock block;

	private:
	ShaderBuffer ubo;
};

//Cascaded shadow map.
struct CSM{
	CSM(){};
	void init(Uint32 width, Uint32 height, Uint32 numMaps);
	~CSM();

	void bind();
	void bindTexture(Uint32 base);

	Uint32 width, height, numMaps;

	private:
	Uint32 frame, depth;
};

//Sun data.
struct Sun{
	Vec3 direction;
		char padding0[4];
	Vec3 ambient;
		char padding1[4];
	Vec3 diffuse;
		char padding2[4];
	Mat4 projViewCSM[SUN_NUM_SHADOW_CASCADES];
};

//Pointlight data.
struct Pointlight{
	Vec3 position;
	float radius;
	Vec3 ambient;
		char padding0[4];
	Vec3 diffuse;
		char padding1[4];
};

//Spotlight data.
struct Spotlight{
	Vec3 position;
	float radius;
	Vec3 direction;
	float cutOff;
	Vec3 ambient;
		char padding0[4];
	Vec3 diffuse;
		char padding1[4];
};

//Light uniforms.
struct LightingBlock{
	Sun sun;
	float numPointlights;
	float numStaticPointlights;
	float numSpotlights;
	float numStaticSpotlights;
	Pointlight pointlights[MAX_POINTLIGHTS];
	Spotlight spotlights[MAX_SPOTLIGHTS];
};

//Light storage.
struct Lighting{
	Lighting(){};
	void init(Uint32 shadowWidth, Uint32 shadowHeight);
	~Lighting(){};

	void bind();
	void reset();
	void resetStatic();
	void push(Pointlight light);
	void push(Spotlight light);
	void pushStatic(Pointlight* points, Uint32 numPoints, Spotlight* spots, Uint32 numSpots);
	void write();

	LightingBlock block;
	CSM sunCSM;

	private:
	ShaderBuffer ubo;
};

//Deferred shading render target.
struct DeferredTarget{
	DeferredTarget(){};
	void init(Uint32 width, Uint32 height);
	~DeferredTarget();

	void bind();
	void bindFinal();
	void draw();
	void display(Uint32 displayWidth, Uint32 displayHeight);

	Uint32 width, height;

	private:
	Uint32 gBuffer, gPosition, gNormal, gAlbedo, depthStencil, finalBuffer, finalImage;
	Shader deferredProgram, displayProgram;
	VertexBuffer buffer;
};
