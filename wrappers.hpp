#pragma once


#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "3Dmaths.hpp"
#include "graphics.hpp"

#define MAX_POINT_LIGHTS 64

//OpenGL Rendertarget.
struct Rendertarget{
	Rendertarget(){};
	void init(Uint32 width, Uint32 height);
	~Rendertarget();

	void bind();
	//void bindColorbuffer(Uint32 textureSlot);
	//void bindDepthStencilbuffer(Uint32 textureSlot);
	void draw();

	Uint32 width, height;

	private:
	Uint32 frame, color, depthStencil;
	Shader program;
	VertexBuffer buffer;
};

//Opengl shader storage buffer.
struct SSBO{
	SSBO(){};
	void init(Uint32 base, Uint32 length, char* data);
	~SSBO();

	void bind();
	void write(Uint32 offset, Uint32 length, char* data);
	void bindBase(Uint32 base);

	private:
	Uint32 buffer;
};

//Uniform block.
struct UniformBlock{
	Mat4 projView;
	float time;
};

//Opengl uniform buffer.
struct UniformBuffer{
	UniformBuffer(){};
	void init(Uint32 base);
	~UniformBuffer();

	void bind();
	void write();
	void bindBase(Uint32 base);

	UniformBlock block;

	private:
	Uint32 buffer;
};

//Point light data.
struct PointLightData{
	PointLightData(){};
	PointLightData(Vec3 ambient, Vec3 diffuse, Vec3 position, float radius);
	~PointLightData(){};

	Vec3 ambient;
		char padding0[4];
	Vec3 diffuse;
		char padding1[4];
	Vec3 position;
	float radius;
};

//Light data for light storage.
struct LightData{
	Vec3 sunDirection;
		char padding0[4];
	Vec3 sunAmbient;
		char padding1[4];
	Vec3 sunDiffuse;
		char padding2[4];
	Mat4 sunView[3];
	float numPoints;
		char padding[12];
	PointLightData points[MAX_POINT_LIGHTS];
};

//Lighting pass render target. 
struct DTarget{
	DTarget(){};
	void init(Uint32 width, Uint32 height);
	~DTarget();

	void bind();
	void draw();

	Uint32 width, height;

	private:
	Uint32 gBuffer, gPosition, gNormal, gAlbedo;
	Uint32 depthStencil;
	Shader program;
	VertexBuffer buffer;
};

//Shadow map
struct ShadowMap{
	ShadowMap(){};
	void init(Uint32 width, Uint32 height, Uint32 numMaps);
	~ShadowMap();

	void bind();
	void bindShadow(Uint32 index);

	Uint32 width, height, numMaps;

	private:
	Uint32 frame, depth;
};

//Light storage.
struct LightStorage{
	LightStorage(){};
	void init(Uint32 base, Uint32 mapWidth, Uint32 mapHeight);
	~LightStorage(){};

	void staticPush(PointLightData* data, Uint32 numLights);

	void reset();
	void push(PointLightData data);
	void bind();
	void write();

	LightData lights;
	ShadowMap shadowMap;

	private:
	Uint32 staticPoints;
	SSBO storage;
};
