#pragma once

#include "resources.hpp"
#include "graphics.hpp"
#include "3Dphysics.hpp"

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

//Static single mesh model
struct StaticModel{
	StaticModel(){};
	void init(const char* filename);
	~StaticModel(){};

	void draw(Mat4 model);

	private:
	Shader shader;
	VertexBuffer buffer;
	Texture texture;
};

//Animated single mesh model
struct AnimatedModel{
	AnimatedModel(){};
	void init(const char* filename);
	~AnimatedModel();

	void pose(Animation& anim, float time);
	void draw(Mat4 model);

	private:
	Uint32 numBones;
	Mat4* joints;

	Shader shader;
	VertexBuffer buffer;
	Texture texture;
};

//Static multi mesh model
struct MultiModel{
	MultiModel(){};
	void init(const char* filename);
	~MultiModel(){};

	void draw(Mat4 model);
	void drawSunShadows(Mat4 model);

	private:
	Shader shader, sunShadowProgram;
	VertexBuffer buffer;
	ArrayTexture texture;
};

//Skybox.
struct Skybox{
	Skybox(){};
	void init(const char* filename);
	~Skybox(){};

	void draw();

	private:
	Shader program;
	VertexBuffer buffer;
	Texture texture;
};
