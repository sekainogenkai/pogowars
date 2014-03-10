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
static void wallHit (double *position_x, double *position_y, double *velocity_x, double *velocity_y, double radius, double wallWidth);
//Combined circle logic
static void circleLogicCombined(double *position_x, double *position_y, double *velocity_x
, double *velocity_y, double radius, double wallWidth, double *angle, double max_speed);

static void angerChaseFear(double anger_x, double anger_y, double fear_x, double fear_y, double *anger_velocity_x, double *anger_velocity_y, 
double *fear_velocity_x, double *fear_velocity_y, double angerAccelerationRatio, double fearAccelerationRatio);

circle::circle(SDL_Renderer *ren, const char *texfile, double start_x, double start_y, double radius, double max_speed){
	
	position_x = start_x;
	position_y = start_y;
	velocity_x = 0;
	velocity_y = 0;
	this->radius = radius;
	this->max_speed = max_speed;

	tex = loadTexture(ren, texfile);
}
circle::~circle(){
	SDL_DestroyTexture(tex);
}

one_player_game_mode::one_player_game_mode(SDL_Renderer *ren)
: player(ren, "playerOne.png", 1920*.5, 1080*.5, 37, 10)
, anger(ren, "anger.png", 1920*.25, 1080*.25, 37, 10)
, fear(ren, "fear.png", 1920*.75, 1080*.75, 37, 10)
{
	wallWidth = 80;
	angerMaxSpeed = 10;
	fearMaxSpeed = 8;
	angerAccelerationRatio = 6;
	fearAccelerationRatio = 6;
	
	tex_map = loadTexture(ren, "Map.png");
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
		player.velocity_y -= .5;
	}
	if (down && !up){
		player.velocity_y += .5;
	}
	if (!up && !down){
		player.velocity_y /= 1.025;
		if (fabs(player.velocity_y) < .05)
			player.velocity_y = 0;
	}
	if (left && !right){
		player.velocity_x -= .5;
	}
	if (right && !left){
		player.velocity_x += .5;
	}
	if (!left && !right){
		player.velocity_x /= 1.025;
		if (fabs(player.velocity_x) < .05)
			player.velocity_x = 0;
	}
	
	//After player input logic
	  
	
	//Anger circle chases fear circle and fear circle runs away from anger ball
	angerChaseFear(anger.position_x, anger.position_y, fear.position_x,fear.position_y, &anger.velocity_x, &anger.velocity_y, 
	&fear.velocity_x, &fear.velocity_y, angerAccelerationRatio, fearAccelerationRatio);
	  
	  
	//Final circle logic
	//Final player
	circleLogicCombined(&player.position_x, &player.position_y, &player.velocity_x, &player.velocity_y, player.radius, wallWidth, &player.angle, player.max_speed);
	//Final anger
	circleLogicCombined(&anger.position_x, &anger.position_y, &anger.velocity_x, &anger.velocity_y, anger.radius, wallWidth, &anger.angle, anger.max_speed);
	//Final fear
	circleLogicCombined(&fear.position_x, &fear.position_y, &fear.velocity_x, &fear.velocity_y, fear.radius, wallWidth, &fear.angle, fear.max_speed);
	
}

void one_player_game_mode::render(SDL_Renderer *ren, TTF_Font *font){
	
	SDL_RenderCopy(ren, tex_map, NULL, NULL);
	
	// Player one render
	SDL_Rect dst;
	dst.x = player.position_x - player.radius;
	dst.y = player.position_y - player.radius;
	SDL_QueryTexture(player.tex, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopyEx(ren, player.tex, NULL, &dst, player.angle, NULL, SDL_FLIP_NONE);
	
	// Anger ball renderer
	dst.x = anger.position_x - anger.radius;
	dst.y = anger.position_y - anger.radius;
	SDL_QueryTexture(anger.tex, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopyEx(ren, anger.tex, NULL, &dst, anger.angle, NULL, SDL_FLIP_NONE);
	
	
	//Fear
	dst.x = fear.position_x - fear.radius;
	dst.y = fear.position_y - fear.radius;
	SDL_QueryTexture(fear.tex, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopyEx(ren, fear.tex, NULL, &dst, fear.angle, NULL, SDL_FLIP_NONE);
	
}

one_player_game_mode::~one_player_game_mode(){
	SDL_DestroyTexture(tex_map);
}

static void vectorSpeedConstant(double *velocity_x, double *velocity_y, double max_vector_speed)
{
	
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

static void wallHit (double *position_x, double *position_y, double *velocity_x, double *velocity_y, double radius, double wallWidth)
{
	//left wall hit
	if(*position_x - radius < wallWidth){
		*position_x = wallWidth + radius;
		*velocity_x = fabs(*velocity_x);
	}
	//right wall hit
	if(*position_x + radius > 1920 - wallWidth){
		*position_x = 1920 - (wallWidth + radius);
		*velocity_x = -fabs(*velocity_x);
	}
	//top wall hit
	if(*position_y - radius < wallWidth){
		*position_y = wallWidth + radius;
		*velocity_y = fabs(*velocity_y);
	}
	//bottom wall hit
	if(*position_y + radius > 1080 - wallWidth){
		*velocity_y = - fabs(*velocity_y);
		*position_y = 1080 - (wallWidth + radius);
	}
}

//Combine all the simple circle logic into this
static void circleLogicCombined(double *position_x, double *position_y, double *velocity_x
, double *velocity_y, double radius, double wallWidth, double *angle, double max_speed){
	
	vectorSpeedConstant(&*velocity_x, &*velocity_y, max_speed);
	if (fabs(*velocity_x) > 0.25 || fabs(*velocity_y) > 0.25)
		*angle = cart2angle(*velocity_x, *velocity_y);
	wallHit(&*position_x, &*position_y, &*velocity_x, &*velocity_y, radius, wallWidth);
	
	*position_x += *velocity_x;
	*position_y += *velocity_y;
}


static void angerChaseFear(double anger_x, double anger_y, double fear_x, double fear_y, double *anger_velocity_x, double *anger_velocity_y, 
double *fear_velocity_x, double *fear_velocity_y, double angerAccelerationRatio, double fearAccelerationRatio){
	
	double x_distance = fear_x - anger_x;
	double y_distance = fear_y - anger_y;
	
	double angerToFearAngle = cart2angle(x_distance, y_distance);
	double angerToFearDistance = sqrt(pow(x_distance, 2) + pow(y_distance,2));
	
	double angerAcceleration = 1/angerToFearDistance * angerAccelerationRatio;
	double fearAcceleration = 1/angerToFearDistance * fearAccelerationRatio;
	
	double angerAcceleration_x = cos(angerToFearAngle/180*M_PI) * angerAcceleration;
	double angerAcceleration_y = sin(angerToFearAngle/180*M_PI) * angerAcceleration;
	double fearAcceleration_x = cos(angerToFearAngle/180*M_PI) * fearAcceleration;
	double fearAcceleration_y = sin(angerToFearAngle/180*M_PI) * fearAcceleration;
	
	*anger_velocity_x += angerAcceleration_x;
	*anger_velocity_y += angerAcceleration_y;
	*fear_velocity_x += fearAcceleration_x;
	*fear_velocity_y += fearAcceleration_y;
	
}

