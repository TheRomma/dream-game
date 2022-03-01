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
	player.init(Vec3(2,8,1), -1.57);

	Level level;
	level.init("res/tech_demo");

	AnimatedModel aModel;
	aModel.init("res/animated_multi_model_test.am");

	StaticModel ball_0;
	ball_0.init("res/steel_ball.sm");

	Animation anim;
	anim.init("res/animated_multi_test.ad");
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

	//EnvironmentMap skybox;
	//skybox.init("res/test_skybox.em");

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
						}else{
							fullscreenMode = true;
							window->fullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP);
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
					if(event.key.keysym.scancode == SDL_SCANCODE_V){
						if(updateFlashlight){
							updateFlashlight = false;
						}else{
							updateFlashlight = true;
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
		
		animTimer += delta * 0.5;
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
		//lights.block.sun.direction = Vec3(cos(timer), sin(timer), 1.0);

		Mat4 proj = Mat4::perspective(3.14 / 2, window->getAspect(), 0.01, 10.0);
		uniform.block.projView = player.camera.getView() * proj;
		uniform.block.time = timer;

		if(updateFlashlight){
			flashlightPos = player.position + Vec3(0,0,2);
			flashlightDir = player.camera.direction;
		}

		Spotlight plight;
		plight.position = flashlightPos;
		plight.radius = 50;
		plight.direction = flashlightDir;
		plight.cutOff = cos(0.9);
		//plight.ambient = Vec3(0.0,0.0,0.0);
		plight.diffuse = Vec3(8.0, 0.0, 0.0);
		lights.push(plight);

		Spotlight plight1;
		plight1.position = Vec3(6,6,3);
		plight1.radius = 50;
		plight1.direction = Vec3(-1,-1,0.5);
		plight1.cutOff = cos(0.785);
		//plight1.ambient = Vec3(0.0,0.0,0.0);
		plight1.diffuse = Vec3(8.0, 10.0, 0.0);
		lights.push(plight1);

		Spotlight spots;
		spots.position = Vec3(10, 14, 2);
		spots.radius = 30;
		spots.direction = Vec3(1.5, 1, 1);
		spots.cutOff = cos(0.785);
		spots.diffuse = Vec3(5, 0, 0);
		lights.push(spots);

		spots.diffuse = Vec3(0, 5, 0);
		spots.position = Vec3(10, 26, 2);
		spots.direction = Vec3(1.5, -1, 1);
		lights.push(spots);

		spots.diffuse = Vec3(0, 0, 5);
		spots.position = Vec3(10, 14, 8);
		spots.direction = Vec3(1.5, 1, -1);
		lights.push(spots);

		spots.diffuse = Vec3(2, 2, 2);
		spots.position = Vec3(10, 26, 8);
		spots.direction = Vec3(1.5, -1, -1);
		lights.push(spots);


		uniform.write();
		lights.write();

		Vec3 ballPos_0 = Vec3(20, 20, 2) + Vec3(sin(timer)*5, cos(timer)*4, sin(timer*1.2));
		Vec3 ballPos_1 = Vec3(20, 20, 6) + Vec3(sin(timer*2)*3, cos(timer*2)*4, cos(timer*1.4));

		level.draw();
		aModel.draw(Mat4::translation(2,2,0));
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
		aModel.drawShadow(Mat4::translation(2,2,0), lights);
		ball_0.drawShadow(Mat4::translation(ballPos_0), lights);
		ball_0.drawShadow(Mat4::translation(ballPos_1), lights);
		//Display ------------------------------
		lights.bindShadowMap();
		lights.bindEnvironmentMap();
		//skybox.bind(4);
		target->bindDisplay();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		//skybox.display();
		lights.displayEnvironmentMap();
		target->draw();

		target->display(window->width, window->height);

		window->swap();
	}
	return nextLayer;
}
