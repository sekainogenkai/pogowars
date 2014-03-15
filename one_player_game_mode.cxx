#include <iostream>

extern "C" {
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
}

#include "one_player_game_mode.hxx"
#include "main.hxx"

static void vectorSpeedConstant(circle *circ);
static double cart2angle(double x, double y);
static void wallHit (circle *circ, double wallWidth);
//Combined circle logic
static void circleLogicCombined(circle *circ , double wallWidth);

static void angerChaseFear(circle *anger, circle *fear, double angerAccelerationRatio, double fearAccelerationRatio);

//static void circleCollisionDetection(circle *circleOne, circle *circleTwo);

static void playerMakesRandomSeed(circle *player, circle *fear, double randomPlayerSeedStrength);

static void yinAndYangCircleLogic(circle *anger, circle *fear, circle *yinAndYangCircleLogic);

static void circleCollisionDetection(circle *circleOne, circle *circleTwo, bool collidePhysics);

static void circleCollisionResponse(circle *circleOne, circle *circleTwo);

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
: player(ren, "playerOne.png", 1920*.5, 1080*.5, 100, 15)   //The radius which is the second to last variable taken should normally be 37
, anger(ren, "anger.png", 1920*.25, 1080*.25, 100, 20)
, fear(ren, "fear.png", 1920*.75, 1080*.75, 100, 24)
, yinAndYangCircle(ren, "yinAndYang.png", 1920/2, 1080/2, 1920/2, 0)

{
	wallWidth = 80;
	angerAccelerationRatio = 170;
	fearAccelerationRatio = 190;
	playerSeedStrength = .01;
	playerAcceleration = 1;
	
	
	tex_map = loadTexture(ren, "Map.png");
	tex_wall = loadTexture(ren, "Wall.png");
	

	
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
		player.velocity_y -= playerAcceleration;
	}
	if (down && !up){
		player.velocity_y += playerAcceleration;
	}
	if (!up && !down){
		player.velocity_y /= 1.025;
		if (fabs(player.velocity_y) < .05)
			player.velocity_y = 0;
	}
	if (left && !right){
		player.velocity_x -= playerAcceleration;
	}
	if (right && !left){
		player.velocity_x += playerAcceleration;
	}
	if (!left && !right){
		player.velocity_x /= 1.025;
		if (fabs(player.velocity_x) < .05)
			player.velocity_x = 0;
	}
	
	//+++++++++++++++++++++++After player input logic
	  
	
	//Anger circle chases fear circle and fear circle runs away from anger ball
	angerChaseFear(&anger, &fear, angerAccelerationRatio, fearAccelerationRatio);
	  
	//Collision detection
	circleCollisionDetection(&fear, &anger, true);
	circleCollisionDetection(&player, &anger, true);
	circleCollisionDetection(&player, &fear, true);
	
	
	//Player random seed
	playerMakesRandomSeed(&player, &fear, playerSeedStrength);
	
	//Final circle logic
	//Final player
	circleLogicCombined(&player, wallWidth);
	//Final anger
	circleLogicCombined(&anger, wallWidth);
	//Final fear
	circleLogicCombined(&fear, wallWidth);
	
	//Yin and yang circle logic
	yinAndYangCircleLogic(&anger, &fear, &yinAndYangCircle);
	
}

