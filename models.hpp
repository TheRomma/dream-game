#pragma once

#include "3Dphysics.hpp"
#include "shader.hpp"
#include "graphics.hpp"

//Joint.
struct Joint{
	Vec3 translation;
	Quat rotation;
	Vec3 scaling;
};

//Skeletal animation.
struct Animation{
	Animation(){};
	void init(const char* filename);
	~Animation();

	Mat4 calcJoint(Uint32 bone, float time);

	float duration;

	//private:
	Uint32 numFrames;
	Uint32 numBones;
	Uint32 animRate;
	
	Joint* transforms;
};

//A drawable 3d model.
struct StaticModel{
	StaticModel(){};
	void init(const char* filename);
	~StaticModel(){};

	void draw(Mat4 model);
	void drawShadow(Mat4 model);

	private:
	Shader gProgram, shadowProgram;
	VertexBuffer buffer;
	Material material;
};

//A drawable 3d model.
struct AnimatedModel{
	AnimatedModel(){};
	void init(const char* filename);
	~AnimatedModel(){};

	void pose(Animation& anim, float time);
	void draw(Mat4 model);
	void drawShadow(Mat4 model);

	private:
	Uint32 numBones;
	Mat4* joints;

	Shader gProgram, shadowProgram;
	VertexBuffer buffer;
	Material material;
};

//Physics mesh
struct PhysicsMesh{
	PhysicsMesh(){};	
	void init(const char* filename);
	~PhysicsMesh();	

	Uint32 numConvexes;
	Vec3* vertices = nullptr;
	Uint16* indices;
	BoundingConvex* convexes;
};
