#include "system.hpp"

//Camera init.
void Camera::init(Vec3 position, float yaw, float pitch, float sensitivity){
	this->position = position;
	this->yaw = yaw;
	this->pitch = pitch;
	this->sensitivity = sensitivity;
	mouseUpdate(0.0, 0.0);
	frustum = BoundingConvex(corners, 8);
}

//Update direction with mouse motion.
void Camera::mouseUpdate(float Xrelative, float Yrelative){
	yaw -= Xrelative * sensitivity;
	pitch -= Yrelative * sensitivity;
	
	if(pitch > 1.56){pitch = 1.56;}
	else if(pitch < -1.56){pitch = -1.56;}

	direction.x = cos(yaw) * cos(pitch);
	direction.z = sin(pitch);
	direction.y = sin(yaw) * cos(pitch);
}

//Construct a view matrix.
Mat4 Camera::getView(){
	return Mat4::lookAt(position, position + direction, Vec3(0,0,1));
}

//Construct a view matrix.
Mat4 Camera::getOriginView(){
	return Mat4::lookAt(Vec3(0,0,0), direction, Vec3(0,0,1));
}

//Construct a right vector.
Vec3 Camera::getRight(){
	return Vec3::normalize(Vec3::cross(direction, Vec3(0,0,1)));
}

//Construct a xy plane front vector.
Vec3 Camera::getFront(){
	return Vec3::normalize(Vec3(direction.x, direction.y, 0.0));
}

void Camera::updateFrustum(Mat4 invProjView){
	corners[0] = Vec3(-1, -1, -1);
	corners[1] = Vec3( 1, -1, -1);
	corners[2] = Vec3( 1,  1, -1);
	corners[3] = Vec3(-1,  1, -1);
	corners[4] = Vec3(-1, -1,  1);
	corners[5] = Vec3( 1, -1,  1);
	corners[6] = Vec3( 1,  1,  1);
	corners[7] = Vec3(-1,  1,  1);

	float w = 0;
	for(unsigned int i=0;i<8;i++){
		corners[i] = invProjView.transform(corners[i], 1.0, w);
		corners[i] = corners[i] / w;
		corners[i].print();
	}
	std::cout<<std::endl;
}

//--------------------------------------------------------------------------------------------------------

//Clock constructor.
Clock::Clock(){
	now = SDL_GetPerformanceCounter();
	prev = now;
	dt = 1/144;
}

//Update clock.
void Clock::update(){
	now = SDL_GetPerformanceCounter();
	dt = (now - prev) / SDL_GetPerformanceFrequency();
	prev = now;
}

//--------------------------------------------------------------------------------------------------------

//SDL Window wrapper init.
bool Window::init(const char* title, Uint32 width, Uint32 height, Uint32 flags){
	this->width = width;
	this->height = height;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL|flags);

	context = SDL_GL_CreateContext(window);

	glewExperimental = true;
	if(glewInit() != GLEW_OK){
		return false;
	}

	return true;
}

//Window wrapper destructor.
Window::~Window(){
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
}

//Enable vsync. 0 = off, 1 = on, -1 = adaptive vsync.
void Window::vsync(Uint32 setting){
	SDL_GL_SetSwapInterval(setting);
}

//Enable fullscreen.
void Window::fullscreen(Uint32 setting){
	SDL_SetWindowFullscreen(window, setting);
}

//Swap buffers in the swapchain.
void Window::swap(){
	SDL_GL_SwapWindow(window);
}

//Bind the default framebuffer AKA this windows framebuffer.
void Window::bind(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//Return window aspect ratio.
float Window::getAspect(){
	return (float)width / (float)height;
}

//Window has been resized.
void Window::eventResized(){
	SDL_GetWindowSize(window, (int*)&width, (int*)&height);
}

//----------------------------------------------------------------------------------------------------------

//SDL keyboard state wrapper.
void Keyboard::init(){
	state = SDL_GetKeyboardState(NULL);
}

//Check if keyboard key is pressed.
bool Keyboard::keyPressed(SDL_Scancode key){
	return state[key];
}
