#include <stdio.h>
#include <stddef.h>

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char** argv)
{
	SDL_Window* window = NULL;

	SDL_Surface* screen_surface = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Error: %s\n", SDL_GetError());
	}
	else
	{
		window = SDL_CreateWindow("raycasting-engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);

			//Fill the surface white
			SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, 0xFF, 0xFF, 0xFF));

			//Update the surface
			SDL_UpdateWindowSurface(window);

			//Hack to get window to stay up
			SDL_Event e; int quit = 0; while (quit == 0) { while (SDL_PollEvent(&e)) { if (e.type == SDL_QUIT) quit = 1; } }
		}
	}

	return 0;
}