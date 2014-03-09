#include <iostream>

extern "C" {
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
}

#include "one_player_game_mode.hxx"
#include "main.hxx"

static void vectorSpeedConstant(double *velocity_x, double *velocity_y, double max_vector_speed);
static double cart2angle(double x, double y);

one_player_game_mode::one_player_game_mode(SDL_Renderer *ren){
	position_x = 1;
	position_y = 1;
	velocity_x = 0;
	velocity_y = 0;
	max_speed = 9;
	angle = 0;
	tex_map = loadTexture(ren, "Map.png");
	tex_playerOne = loadTexture(ren, "playerOne.png");
	left = right = up = down = false;
}

bool one_player_game_mode::processEvents(SDL_Event *event, int *current_game_mode){
	switch (event->type)
	{
	case SDL_KEYDOWN:
		if (event->key.repeat)
			break;
		switch (event->key.keysym.sym)
		{
		case SDLK_LEFT:
			left = true;
			break;
		case SDLK_RIGHT:
			right = true;
			break;
		case SDLK_UP:
			up = true;
			break;
		case SDLK_DOWN:
			down = true;
			break;
		}
		break;
		
		case SDL_KEYUP:
		if (event->key.repeat)
			break;
		switch (event->key.keysym.sym)
		{
		case SDLK_LEFT:
			left = false;
			break;
		case SDLK_RIGHT:
			right = false;
			break;
		case SDLK_UP:
			up = false;
			break;
		case SDLK_DOWN:
			down = false;
			break;
		}
		break;
	}
	return false;
}

void one_player_game_mode::animate(){
	if (up && !down){
		velocity_y -= .5;
	}
	if (down && !up){
		velocity_y += .5;
	}
	if (!up && !down){
		velocity_y /= 1.025;
		if (fabs(velocity_y) < .05)
			velocity_y = 0;
	}
	if (left && !right){
		velocity_x -= .5;
	}
	if (right && !left){
		velocity_x += .5;
	}
	if (!left && !right){
		velocity_x /= 1.025;
		if (fabs(velocity_x) < .05)
			velocity_x = 0;
	}
	
	vectorSpeedConstant(&velocity_x, &velocity_y, max_speed);
	if (fabs(velocity_x) > 0.25 || fabs(velocity_y) > 0.25)
		angle = cart2angle(velocity_x, velocity_y);
	position_x += velocity_x;
	position_y += velocity_y;
}

void one_player_game_mode::render(SDL_Renderer *ren, TTF_Font *font){
	
	SDL_RenderCopy(ren, tex_map, NULL, NULL);
	// Load one_player select bar and specify its position
	SDL_Rect dst;
	dst.x = position_x;
	dst.y = position_y;
	SDL_QueryTexture(tex_playerOne, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopyEx(ren, tex_playerOne, NULL, &dst, angle, NULL, SDL_FLIP_NONE);
}

one_player_game_mode::~one_player_game_mode(){
	SDL_DestroyTexture(tex_map);
	SDL_DestroyTexture(tex_playerOne);
}

static void vectorSpeedConstant(double *velocity_x, double *velocity_y, double max_vector_speed){
	
	double ratio = max_vector_speed/sqrt((*velocity_x)*(*velocity_x) + (*velocity_y) * (*velocity_y));
	if(ratio < 1){
		*velocity_x *= ratio;
		*velocity_y *= ratio;
	}
}

static double cart2angle(double x, double y)
{
	if (fabs(x) < 0.0001)
		return y > 0 ? 90 : 270;
	else
		return fmod(180 * atanf(y/x) / M_PI + (x > 0 ? 0 : 180) + 360, 360);
}

	

