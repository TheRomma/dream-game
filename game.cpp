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
	SDL_SetRelativeMouseMode(SDL_TRUE);
	bool mouseMode = true;
	bool fullscreenMode = false;

	Keyboard kb;
	kb.init();

	EnvironmentMap emap;
	emap.init("res/test_skybox.em");
	//CSM shadows;
	//shadows.init(1024, 1024, NUM_SUN_CASCADES + MAX_SPOTLIGHTS);

	renderer->uniforms.lights.sun.direction = Vec3::normalize(Vec3(-2.5,4,3));
	renderer->uniforms.lights.sun.ambient = Vec3(0.7,0.7,0.7);
	renderer->uniforms.lights.sun.diffuse = Vec3(8.0,8.0,7.0);
	renderer->updateLights();

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

	bool bloomOn = true;

	BoundingSphere aModelBounds(Vec3(-38, 14, 4), 1.0, 1.0);

	ProceduralSky sky;
	sky.init();

	while(alive){
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

					player.camera.mouseUpdate(mouseX, mouseY);
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
					if(event.key.keysym.scancode == SDL_SCANCODE_V){
						player.camera.updateFrustum(renderer->uniforms.common.projView.inverse());
					}
					break;
			}
		}

		//Update -------------------------------------------------------------------------
		clock.update();
		delta = (delta + clock.dt) * 0.5;
		if(delta > 0.1){
			delta = 0.1;
		}

		timer += delta;
		player.input(delta, kb);
		player.update(delta, physics, level.mesh);
		renderer->uniforms.common.camPosition = player.camera.position;

		animTimer += delta;
		if(animTimer >= anim.duration){
			animTimer = 0;
		}
		aModel.pose(anim, animTimer);


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

		//Gbuffer ------------------------------
		renderer->bindGBuffer();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		renderer->uniforms.common.projView = player.camera.getView() * renderer->getWindowProjection(1.7);

		renderer->uniforms.common.time = timer;

		renderer->updateCommons();

		Vec3 ballPos_0 = Vec3(20, 20, 2) + Vec3(sin(timer)*5, cos(timer)*4, sin(timer*1.2));
		Vec3 ballPos_1 = Vec3(20, 20, 6) + Vec3(sin(timer*2)*3, cos(timer*2)*4, cos(timer*1.4));

		level.draw();
		if(physics.gjk(player.camera.frustum, aModelBounds)){
			aModel.draw(animated_transforms);
		}
		ball_0.draw(Mat4::translation(ballPos_0));
		ball_0.draw(Mat4::translation(ballPos_1));

		//Shadows ------------------------------
		renderer->bindShadowFrame();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		Vec3 front = player.camera.direction;

		renderer->clearShadows();
		renderer->updateLights();

		Uint32 mapOffset = 0;
		for(unsigned int i=0;i<NUM_SUN_CASCADES;i++){
			renderer->bindShadowLayer(i);
			level.drawSunShadows(renderer->uniforms.lights.sun.projViewCSM[i]);
			aModel.drawShadow(animated_transforms, renderer->uniforms.lights.sun.projViewCSM[i]);
			ball_0.drawShadow(Mat4::translation(ballPos_0), renderer->uniforms.lights.sun.projViewCSM[i]);
			ball_0.drawShadow(Mat4::translation(ballPos_1), renderer->uniforms.lights.sun.projViewCSM[i]);
		}
		mapOffset += NUM_SUN_CASCADES;
		for(unsigned int i=0;i<renderer->uniforms.lights.numSpotlights;i++){
			renderer->bindShadowLayer(mapOffset + i);
			level.drawSunShadows(renderer->uniforms.lights.spotlights[i].projViewCSM);
			aModel.drawShadow(animated_transforms, renderer->uniforms.lights.spotlights[i].projViewCSM);
			ball_0.drawShadow(Mat4::translation(ballPos_0), renderer->uniforms.lights.spotlights[i].projViewCSM);
			ball_0.drawShadow(Mat4::translation(ballPos_1), renderer->uniforms.lights.spotlights[i].projViewCSM);
		}


		//Display ------------------------------
		emap.bind(4);

		renderer->bindDisplay();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		emap.display();
		//sky.draw();
		renderer->deferredPass();

		renderer->displayFrame();
	}
	return nextLayer;
}
