#pragma once

#include "entities.hpp"
#include "graphics.hpp"
#include "system.hpp"

struct L_Game{
	L_Game();
	~L_Game(){};

	void update(float delta);
	void draw(float aspect);
	void drawShadowMap();

	CommonUniforms uniform;
	Keyboard kb;
	Lighting lights;
	CollisionHandler physics;
	Player player;
	Level level;
	float timer;
	Skybox sky;

	AnimatedModel animModel;
	Animation anim;
	float animTimer;

	bool alive;
	//L_Game* nextLayer = nullptr;
};
