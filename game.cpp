#include "game.hpp"

L_Game::L_Game(){
	uniform.init();
	kb.init();
	player.init(Vec3(0,9,5), -3.14*0.0);
	level.init("res/industrial_test");
	timer = 0;
	alive = true;

	lights.init(512, 512);
	lights.block.sun.direction = Vec3(1,1,1);
	lights.block.sun.ambient = Vec3(0.2,0.25,0.2);
	lights.block.sun.diffuse = Vec3(1.7,0.5,0.5);
	lights.write();

	sky.init("res/sky_test.sk");

	animModel.init("res/test_walk_newer.am");
	anim.init("res/test_inflate.ad");
	animTimer = 0;
}

void L_Game::update(float dt){
	float delta = dt;
	if(delta > 0.1){delta = 0.1;}
	SDL_SetRelativeMouseMode(SDL_TRUE);

	if(kb.keyPressed(SDL_SCANCODE_ESCAPE)){alive = false;}
	/*
	if(kb.keyPressed(SDL_SCANCODE_R)){
		alive = false;
		nextLayer = new L_Game();
	}
	*/

	animTimer += delta;
	if(animTimer > anim.duration){animTimer = 0;}
	animModel.pose(anim, animTimer);

	timer += delta;
	player.input(delta, kb);
	player.update(delta, physics, level.mesh);
	uniform.block.position = player.camera.position;
}

void L_Game::draw(float aspect){
	lights.reset();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	lights.block.sun.direction = Vec3(cos(timer), sin(timer), 1.0);

	Mat4 proj = Mat4::perspective(3.14 / 2, aspect, 0.01, 10.0);
	uniform.block.projView = player.camera.getView() * proj;
	uniform.block.time = timer;

	/*
	Pointlight plight;
	plight.position = Vec3(0,0,2);
	plight.radius = 12;
	plight.ambient = Vec3(-1.0,-1.0,-1.0);
	plight.diffuse = Vec3(0.0, 0.0, 0.0);
	lights.push(plight);
	*/

	uniform.write();
	lights.write();

	level.draw();

	animModel.draw(Mat4::identity());

	proj = Mat4::perspective(3.14 / 4, aspect, 0.01, 10.0);
	uniform.block.projView = player.camera.getOriginView() * proj;
	uniform.write();
}

void L_Game::drawShadowMap(){
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
}
