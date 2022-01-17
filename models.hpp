#pragma once

#include "3Dphysics.hpp"
#include "shader.hpp"
#include "graphics.hpp"

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
