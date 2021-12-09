#include "game.hpp"

L_Game::L_Game(){
	target.init(320, 240);
	uniform.init();
	kb.init();
	player.init(Vec3(0,9,5), -3.14*0.0);
	level.init("res/industrial_test");
	timer = 0;
	alive = true;

	lights.init(512, 512);
	lights.block.sun.direction = Vec3(1,1,1);
	lights.block.sun.ambient = Vec3(0.1,0.2,0.1);
	lights.block.sun.diffuse = Vec3(1.7,0.5,0.5);
	lights.write();

	sky.init("res/sky_test.sk");
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

	timer += delta;
	player.input(delta, kb);
	player.update(delta, physics, level.mesh);
	uniform.block.position = player.camera.position;
}

void L_Game::draw(float aspect){
	lights.reset();
	target.bind();

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

	lights.block.sun.projViewCSM[0] = Mat4::lookAt(
	Vec3::normalize(lights.block.sun.direction) * 100 + (player.position),
	(player.position), Vec3(0,0,1)) * 
	Mat4::orthographic(-8, 8, -8, 8, 0.1, 200.0);

	lights.block.sun.projViewCSM[1] = Mat4::lookAt(
	Vec3::normalize(lights.block.sun.direction) * 100 + (player.position + front * 0),
	(player.position + front * 0), Vec3(0,0,1)) * 
	Mat4::orthographic(-16, 16, -16, 16, 0.1, 200.0);

	lights.block.sun.projViewCSM[2] = Mat4::lookAt(
	Vec3::normalize(lights.block.sun.direction) * 100 + (player.position + front * 0),
	(player.position + front * 0), Vec3(0,0,1)) * 
	Mat4::orthographic(-32, 32, -32, 32, 0.1, 200.0);

	lights.block.sun.projViewCSM[3] = Mat4::lookAt(
	Vec3::normalize(lights.block.sun.direction) * 100 + (player.position + front * 0),
	(player.position + front * 0), Vec3(0,0,1)) * 
	Mat4::orthographic(-64, 64, -64, 64, 0.1, 200.0);

	lights.write();

	level.drawSunShadows();
}
