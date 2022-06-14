#pragma once

#include "3Dmaths.hpp"
#include "3Dphysics.hpp"
#include "models.hpp"
#include "system.hpp"
#include "renderer.hpp"

#include <string>

#define GRAVITY -9.81 * 2
#define ANGLE_THRESHOLD 0.7

struct C_Physics{
	C_Physics(){};
	C_Physics(float radius, float aspect, Vec3 centerPos);
	~C_Physics(){};

	void updateGravity(float delta);
	void handleGravity(float delta);
	void handleCollision(PhysicsMesh& mesh);
	void update(float delta);

	Vec3 velocity;
	SweptSphere collider;
	bool onGround;
};

struct Player{
	Player(){};
	void init(Vec3 position);
	~Player(){};

	void input(float delta, Keyboard& kb, Vec3 right, Vec3 front);
	void update(float delta, PhysicsMesh& mesh, Renderer* renderer);

	Vec3 position;
	C_Physics physics;
	float runBonus;
};

struct Level{
	Level(){};
	void init(std::string filename);
	~Level(){};

	StaticModel model;
	PhysicsMesh mesh;
};
