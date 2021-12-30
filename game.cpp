#include "game.hpp"

Uint32 startLayer(Window* window, DeferredTarget* target){
	SDL_SetRelativeMouseMode(SDL_TRUE);
	CommonUniforms uniform;
	uniform.init();

	Keyboard kb;
	kb.init();

	Lighting lights;
	lights.init(512, 512);
	lights.block.sun.direction = Vec3(1,1,1);
	lights.block.sun.ambient = Vec3(0.05,0.0,0.05);
	lights.block.sun.diffuse = Vec3(0.2,0.1,0.5);
	lights.write();

	CollisionHandler physics;

	Player player;
	player.init(Vec3(0,9,5), -1.57);

	Level level;
	level.init("res/castle_level");

	float timer = 0;

	AnimatedModel animModel;
	animModel.init("res/test_walk_newer.am");

	Animation anim;
	anim.init("res/test_inflate.ad");

	float animTimer = 0;

	float mouseX = 0;
	float mouseY = 0;
	Clock clock;
	bool alive = true;
	SDL_Event event;
	Uint32 nextLayer = 0;

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
						window->fullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP);
					}
					if(event.key.keysym.scancode == SDL_SCANCODE_G){
						window->fullscreen(0);
					}
					if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
						alive = false;
					}
					if(event.key.keysym.scancode == SDL_SCANCODE_R){
						alive = false;
						nextLayer = 1;
					}
					break;
			}
		}

		//Update -------------------------------------------------------------------------
		clock.update();
		float delta = clock.dt;

		animTimer += delta;
		if(animTimer > anim.duration){animTimer = 0;}
		animModel.pose(anim, animTimer);

		timer += delta;
		player.input(delta, kb);
		player.update(delta, physics, level.mesh);
		uniform.block.position = player.camera.position;

		//Draw ---------------------------------------------------------------------------

		//Gbuffer ------------------------------
		target->bind();

		lights.reset();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		lights.block.sun.direction = Vec3(cos(timer), sin(timer), 1.0);

		Mat4 proj = Mat4::perspective(3.14 / 2, window->getAspect(), 0.01, 10.0);
		uniform.block.projView = player.camera.getView() * proj;
		uniform.block.time = timer;

		Spotlight plight;
		plight.position = player.position + Vec3(0,0,2);
		plight.radius = 20;
		plight.direction = player.camera.direction;
		plight.cutOff = 0.9;
		plight.ambient = Vec3(0.0,0.0,0.0);
		plight.diffuse = Vec3(0.0, 0.0, 0.0);
		lights.push(plight);

		uniform.write();
		lights.write();

		level.draw();

		animModel.draw(Mat4::identity());

		//Shadows ------------------------------
		lights.bind();
		lights.sunCSM.bind();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_DEPTH_BUFFER_BIT);

		Vec3 front = player.camera.direction;

		lights.sunCSM.calcProjViews(10, player.position, lights.block.sun.direction);
		lights.block.sun.projViewCSM[0] = lights.sunCSM.projView[0];
		lights.block.sun.projViewCSM[1] = lights.sunCSM.projView[1];
		lights.block.sun.projViewCSM[2] = lights.sunCSM.projView[2];
		lights.block.sun.projViewCSM[3] = lights.sunCSM.projView[3];

		lights.write();

		level.drawSunShadows();
		//Display ------------------------------
		lights.sunCSM.bindTexture(3);
		target->draw();

		target->display(window->width, window->height);

		window->swap();
	}
	return nextLayer;
}
