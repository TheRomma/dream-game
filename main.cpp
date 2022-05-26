#define SDL_MAIN_HANDLED

#include "renderer.hpp"

int main(int argc, const char* argv[]){
	RendererSettings settings;
	Renderer renderer;
	renderer.init(settings);

	bool alive = true;
	SDL_Event event;
	while(alive){
		SDL_PollEvent(&event);
		switch(event.type){
			case SDL_QUIT:
				alive = false;
				break;
		}
	}
	return 0;
}






























/*
#include "game.hpp"
#include "system.hpp"

#include <ctime>
#include <random>
int main(int argc, const char* argv[]){
	
	SDL_MessageBoxButtonData buttons[] = {
		{0, 0, "640x480"},
		{0, 1, "1280x720"},
		{0, 2, "1920x1080"},
		{0, 3, "2560x1440"}
	};

	SDL_MessageBoxData messageData = {
		SDL_MESSAGEBOX_INFORMATION,
		0,
		"Display",
		"Please choose the games internal resolution.",
		4,
		buttons,
		0
	};
	

	int buttonId = 0;
	Uint32 width = 1280;
	Uint32 height = 720;
	
	SDL_ShowMessageBox(&messageData, &buttonId);
	if(buttonId == 0){
		width = 640;
		height = 480;
	}else if(buttonId == 1){
		width = 1280;
		height = 720;
	}else if(buttonId == 2){
		width = 1920;
		height = 1080;
	}else if(buttonId == 3){
		width = 2560;
		height = 1440;
	}
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Controls", R"(
Movement: WASD
Run: Left Shift
Look around: Mouse
Jump: Spacebar
Toggle fullscreen: F
Detach cursor: M
Restart: R
Quit: ESC)", NULL);

	SDL_Init(SDL_INIT_EVERYTHING);
	Window window;
	if(!window.init("A tech demo by Jere Koivisto", width, height, SDL_WINDOW_RESIZABLE)){
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "OpenGL Error!", "Could not initiate an OpenGL context. Make sure your graphics device supports OpenGL 4.3.X or above.", NULL);
		SDL_Quit();
		return 0;
	}
	window.vsync(-1);

	DeferredTarget target(1920, 1080);

	Uint32 next = LAYER_TEST;
	while(next){
		next = startLayer(next, &window, &target);
	}

	SDL_Quit();
	return 0;
}
*/