void one_player_game_mode::render(SDL_Renderer *ren, TTF_Font *font){
	
	SDL_RenderCopy(ren, tex_map, NULL, NULL);
	
	SDL_Rect dst;
	
	//SDL_Rect src; I will use this when I want to start to animate things
	
	//Yin and yang circle RENDER
	dst.x = yinAndYangCircle.position_x - yinAndYangCircle.radius/2;
	dst.y = yinAndYangCircle.position_y - yinAndYangCircle.radius/2;
	SDL_QueryTexture(yinAndYangCircle.tex, NULL, NULL, &dst.w, &dst.h);
	
	dst.w = dst.h = yinAndYangCircle.radius;
	SDL_RenderCopyEx(ren, yinAndYangCircle.tex, NULL, &dst, yinAndYangCircle.angle, NULL, SDL_FLIP_NONE);
	
	// Player one RENDER
	
	dst.x = player.position_x - player.radius;
	dst.y = player.position_y - player.radius;
	
	SDL_QueryTexture(player.tex, NULL, NULL, &dst.w, &dst.h);
	
	dst.w = dst.h = 2*player.radius;
	SDL_RenderCopyEx(ren, player.tex, NULL, &dst, player.angle, NULL, SDL_FLIP_NONE);
	
	// Anger ball renderer RENDER
	dst.x = anger.position_x - anger.radius;
	dst.y = anger.position_y - anger.radius;
	SDL_QueryTexture(anger.tex, NULL, NULL, &dst.w, &dst.h);
	
	dst.w = dst.h = 2*anger.radius;
	SDL_RenderCopyEx(ren, anger.tex, NULL, &dst, anger.angle, NULL, SDL_FLIP_NONE);
	
	
	//Fear RENDER
	dst.x = fear.position_x - fear.radius;
	dst.y = fear.position_y - fear.radius;
	SDL_QueryTexture(fear.tex, NULL, NULL, &dst.w, &dst.h);
	
	dst.w = dst.h = 2*fear.radius;
	SDL_RenderCopyEx(ren, fear.tex, NULL, &dst, fear.angle, NULL, SDL_FLIP_NONE);
	
	SDL_RenderCopy(ren, tex_wall, NULL, NULL);
}

