#include "entities.hpp"

C_Physics::C_Physics(float radius, float aspect, Vec3 centerPos){
	velocity.z = 0;
	onGround = false;
	collider = SweptSphere(centerPos, radius, aspect);
}

void C_Physics::updateGravity(float delta){
	velocity.z += GRAVITY * delta;
}

void C_Physics::handleGravity(float delta){
	collider.getNext()->center.z += velocity.z * delta * 2.5;
}

void C_Physics::handleCollision(PhysicsMesh& mesh){
	Vec3 initDir = Vec3::cross(Vec3::normalize(Vec3(velocity.x+0.01, velocity.y, 0.0)), Vec3(0.0, 0.0, 1.0));
	float distance = 0.0;
	Vec3 normal(0.0, 0.0, 0.0);
	for(int i=0;i<mesh.numConvexes;i++){
		if(gjk(collider, mesh.convexes[i], normal, distance, initDir)){
			velocity.x -= velocity.x * fabs(normal.x);
			velocity.y -= velocity.y * fabs(normal.y);
			if(normal.z > ANGLE_THRESHOLD && velocity.z < 0){
				velocity.z = 0;
				onGround = true;
			}else if(normal.z < -ANGLE_THRESHOLD && velocity.z > 0){
				velocity.z = 0;
			}else{
				velocity.z -= velocity.z * normal.z * normal.z * 0.03;
			}
			collider.getNext()->center = collider.getNext()->center + normal * distance;
			collider.getPrev()->center = collider.getPrev()->center + normal * distance;
		}
	}
}

void C_Physics::update(float delta){
	velocity.z += GRAVITY * delta;
	velocity.z = fmin(velocity.z, 50.0);
	collider.getNext()->center = collider.getPrev()->center + velocity * delta;
	//velocity.x -= velocity.x * delta * 20;
	//velocity.y -= velocity.y * delta * 20;

	onGround = false;
}

void Player::init(Vec3 position){
	this->position = position;
	physics = C_Physics(1.2, 1.65, position + Vec3(0,0,1));
	runBonus = 0.0;
}

void Player::input(float delta, Keyboard& kb, Vec3 camRight, Vec3 camFront){
	if(kb.keyPressed(SDL_SCANCODE_LSHIFT)){runBonus = 4.0;}
	else{runBonus = 0.0;}

	physics.velocity.x = 0.0;
	physics.velocity.y = 0.0;
	if(kb.keyPressed(SDL_SCANCODE_W)){physics.velocity = physics.velocity + camFront * ((4 + runBonus));}
	if(kb.keyPressed(SDL_SCANCODE_S)){physics.velocity = physics.velocity - camFront * ((4 + runBonus));}
	if(kb.keyPressed(SDL_SCANCODE_A)){physics.velocity = physics.velocity - camRight * ((4 + runBonus));}
	if(kb.keyPressed(SDL_SCANCODE_D)){physics.velocity = physics.velocity + camRight * ((4 + runBonus));}
	
	if(physics.onGround){
		if(kb.keyPressed(SDL_SCANCODE_SPACE)){physics.velocity.z = 16.0;}
	}
}

void Player::update(float delta, PhysicsMesh& mesh, Renderer* renderer){
	physics.collider.swapSpheres();

	physics.update(delta);
	physics.handleCollision(mesh);

	position = physics.collider.getNext()->center + Vec3(0,0,-1);
	renderer->uniforms.common.camPosition = position + Vec3(0,0,1.8);
}

void Level::init(std::string filename){
	model.init((filename + ".sm").c_str());
	mesh.init((filename + ".pm").c_str());
}
