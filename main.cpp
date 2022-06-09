#define SDL_MAIN_HANDLED

#include "renderer.hpp"
#include "game.hpp"
#include "system.hpp"

#include <ctime>
#include <random>

int main(int argc, const char* argv[]){
	Renderer renderer;
	RendererSettings settings;
	settings.frameWidth = 1920;
	settings.frameHeight = 1080;
	//settings.windowVsync = 0;
	renderer.init(settings);

	Uint32 next = LAYER_TEST;
	while(next){
		next = startLayer(next, &renderer);
	}

	return 0;
}

