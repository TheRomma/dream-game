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

	Mat4 calcJoint(Uint32 bone, float time);

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

	void draw(Mat4 model);
	void drawShadow(Mat4 model, Mat4 view);

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

	void pose(Animation& anim, float time);
	void draw(Mat4 model);
	void drawShadow(Mat4 model, Mat4 view);

	Uint32 numVertices, numBones;
	Mat4* joints;

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