one_player_game_mode::~one_player_game_mode(){
	SDL_DestroyTexture(tex_map);
	SDL_DestroyTexture(tex_wall);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Functions
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCTIONS
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ I SAID FUNCTIONS YOU  YOU YOU


static void vectorSpeedConstant(circle *circ)
{
	
	double ratio = circ->max_speed/circ->vectorSpeed();
	if(ratio < 1){
		circ->velocity_x *= ratio;
		circ->velocity_y *= ratio;
	}
}

static double cart2angle(double x, double y)
{
	if (fabs(x) < 0.0001)
		return y > 0 ? 90 : 270;
	else
		return fmod(180 * atanf(y/x) / M_PI + (x > 0 ? 0 : 180) + 360, 360);
}

static void wallHit (circle *circ, double wallWidth)
{
	//left wall hit
	if(circ->position_x - circ->radius < wallWidth){
		circ->position_x = wallWidth + circ->radius;
		circ->velocity_x = fabs(circ->velocity_x);
	}
	//right wall hit
	if(circ->position_x + circ->radius > 1920 - wallWidth){
		circ->position_x = 1920 - (wallWidth + circ->radius);
		circ->velocity_x = -fabs(circ->velocity_x);
	}
	//top wall hit
	if(circ->position_y - circ->radius < wallWidth){
		circ->position_y = wallWidth + circ->radius;
		circ->velocity_y = fabs(circ->velocity_y);
	}
	//bottom wall hit
	if(circ->position_y + circ->radius > 1080 - wallWidth){
		circ->velocity_y = - fabs(circ->velocity_y);
		circ->position_y = 1080 - (wallWidth + circ->radius);
	}
}

//Combine all the simple circle logic into this
static void circleLogicCombined(circle *circ, double wallWidth){
	
	vectorSpeedConstant(circ);
	if (fabs(circ->velocity_x) > 0.25 || fabs(circ->velocity_y) > 0.25)
		circ->angle = cart2angle(circ->velocity_x, circ->velocity_y);
	wallHit(circ , wallWidth);
	
	circ->position_x += circ->velocity_x;
	circ->position_y += circ->velocity_y;
}



//Maybe later on make this take up less room now that I know how to do it
static void angerChaseFear(circle *anger, circle *fear, double angerAccelerationRatio, double fearAccelerationRatio){
	
	//Pretty simple just read the variables to know what is happening here
	double x_distance = fear->position_x - anger->position_x;
	double y_distance = fear->position_y - anger->position_y;
	
	double angerToFearAngle = cart2angle(x_distance, y_distance);
	double angerToFearDistance = sqrt(pow(x_distance, 2) + pow(y_distance,2));
	
	double angerAcceleration = 1/angerToFearDistance * angerAccelerationRatio;
	double fearAcceleration = 1/angerToFearDistance * fearAccelerationRatio;
	
	double angerAcceleration_x = cos(angerToFearAngle/180*M_PI) * angerAcceleration;
	double angerAcceleration_y = sin(angerToFearAngle/180*M_PI) * angerAcceleration;
	double fearAcceleration_x = cos(angerToFearAngle/180*M_PI) * fearAcceleration;
	double fearAcceleration_y = sin(angerToFearAngle/180*M_PI) * fearAcceleration;
	
	anger->velocity_x += angerAcceleration_x;
	anger->velocity_y += angerAcceleration_y;
	fear->velocity_x += fearAcceleration_x;
	fear->velocity_y += fearAcceleration_y;
	
}

static void playerMakesRandomSeed(circle *player, circle *fear, double randomPlayerSeedStrength){
	
	double x_distance =  player->position_x - fear->position_x;
	double y_distance =  player->position_y - fear->position_y;
	double angle = cart2angle(x_distance, y_distance);
	double fearAcceleration_x = cos(angle/180*M_PI) * randomPlayerSeedStrength;
	double fearAcceleration_y = sin(angle/180*M_PI) * randomPlayerSeedStrength;
	fear->velocity_x += fearAcceleration_x;
	fear->velocity_y += fearAcceleration_y;
}


static void circleCollisionDetection(circle *circleOne, circle *circleTwo, bool collidePhysics){
	
	double x_distance = circleTwo->position_x - circleOne->position_x;
	double y_distance = circleTwo->position_y - circleOne->position_y;
	double firstToSecondDistance = sqrt(pow(x_distance, 2) + pow(y_distance, 2));
	double radiusCombined = circleOne->radius + circleTwo->radius;
	//Circle physics collison response in here using http://en.wikipedia.org/wiki/Elastic_collision at 2 and three-dimensional
	if(firstToSecondDistance <= radiusCombined && collidePhysics){
		circleCollisionResponse(circleOne, circleTwo);
		circleCollisionResponse(circleTwo, circleOne);
	}
}

static void circleCollisionResponse(circle *circleOne, circle *circleTwo){
	
	//Setting up variables to put into the weird equation found from wikipedia at http://en.wikipedia.org/wiki/Elastic_collision at 2 and three-dimensional
	double v1 = circleOne->vectorSpeed();
	double v2 = circleTwo->vectorSpeed();
	double o1 = circleOne->vectorAngle();
	double o2 = circleTwo->vectorAngle();
	double m1 = pow(circleOne->radius, 2) *M_PI;
	double m2 = pow(circleTwo->radius, 2) *M_PI;
	double middleAngles = cart2angle((circleTwo->position_x - circleOne->position_x) , (circleTwo->position_y - circleOne->position_y));
	
	double topPart = v1 * cos(o1/180*M_PI) * (m1-m2) + 2*m2*v2 * cos((o2 - middleAngles)/180*M_PI);
	double bottomPart = m1 + m2;
	double sidePartForX = cos(middleAngles/180*M_PI) + v1 * sin((o1-middleAngles)/180*M_PI) * cos((middleAngles + M_PI / 2)/180*M_PI);
	double sidePartForY = sin(middleAngles/180*M_PI) + v1 * sin((o1 - middleAngles)/180*M_PI) * sin((middleAngles + M_PI / 2)/180*M_PI);
	
	circleOne->velocity_x = topPart/bottomPart * sidePartForX;
	circleTwo->velocity_y = topPart/bottomPart * sidePartForY;
	
}


// Yin and yang circle in the background to follow the yin and yang circles
static void yinAndYangCircleLogic(circle *anger, circle *fear, circle *yinAndYangCircle){
	
	//Find the value in the middle of yin and yang
	yinAndYangCircle->position_x = (anger->position_x + fear->position_x)/2;
	yinAndYangCircle->position_y = (anger->position_y + fear->position_y)/2;
	double x_distance =  anger->position_x - fear->position_x;
	double y_distance =  anger->position_y - fear->position_y;
	yinAndYangCircle->radius = ((sqrt(pow(x_distance, 2) + pow(y_distance,2)))/2) * 3.25;
	yinAndYangCircle->angle = cart2angle(x_distance, y_distance);
	
}





