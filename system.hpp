#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "3Dmaths.hpp"
#include "3Dphysics.hpp"

//Camera
struct Camera{
	Camera(){};
	void init(Vec3 position, float yaw, float pitch, float sensitivity);
	~Camera(){};

	void mouseUpdate(float Xrelative, float Yrelative);
	Mat4 getView();
	Mat4 getOriginView();
	Vec3 getRight();
	Vec3 getFront();
	void updateFrustum(Mat4 invProjView);

	Vec3 position;
	Vec3 direction;
	float yaw, pitch, sensitivity;

	Vec3 corners[8];
	BoundingConvex frustum;
};

//Clock
struct Clock{
	Clock();
	~Clock(){};

	void update();

	float now, prev, dt;
};


//SDL Window & OpenGL context.
struct Window{
	Window(){};
	bool init(const char* title, Uint32 width, Uint32 height, Uint32 flags);
	~Window();

	void vsync(Uint32 setting);
	void fullscreen(Uint32 setting);
	void swap();
	void bind();

	float getAspect();

	void eventResized();

	Uint32 width, height;

	private:
	SDL_Window* window;
	SDL_GLContext context;
};

//SDL Keyboard state.
struct Keyboard{
	Keyboard(){};
	void init();
	~Keyboard(){};

	bool keyPressed(SDL_Scancode key);

	const Uint8* state;
};
