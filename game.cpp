#include "game.hpp"

Uint32 startLayer(Uint32 type, Renderer* renderer){
	switch(type){
		case LAYER_TEST:
			return L_Test(renderer);

		default:
			return 0;
	}
}

Uint32 L_Test(Renderer* renderer){
	Uint64 frameStart, frameEnd;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	bool mouseMode = true;
	bool fullscreenMode = false;

	Keyboard kb;
	kb.init();

	EnvironmentMap emap;
	emap.init("res/test_skybox.em");

	renderer->uniforms.lights.sun.direction = Vec3::normalize(Vec3(-2.5,4,3));
	renderer->uniforms.lights.sun.ambient = Vec3(0.7,0.7,0.7);
	renderer->uniforms.lights.sun.diffuse = Vec3(8.0,8.0,7.0);
	renderer->uniforms.lights.exposure = 1.2;
	renderer->setCameraView(1.57, 0.0);

	Player player;
	player.init(Vec3(2,8,1));

	Level level;
	level.init("res/tech_demo");

	AnimatedModel aModel;
	aModel.init("res/animated_demo.am");

	Mat4 animated_transforms = (Quat(3.14, Vec3(0,0,1))).toMatrix() * Mat4::translation(-38, 14, 4);

	StaticModel ball_0;
	ball_0.init("res/steel_ball.sm");

	Animation anim;
	anim.init("res/animation_demo.ad");
	float animTimer = 0;

	float timer = 0;

	float mouseX = 0;
	float mouseY = 0;
	Clock clock;
	bool alive = true;
	SDL_Event event;
	Uint32 nextLayer = 0;
	float delta = 0.01;

	while(alive){
		frameStart = SDL_GetPerformanceCounter();
		//Input -------------------------------------------------------------------------
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT:
					alive = false;
					break;
				
				case SDL_MOUSEMOTION:
					mouseX = event.motion.xrel;
					mouseY = event.motion.yrel;
					
					if(mouseY > 50|mouseY < -50){mouseY = 0;}
					if(mouseX > 50|mouseX < -50){mouseX = 0;}

					renderer->updateCameraView(mouseX, mouseY);
					break;

				case SDL_KEYUP:
					if(event.key.keysym.scancode == SDL_SCANCODE_F){
						renderer->toggleWindowFullscreen();
					}
					if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
						alive = false;
					}
					if(event.key.keysym.scancode == SDL_SCANCODE_R){
						alive = false;
						nextLayer = LAYER_TEST;
					}
					if(event.key.keysym.scancode == SDL_SCANCODE_M){
						if(mouseMode){
							mouseMode = false;
							SDL_SetRelativeMouseMode(SDL_FALSE);
						}else{
							mouseMode = true;
							SDL_SetRelativeMouseMode(SDL_TRUE);
						}
					}
					/*
					if(event.key.keysym.scancode == SDL_SCANCODE_V){
						player.camera.updateFrustum(renderer->uniforms.common.projView.inverse());
					}
					*/
					break;
			}
		}

		//Update -------------------------------------------------------------------------
		clock.update();
		delta = (delta + clock.dt) * 0.5;
		//delta = 0.05;
		if(delta > 0.1){
			delta = 0.1;
		}

		timer += delta;
		player.input(delta, kb, renderer->getCameraRight(), renderer->getCameraFront());
		player.update(delta, level.mesh, renderer);

		animTimer += delta;
		if(animTimer >= anim.duration){
			animTimer = 0;
		}

		Spotlight spot;
		spot.position = Vec3(10, 14, 2);
		spot.radius = 30;
		spot.direction = Vec3(1.5, 1, 1);
		spot.cutOff = cos(0.8);
		spot.diffuse = Vec3(5, 0, 0);
		renderer->pushLight(spot);

		spot.diffuse = Vec3(0, 5, 0);
		spot.position = Vec3(10, 26, 2);
		spot.direction = Vec3(1.5, -1, 1);
		renderer->pushLight(spot);

		spot.diffuse = Vec3(0, 0, 5);
		spot.position = Vec3(10, 14, 8);
		spot.direction = Vec3(1.5, 1, -1);
		renderer->pushLight(spot);

		spot.diffuse = Vec3(2, 2, 2);
		spot.position = Vec3(10, 26, 8);
		spot.direction = Vec3(1.5, -1, -1);
		renderer->pushLight(spot);

		spot.diffuse = Vec3(3, 3, 3);
		spot.position = Vec3(-38, 62, 12);
		spot.direction = Vec3(1, 1, -2);
		renderer->pushLight(spot);

		spot.diffuse = Vec3(5, 0, 0);
		spot.position = Vec3(-38, 82, 12);
		spot.direction = Vec3(1, -1, -2);
		renderer->pushLight(spot);

		spot.diffuse = Vec3(0, 5, 0);
		spot.position = Vec3(-17, 82, 12);
		spot.direction = Vec3(0, -1, -2);
		renderer->pushLight(spot);

		spot.diffuse = Vec3(0, 0, 5);
		spot.position = Vec3(-2, 82, 12);
		spot.direction = Vec3(-1, -1, -2);
		renderer->pushLight(spot);

		spot.diffuse = Vec3(4, 4, 0);
		spot.position = Vec3(-4, 67.5, 6);
		spot.direction = Vec3(1, 1, 1);
		renderer->pushLight(spot);

		spot.radius = 40;
		spot.cutOff = cos(0.8);
		spot.diffuse = Vec3(5, 3, 0);
		spot.position = Vec3(-34, 12, 8);
		spot.direction = Vec3(-1.5, 1, 0.5);
		renderer->pushLight(spot);

		//Draw ---------------------------------------------------------------------------
		//renderer->uniforms.common.projView = player.camera.getView() * renderer->getWindowProjection(1.5);

		renderer->uniforms.common.time = timer;

		Vec3 ballPos_0 = Vec3(20, 20, 2) + Vec3(sin(timer)*5, cos(timer)*4, sin(timer*1.2));
		Vec3 ballPos_1 = Vec3(20, 20, 6) + Vec3(sin(timer*2)*3, cos(timer*2)*4, cos(timer*1.4));

		renderer->drawModel(&level.model, Mat4::identity());
		renderer->drawModel(&aModel, animated_transforms, &anim, animTimer);
		renderer->drawModel(&ball_0, Mat4::translation(ballPos_0));
		renderer->drawModel(&ball_0, Mat4::translation(ballPos_1));
		//Display ------------------------------
		emap.bind(4);

		renderer->bindDisplay();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		emap.display();
		//sky.draw();

		renderer->displayFrame();

		frameEnd = SDL_GetPerformanceCounter();
		//SDL_Delay(floor(50.0 - ((frameEnd - frameStart)/(float)SDL_GetPerformanceFrequency()*1000)));
	}
	return nextLayer;
}
