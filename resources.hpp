#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include <SDL2/SDL.h>

//Static model
struct StaticModelFile{
	StaticModelFile(const char* filename);
	~StaticModelFile();

	Uint32 attribLength;
	char* attributes;

	Uint32 texWidth;
	Uint32 texHeight;
	Uint32 texChannels;
	Uint32 texFilter;

	Uint32 texLength;
	float* texture;
};

//Animated model
struct AnimatedModelFile{
	AnimatedModelFile(const char* filename);
	~AnimatedModelFile();

	Uint32 attribLength;
	char* attributes;

	Uint32 texWidth;
	Uint32 texHeight;
	Uint32 texChannels;
	Uint32 texFilter;

	Uint32 texLength;
	float* texture;

	Uint32 numBones;
};

//Multi mesh model
struct MultiModelFile{
	MultiModelFile(const char* filename);
	~MultiModelFile();

	void print();

	Uint32 attribLength;
	char* attributes;

	Uint32 texWidth;
	Uint32 texHeight;
	Uint32 texDepth;
	Uint32 texChannels;
	Uint32 texFilter;

	Uint32 texLength;
	float* texture;
};

//Physics mesh
struct PhysicsMeshFile{
	PhysicsMeshFile(const char* filename);
	~PhysicsMeshFile();

	Uint32 numConvexes;
	
	Uint32 vertsLength;
	char* vertices;

	Uint32 indsLength;
	char* indices;
};

//2d texture
struct TextureFile{
	TextureFile(const char* filename);
	~TextureFile();

	Uint32 texWidth;
	Uint32 texHeight;

	Uint32 texLength;
	float* texture;
};

//Skeletal animation file.
struct AnimationFile{
	AnimationFile(const char* filename);
	~AnimationFile();

	void print();

	Uint32 numFrames;
	Uint32 numBones;
	Uint32 animRate;

	Uint32 animLength;
	char* animation;
};
