#include "game.hpp"

Uint32 startLayer(Uint32 type, Window* window, DeferredTarget* target){
	switch(type){
		case LAYER_TEST:
			return L_Test(window, target);

		default:
			return 0;
	}
}

Uint32 L_Test(Window* window, DeferredTarget* target){
	SDL_SetRelativeMouseMode(SDL_TRUE);
	bool mouseMode = true;
	CommonUniforms uniform;
	bool fullscreenMode = false;

	Keyboard kb;
	kb.init();

	LightUniforms lights(1024, 1024, "res/test_skybox.em");
	lights.block.sun.direction = Vec3::normalize(Vec3(-2.5,4,3));
	lights.block.sun.ambient = Vec3(0.7,0.7,0.7);
	lights.block.sun.diffuse = Vec3(8.0,8.0,7.0);
	lights.write();
	
	CollisionHandler physics;

	Player player;
	player.init(Vec3(2,8,1), 1.57);

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

	bool updateFlashlight = true;
	Vec3 flashlightPos = Vec3(0.0, 0.0, 0.0);
	Vec3 flashlightDir = Vec3(0.0, 0.0, 0.0);

	float delta = 0.01;

	Spotlight spot[10];

	spot[0].position = Vec3(10, 14, 2);
	spot[0].radius = 30;
	spot[0].direction = Vec3(1.5, 1, 1);
	spot[0].cutOff = cos(0.8);
	spot[0].diffuse = Vec3(5, 0, 0);

	spot[1] = spot[0];
	spot[1].diffuse = Vec3(0, 5, 0);
	spot[1].position = Vec3(10, 26, 2);
	spot[1].direction = Vec3(1.5, -1, 1);

	spot[2] = spot[0];
	spot[2].diffuse = Vec3(0, 0, 5);
	spot[2].position = Vec3(10, 14, 8);
	spot[2].direction = Vec3(1.5, 1, -1);

	spot[3] = spot[0];
	spot[3].diffuse = Vec3(2, 2, 2);
	spot[3].position = Vec3(10, 26, 8);
	spot[3].direction = Vec3(1.5, -1, -1);

	spot[4] = spot[0];
	spot[4].diffuse = Vec3(3, 3, 3);
	spot[4].position = Vec3(-38, 62, 12);
	spot[4].direction = Vec3(1, 1, -2);

	spot[5] = spot[0];
	spot[5].diffuse = Vec3(5, 0, 0);
	spot[5].position = Vec3(-38, 82, 12);
	spot[5].direction = Vec3(1, -1, -2);

	spot[6] = spot[0];
	spot[6].diffuse = Vec3(0, 5, 0);
	spot[6].position = Vec3(-17, 82, 12);
	spot[6].direction = Vec3(0, -1, -2);

	spot[7] = spot[0];
	spot[7].diffuse = Vec3(0, 0, 5);
	spot[7].position = Vec3(-2, 82, 12);
	spot[7].direction = Vec3(-1, -1, -2);

	spot[8] = spot[0];
	spot[8].diffuse = Vec3(4, 4, 0);
	spot[8].position = Vec3(-4, 67.5, 6);
	spot[8].direction = Vec3(1, 1, 1);

	spot[9] = spot[0];
	spot[9].radius = 40;
	spot[9].cutOff = cos(0.8);
	spot[9].diffuse = Vec3(5, 3, 0);
	spot[9].position = Vec3(-34, 12, 8);
	spot[9].direction = Vec3(-1.5, 1, 0.5);

	lights.pushStatic(nullptr, 0, spot, 10);

	while(alive){
		//Input -------------------------------------------------------------------------
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT:
					alive = false;
					break;
				
				case SDL_WINDOWEVENT:
					if(event.window.event == SDL_WINDOWEVENT_RESIZED){
						window->eventResized();
					}
					break;

				case SDL_MOUSEMOTION:
					mouseX = event.motion.xrel;
					mouseY = event.motion.yrel;
					
					if(mouseY > 50|mouseY < -50){mouseY = 0;}
					if(mouseX > 50|mouseX < -50){mouseX = 0;}

					player.camera.mouseUpdate(mouseX, mouseY);
					break;

				case SDL_KEYUP:
					if(event.key.keysym.scancode == SDL_SCANCODE_F){
						if(fullscreenMode){
							fullscreenMode = false;
							window->fullscreen(0);
							window->eventResized();
						}else{
							fullscreenMode = true;
							window->fullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP);
							window->eventResized();
						}
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
					break;
			}
		}

		//Update -------------------------------------------------------------------------
		clock.update();
		delta = clock.dt;
		if(delta > 0.1){
			delta = 0.1;
		}

		timer += delta;
		player.input(delta, kb);
		player.update(delta, physics, level.mesh);
		uniform.block.position = player.camera.position;
		
		animTimer += delta;
		if(animTimer >= anim.duration){
			animTimer = 0;
		}
		aModel.pose(anim, animTimer);

		//Draw ---------------------------------------------------------------------------

		//Gbuffer ------------------------------
		target->bind();

		lights.reset();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		Mat4 proj = Mat4::perspective(3.14 / 2, window->getAspect(), 0.01, 10.0);
		uniform.block.projView = player.camera.getView() * proj;
		uniform.block.time = timer;


		uniform.write();
		lights.write();

		Vec3 ballPos_0 = Vec3(20, 20, 2) + Vec3(sin(timer)*5, cos(timer)*4, sin(timer*1.2));
		Vec3 ballPos_1 = Vec3(20, 20, 6) + Vec3(sin(timer*2)*3, cos(timer*2)*4, cos(timer*1.4));

		level.draw();
		aModel.draw(animated_transforms);
		ball_0.draw(Mat4::translation(ballPos_0));
		ball_0.draw(Mat4::translation(ballPos_1));

		//Shadows ------------------------------
		lights.bind();
		lights.bindShadowFrame();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		Uint32 offset = 0;
		for(unsigned int i=0;i<SUN_NUM_SHADOW_CASCADES;i++){
			lights.shadows.bindLayer(i);
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		offset += SUN_NUM_SHADOW_CASCADES;
		for(unsigned int i=0;i<lights.block.numSpotlights;i++){
			lights.shadows.bindLayer(offset + i);
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		Vec3 front = player.camera.direction;

		lights.calcShadowProjections(player.position);

		lights.write();

		level.drawSunShadows(lights);
		aModel.drawShadow(animated_transforms, lights);
		ball_0.drawShadow(Mat4::translation(ballPos_0), lights);
		ball_0.drawShadow(Mat4::translation(ballPos_1), lights);
		//Display ------------------------------
		lights.bindShadowMap();
		lights.bindEnvironmentMap();
		target->bindDisplay();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		lights.displayEnvironmentMap();
		target->draw();

		target->display(window->width, window->height);

		window->swap();
	}
	return nextLayer;
}
