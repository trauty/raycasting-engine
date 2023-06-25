#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"

#define MAP_WIDTH 16
#define MAP_HEIGHT 16

const uint16_t SCREEN_WIDTH = 1280;
const uint16_t SCREEN_HEIGHT = 720;

float_t player_x = 2.0f, player_y = 3.0f;
float_t dir_x = -1.0f, dir_y = 0.0f;
float_t plane_x = 0.0f, plane_y = 0.66f;
float_t rot_speed = 4.0f;

float_t player_delta_x = 0.0f, player_delta_y = 0.0f;

float_t movement_speed = 4.0f;

uint8_t map[MAP_WIDTH][MAP_HEIGHT] =
{
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1},
	{1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
	{1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

void raycast()
{
	for (uint32_t rays = 0; rays < SCREEN_WIDTH; rays++)
	{
		float_t camera_x = 2.0f * rays / (float_t)SCREEN_WIDTH - 1.0f;
		float_t ray_dir_x = dir_x + plane_x * camera_x;
		float_t ray_dir_y = dir_y + plane_y * camera_x;

		uint32_t map_x = (int)player_x;
		uint32_t map_y = (int)player_y;

		float_t side_dist_x = 0.0f;
		float_t side_dist_y = 0.0f;

		float_t delta_dist_x = (ray_dir_x == 0) ? 1e30 : fabs(1.0f / ray_dir_x);
		float_t delta_dist_y = (ray_dir_y == 0) ? 1e30 : fabs(1.0f / ray_dir_y);
		float_t perp_wall_dist = 0.0f;

		uint8_t step_x = 0;
		uint8_t step_y = 0;

		bool hit = false;
		bool side = false;

		if (ray_dir_x < 0)
		{
			step_x = -1;
			side_dist_x = (player_x - map_x) * delta_dist_x;
		}
		else
		{
			step_x = 1;
			side_dist_x = (map_x + 1.0f - player_x) * delta_dist_x;
		}

		while (!hit)
		{
			if (side_dist_x < side_dist_y)
			{
				side_dist_x += delta_dist_x;
			}
		}
	}
}

uint32_t convert_to_arbg(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (a << 24) | (r << 16) | (g << 8) | b;
}

void render(SDL_Renderer* renderer, SDL_Texture* buf_tex)
{
	int32_t mem_offset = 0;

	uint32_t* mem_pointer = NULL;

	if (SDL_LockTexture(buf_tex, NULL, (void**)&mem_pointer, &mem_offset)) return;

	mem_offset /= sizeof(uint32_t);

	//raycasting

	SDL_UnlockTexture(buf_tex);

	SDL_RenderCopy(renderer, buf_tex, NULL, NULL);

	SDL_RenderPresent(renderer);
}

int main(int argc, char** argv)
{
	SDL_Window* window = NULL;

	SDL_Renderer* renderer = NULL;

	SDL_Texture* buf_tex = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Error: %s\n", SDL_GetError());
	}
	else
	{
		window = SDL_CreateWindow("raycasting-engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

		buf_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

		if (window == NULL || renderer == NULL || buf_tex == NULL)
		{
			printf("Error: %s\n", SDL_GetError());
		}
		else
		{
			SDL_Event e; 
			bool quit = false; 
			while (quit == false)
			{ 
				while (SDL_PollEvent(&e)) 
				{ 
					render(renderer, buf_tex);
					if (e.type == SDL_QUIT) quit = true;
				} 
			}
		}
	}

	return 0;
}