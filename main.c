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

float_t delta_t = 0.0f;
uint32_t current_time = 0.0f;
uint32_t last_time = 0.0f;

float_t player_delta_x = 0.0f, player_delta_y = 0.0f;

float_t movement_speed = 4.0f;

const uint8_t* keyboard;

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

uint32_t convert_to_arbg(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (a << 24) | (r << 16) | (g << 8) | b;
}

void calc_delta_time()
{
	current_time = SDL_GetTicks();
	delta_t = current_time - last_time;
	last_time = current_time;
}

void input()
{
	float_t delta_rot = rot_speed * delta_t;

	if (keyboard[SDLK_d])
	{
		float_t old_dir_x = dir_x;
		dir_x = dir_x * cos(-delta_rot) - dir_y * sin(-delta_rot);
		dir_y = old_dir_x * sin(-delta_rot) + dir_y * cos(-delta_rot);

		float_t old_plane_x = plane_x;
		plane_x = plane_x * cos(-delta_rot) - plane_y * sin(-delta_rot);
		plane_y = old_plane_x * sin(-delta_rot) + plane_y * cos(-delta_rot);
		printf("D");
	}
	if (keyboard[SDLK_a])
	{
		float_t old_dir_x = dir_x;
		dir_x = dir_x * cos(delta_rot) - dir_y * sin(delta_rot);
		dir_y = old_dir_x * sin(delta_rot) + dir_y * cos(delta_rot);

		float_t old_plane_x = plane_x;
		plane_x = plane_x * cos(delta_rot) - plane_y * sin(delta_rot);
		plane_y = old_plane_x * sin(delta_rot) + plane_y * cos(delta_rot);
		printf("A");
	}

	player_delta_x = dir_x * movement_speed * delta_t;
	player_delta_y = dir_y * movement_speed * delta_t;

	if (keyboard[SDLK_w])
	{
		if (map[(int)(player_x + dir_x)][(int)player_y] == 0) { player_x += player_delta_x; }
		if (map[(int)player_x][(int)(player_y + dir_y)] == 0) { player_y += player_delta_y; }
		printf("W");
	}

	if (keyboard[SDLK_s])
	{
		if (map[(int)(player_x - dir_x)][(int)player_y] == 0) { player_x -= player_delta_x; }
		if (map[(int)player_x][(int)(player_y - dir_y)] == 0) { player_y -= player_delta_y; }
		printf("S");
	}
}

void raycast(uint32_t* pixel_buf)
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

		int8_t step_x = 0;
		int8_t step_y = 0;

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

		if (ray_dir_y < 0)
		{
			step_y = -1;
			side_dist_y = (player_y - map_y) * delta_dist_y;
		}
		else
		{
			step_y = 1;
			side_dist_y = (map_y + 1.0f - player_y) * delta_dist_y;
		}

		while (!hit)
		{
			if (side_dist_x < side_dist_y)
			{
				side_dist_x += delta_dist_x;
				map_x += step_x;
				side = false;
			}
			else
			{
				side_dist_y += delta_dist_y;
				map_y += step_y;
				side = true;
			}

			if (map[map_x][map_y] > 0)
			{
				hit = true;
			}
		}

		if (side)
		{
			perp_wall_dist = (side_dist_y - delta_dist_y);
		}
		else
		{
			perp_wall_dist = (side_dist_x - delta_dist_x);
		}

		int32_t line_height = (uint32_t)(SCREEN_HEIGHT / perp_wall_dist);

		int32_t draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;

		if (draw_start < 0)
		{
			draw_start = 0;
		}

		int32_t draw_end = line_height / 2 + SCREEN_HEIGHT / 2;

		if (draw_end >= SCREEN_HEIGHT)
		{
			draw_end = SCREEN_HEIGHT - 1;
		}

		uint32_t color;

		if (side)
		{
			color = convert_to_arbg(0, 255, 0, 255);
		}
		else
		{
			color = convert_to_arbg(0, 100, 0, 255);
		}

		for (uint32_t i = draw_start; i < draw_end; i++)
		{

			pixel_buf[rays + i * SCREEN_WIDTH] = color;
		}
	}
}

void render(SDL_Renderer* renderer, SDL_Texture* buf_tex)
{
	int32_t mem_offset = 0;

	uint32_t* mem_pointer = NULL;

	if (SDL_LockTexture(buf_tex, NULL, (void**)&mem_pointer, &mem_offset)) return;

	mem_offset /= sizeof(uint32_t);

	raycast(mem_pointer);

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
				calc_delta_time(&e);
				SDL_PumpEvents();
				keyboard = SDL_GetKeyboardState(NULL);
				input();
				render(renderer, buf_tex);

				while (SDL_PollEvent(&e)) 
				{ 
					if (e.type == SDL_QUIT) quit = true;
				} 
			}
		}
	}

	return 0;
}