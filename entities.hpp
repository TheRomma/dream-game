#pragma once

#include "3Dmaths.hpp"
#include "3Dphysics.hpp"
#include "models.hpp"
#include "system.hpp"
#include "graphics.hpp"

#include <string>

#define GRAVITY -9.81
#define ANGLE_THRESHOLD 0.7

struct C_Physics{
	C_Physics(){};
	C_Physics(float radius, float aspect, Vec3 centerPos);
	~C_Physics(){};

	void updateGravity(float delta);
	void handleGravity(float delta);
	void handleCollision(CollisionHandler& handler, PhysicsMesh& mesh);

	float gravVelocity;
	BoundingSphere collider;
	bool onGround;
};

struct Player{
	Player(){};
	void init(Vec3 position, float yaw);
	~Player(){};

	void input(float delta, Keyboard& kb);
	void update(float delta, CollisionHandler& handler, PhysicsMesh& mesh);

	Vec3 position;
	C_Physics physics;
	Camera camera;
	float runBonus;
};

struct Level{
	Level(){};
	void init(std::string filename);
	~Level(){};

	void draw();
	void drawSunShadows();

	StaticModel model;
	PhysicsMesh mesh;
};
