#include "entities.hpp"

C_Physics::C_Physics(float radius, float aspect, Vec3 centerPos){
	gravVelocity = 0;
	onGround = false;
	collider = BoundingSphere(centerPos, radius, aspect);
}

void C_Physics::updateGravity(float delta){
	gravVelocity += GRAVITY * delta;
}

void C_Physics::handleGravity(float delta){
	collider.center.z += gravVelocity * delta * 2.5;
}

void C_Physics::handleCollision(CollisionHandler& handler, PhysicsMesh& mesh){
	float distance = 0;
	Vec3 normal(0,0,0);

	onGround = false;

	for(unsigned int i=0;i<mesh.numConvexes;i++){
		if(handler.gjk(collider, mesh.convexes[i])){
			handler.epa(collider, mesh.convexes[i], normal, distance);
			if(normal.z > ANGLE_THRESHOLD && gravVelocity < 0){
				gravVelocity = 0;
				onGround = true;
			}else if(normal.z < -ANGLE_THRESHOLD && gravVelocity > 0){
				gravVelocity = 0;
			}else{
				gravVelocity -= gravVelocity * normal.z * normal.z * 0.03;
			}
			collider.center = collider.center + normal * distance;
		}
	}
}

void Player::init(Vec3 position, float yaw){
	this->position = position;
	physics = C_Physics(1.2, 1.65, position + Vec3(0,0,1));
	camera.init(position + Vec3(0,0,1.8), yaw, 0.0, 0.002);
	runBonus = 0.0;
}

void Player::input(float delta, Keyboard& kb){
	if(kb.keyPressed(SDL_SCANCODE_LSHIFT)){runBonus = 4.0;}
	else{runBonus = 0.0;}

	Vec3 camRight = camera.getRight();
	Vec3 camFront = Vec3::normalize(Vec3(camera.direction.x, camera.direction.y, 0.0));
	if(kb.keyPressed(SDL_SCANCODE_W)){position = position + camFront * ((4 + runBonus) * delta);}
	if(kb.keyPressed(SDL_SCANCODE_S)){position = position - camFront * ((4 + runBonus) * delta);}
	if(kb.keyPressed(SDL_SCANCODE_A)){position = position - camRight * ((4 + runBonus) * delta);}
	if(kb.keyPressed(SDL_SCANCODE_D)){position = position + camRight * ((4 + runBonus) * delta);}
	
	if(physics.onGround){
		if(kb.keyPressed(SDL_SCANCODE_SPACE)){physics.gravVelocity = 8.0;}
	}
}

void Player::update(float delta, CollisionHandler& handler, PhysicsMesh& mesh){
	physics.collider.center = position + Vec3(0,0,1);

	physics.updateGravity(delta);
	physics.handleCollision(handler, mesh);
	physics.handleGravity(delta);

	position = physics.collider.center + Vec3(0,0,-1);
	camera.position = position + Vec3(0,0,1.8);
}

void Level::init(std::string filename){
	model.init((filename + ".mm").c_str());
	mesh.init((filename + ".pm").c_str());
}

void Level::draw(){
	model.draw(Mat4::identity());
}

void Level::drawSunShadows(){
	model.drawSunShadows(Mat4::identity());
}
