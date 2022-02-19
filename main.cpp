#include "game.hpp"
#include "system.hpp"

#include <ctime>
#include <random>

int main(int argc, const char* argv[]){
	SDL_Init(SDL_INIT_EVERYTHING);
	Window window;
	window.init("Test title", 1280, 720, SDL_WINDOW_RESIZABLE);
	window.vsync(-1);

	DeferredTarget target(1920, 1080);

	Uint32 next = LAYER_TEST;
	while(next){
		next = startLayer(next, &window, &target);
	}

	SDL_Quit();
	return 0;
}
