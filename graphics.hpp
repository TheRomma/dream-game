#pragma once

#include <string>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "3Dmaths.hpp"
#include "shader.hpp"

#include <iostream>

//Higher level interface for common vertex buffers.
struct VertexBuffer{
	VertexBuffer(){};
	void init(Uint32 stride, Uint32 length, float* data);
	~VertexBuffer();

	void bind();
	void setAttribute(Uint32 location, Uint32 size);

	Uint32 numVertices;		//Number of individual vertices.

	private:
	Uint32 vao, vbo;		//Vertex attribute and buffer object handlers.
	Uint32 stride;			//Length of the stride between vertices.
	long attribLength;
};

//Higher level interface for common textures.
struct Material{
	Material(){};
	void init(Uint32 width, Uint32 height, Uint32 depth, Uint32 wrap,
		Uint32 filter, float* diffSource, float* metalRoughSource);
	~Material();

	void bind(Uint32 slot);

	private:
	Uint32 diffuse;		//Texture object handler.
	Uint32 metalRough;
};

//Higher level interface for UBOs, SSBOs, etc.
struct ShaderBuffer{
	ShaderBuffer(){};
	void init(Uint32 type, Uint32 base, Uint32 length, char* data);
	~ShaderBuffer();

	void bind();
	void write(Uint32 offset, Uint32 length, char* data);
	void bindBase(Uint32 base);

	private:
	Uint32 buffer;			//Buffer object handler.
	Uint32 type;			//Buffer objext type.
	Uint32 base;			//Buffer object binding.
};

//Struct for common uniform data.
struct CommonBlock{
	Mat4 projView;		//Combination of view and projection matrices.
	Vec3 position;		//Position of the theoretical camera.
	float time;			//Elapsed time.
};

//UBO containing common uniforms for drawing.
struct CommonUniforms{
	CommonUniforms();
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
	void bindLayer(Uint32 layer);

	Uint32 width, height, numMaps;

	private:
	Uint32 frame, depth;
};

//Environment map
struct EnvironmentMap{
	EnvironmentMap(){};
	void init(const char* filename);
	~EnvironmentMap();

	void bind(Uint32 slot);
	void display();

	Uint32 texture; 
	Shader program;
	VertexBuffer buffer;
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
	//Vec3 ambient;
	//	char padding0[4];
	Vec3 diffuse;
		char padding1[4];
	Mat4 projViewCSM[6];
};

//Spotlight data.
struct Spotlight{
	Vec3 position;
	float radius;
	Vec3 direction;
	float cutOff;
	//Vec3 ambient;
	//	char padding0[4];
	Vec3 diffuse;
		char padding1[4];
	Mat4 projViewCSM;
};

//Light uniforms.
struct LightBlock{
	Sun sun;
	float numPointlights;
	float numStaticPointlights;
	float numSpotlights;
	float numStaticSpotlights;
	Pointlight pointlights[MAX_POINTLIGHTS];
	Spotlight spotlights[MAX_SPOTLIGHTS];
};

//UBO containing light uniforms for light calculations.
struct LightUniforms{
	LightUniforms(Uint32 shadowWidth, Uint32 shadowHeight, const char* environment);
	~LightUniforms(){};

	void bind();
	void reset();
	void resetStatic();
	void push(Pointlight light);
	void push(Spotlight light);
	void pushStatic(Pointlight* points, Uint32 numPoints, Spotlight* spots, Uint32 numSpots);
	void write();
	void calcShadowProjections(Vec3 position);
	void bindShadowFrame();
	void bindShadowMap();
	void bindEnvironmentMap();
	void displayEnvironmentMap();

	LightBlock block;
	CSM shadows;
	EnvironmentMap envMap;

	private:
	ShaderBuffer ubo;
};

struct DeferredTarget{
	DeferredTarget(Uint32 width, Uint32 height);
	~DeferredTarget();

	void bind();
	void bindDisplay();
	void draw();
	void display(Uint32 width, Uint32 height);

	Uint32 width, height;

	private:
	Uint32 gBuffer, gPosition, gNormal, gAlbedo, depthStencil;
	Uint32 displayBuffer, displayImage;
	Shader deferredProgram, displayProgram;
	VertexBuffer buffer;
};
