#pragma once

#include "3Dphysics.hpp"
#include "shader.hpp"

//Joint.
struct Joint{
	Vec3 translation;
	Quat rotation;
	Vec3 scaling;
};

//Skeletal animation.
struct Animation{
	Animation(){};
	bool init(const char* filename);
	~Animation();

	void calcJointTransforms(Mat4* joints, float time);

	float duration;

	Uint32 numFrames;
	Uint32 numBones;
	Uint32 animRate;
	
	Joint* transforms;
};

//A drawable 3d model.
struct StaticModel{
	StaticModel(){};
	bool init(const char* filename);
	~StaticModel();

	Uint32 numVertices;

	Shader gProgram, shadowProgram;
	Uint32 vao, vbo;
	Uint32 diffuse, metalRough;
};

//A drawable 3d model.
struct AnimatedModel{
	AnimatedModel(){};
	bool init(const char* filename);
	~AnimatedModel();

	Uint32 numVertices, numBones;

	Shader gProgram, shadowProgram;
	Uint32 vao, vbo;
	Uint32 diffuse, metalRough;
};

//Physics mesh
struct PhysicsMesh{
	PhysicsMesh(){};	
	bool init(const char* filename);
	~PhysicsMesh();	

	Uint32 numConvexes;
	Vec3* vertices = nullptr;
	Uint16* indices;
	BoundingConvex* convexes;
};
