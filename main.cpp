#include "model.hpp"
#include "entities.hpp"
#include "game.hpp"
#include "system.hpp"

#include <ctime>
#include <random>

//#define DEBUG_NEW new(__FILE__, __LINE__)
//#define new DEBUG_NEW


int main(){
	SDL_Init(SDL_INIT_EVERYTHING);

	Window window;
	window.init("Test title", 1280, 720, SDL_WINDOW_RESIZABLE);
	window.vsync(-1);

	bool fullscreen = false;

	float mouseX = 0;
	float mouseY = 0;

	Clock clock;

	L_Game* layer = new L_Game();

	bool alive = true;

	SDL_Event event;

	while(alive){
		//Input
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT:
					alive = false;
					break;

				case SDL_WINDOWEVENT:
					if(event.window.event == SDL_WINDOWEVENT_RESIZED){
						window.eventResized();
					}
					break;

				case SDL_MOUSEMOTION:
					mouseX = event.motion.xrel;
					mouseY = event.motion.yrel;
					
					if(mouseY > 50|mouseY < -50){mouseY = 0;}
					if(mouseX > 50|mouseX < -50){mouseX = 0;}

					layer->player.camera.mouseUpdate(mouseX, mouseY);
					break;

				case SDL_KEYUP:
					if(event.key.keysym.scancode == SDL_SCANCODE_F){
						if(!fullscreen){
							window.fullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP);
							fullscreen = true;
						}else{
							window.fullscreen(0);
							fullscreen = false;
						}
					}
					if(event.key.keysym.scancode == SDL_SCANCODE_R){
						layer->nextLayer = new L_Game();
						layer->alive = false;
					}
					break;
			}
		}

		//Update
		clock.update();

		layer->update(clock.dt);
		if(!layer->alive){
			if(!layer->nextLayer){
				delete layer;
				alive = false;
			}else{
				L_Game* next = layer->nextLayer;
				delete layer;
				layer = next;
			}
		}

		//Draw
		layer->draw(window.getAspect());

		layer->lights.bind();
		layer->drawShadowMap();

		layer->lights.sunCSM.bindTexture(3);
		layer->target.bindFinal();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		layer->sky.draw();
		layer->target.draw();

		glViewport(0, 0, window.width, window.height);
		layer->target.display(window.width, window.height);

		window.swap();
	}

	SDL_Quit();
	return 0;
}
