#include <iostream>

extern "C" {
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
}

#include "one_player_game_mode.hxx"
#include "main.hxx"

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

static void vectorSpeedConstant(circle *circ);
static double cart2angle(double x, double y);
static void wallHit (circle *circ, double wallWidth);
//Combined circle logic
static void circleLogicCombined(circle *circ , double wallWidth);

static void angerChaseFear(circle *anger, circle *fear, double angerAccelerationRatio, double fearAccelerationRatio);

//static void circleCollisionDetection(circle *circleOne, circle *circleTwo);

static void playerMakesRandomSeed(circle *player, circle *fear, double randomPlayerSeedStrength);

static void yinAndYangCircleLogic(circle *anger, circle *fear, circle *yinAndYangCircleLogic);

static bool circleCollisionDetection(circle *circleOne, circle *circleTwo, bool collidePhysics, double wallWidth);

static void circleCollisionResponse(circle *circleOne, circle *circleTwo, double wallWidth, double *circleOne_new_velocity_x, double *circleOne_new_velocity_y);


circle::circle(double start_x, double start_y, double radius, double max_speed){
	
	position_x = start_x;
	position_y = start_y;
	velocity_x = 0;
	velocity_y = 0;
	this->radius = radius;
	this->max_speed = max_speed;
}
circle::~circle() {
}
particle_circle::particle_circle()
: circle(0, 0, 0, 0)
{
}
particle_circle::~particle_circle() {
}


textured_circle::textured_circle(SDL_Renderer *ren, const char *texfile, double start_x, double start_y, double radius, double max_speed)
: circle(start_x, start_y, radius, max_speed)
{
	this_that_should_kill_tex = this;
	tex = loadTexture(ren, texfile);
}

void textured_circle::render(SDL_Renderer *ren) {
	SDL_Rect dst;
	dst.x = position_x - radius;
	dst.y = position_y - radius;
	dst.w = radius*2;
	dst.h = radius*2;
	SDL_RenderCopyEx(ren, tex, NULL, &dst, angle, NULL, SDL_FLIP_NONE);
}

textured_circle::~textured_circle() {
	// HACK: avoid double-free (the wrong way) by checking that we were actually the object that called loadTexture() using the stored this token which gets copied with the automatically-implemented shallow copy.
	if (this_that_should_kill_tex == (void*)this)
	/*{std::cerr << "destrolying thing " << ((size_t)tex) << " because I am " << ((size_t)(void*)this) << std::endl;*/
		SDL_DestroyTexture(tex);
		/*} else std::cerr << "not destryojing thing " << ((size_t)tex) << " because I am " << ((size_t)(void*)this) << " instead of " << ((size_t)this_that_should_kill_tex) << std::endl;*/
}

one_player_game_mode::one_player_game_mode(SDL_Renderer *ren)
: defaultPlayer(ren, "playerOne.png", 1920*.5, 1080*.5, 37, 15)   //The radius which is the second to last variable taken should normally be 37
, player(defaultPlayer)
, defaultAnger(ren, "anger.png", 1920*.25, 1080*.25, 37, 15) //20
, anger(defaultAnger)
, defaultFear(ren, "fear.png", 1920*.75, 1080*.75, 37, 17)  //24
, fear(defaultFear)
, yinAndYangCircle(ren, "yinAndYang.png", 1920/2, 1080/2, 1920/2, 0)

{
	wallWidth = 80;
	angerAccelerationRatio = 220; // 200
	fearAccelerationRatio = 190; // 190
	playerSeedStrength = .01;
	playerAcceleration = 1.2;
	score = 0;
	
	tex_map = loadTexture(ren, "Map.png");
	tex_wall = loadTexture(ren, "Wall.png");
	tex_watermelon = loadTexture(ren, "watermelon.png");
	tex_rcircle = loadTexture(ren, "rcircle.png");
	tex_endGame = loadTexture(ren, "endGame.png");
	showScore = false;
	
	//Rcircle start points and velocities
	for (size_t i = 0; i < ARRAY_LENGTH(rcircle); i++){
		rcircle[i].position_x = wallWidth + fmod(i * 1029 + 257 * (i % 7), 1920 - 2*wallWidth);
		rcircle[i].position_y = wallWidth + fmod(i * 703 + 51 * (i % 13), 1080 - 2*wallWidth);
		rcircle[i].velocity_x = 7;
		rcircle[i].velocity_y = -7;
		rcircle[i].radius = 10;
		rcircle[i].enabled = true;
		rcircle[i].max_speed = 10;
	}
	
	left = right = up = down = false;
}

void one_player_game_mode::clear(){
	player = defaultPlayer;
	anger = defaultAnger;
	fear = defaultFear;
	showScore = false;
	score = 0;
	for (size_t i = 0; i < ARRAY_LENGTH(watermelons); i++)
		{
			watermelons[i].enabled = false;
		}
	//Rcircle start points and velocities
	for (size_t i = 0; i < ARRAY_LENGTH(rcircle); i++){
		rcircle[i].position_x = wallWidth + fmod(i * 1029 + 257 * (i % 7), 1920 - 2*wallWidth);
		rcircle[i].position_y = wallWidth + fmod(i * 703 + 51 * (i % 13), 1080 - 2*wallWidth);
		rcircle[i].velocity_x = 7;
		rcircle[i].velocity_y = 7;
		rcircle[i].radius = 10;
		rcircle[i].enabled = true;
	}
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
		case SDLK_RETURN:
			if (showScore)
			{
				*current_game_mode = 0;
				clear();
			}
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
	
	
	//+++++++++++++++++++++++    After player input logic
	
	//Testing if this is good enough for a pool game even though I don't want to make one
	 anger.velocity_x /= 1.025;
	 anger.velocity_y /= 1.025;
	 fear.velocity_x /= 1.025;
	 fear.velocity_y /= 1.025;
	
	//Anger circle chases fear circle and fear circle runs away from anger ball
	angerChaseFear(&anger, &fear, angerAccelerationRatio, fearAccelerationRatio);
	  
	
	
	for (size_t u = 0; u < ARRAY_LENGTH(rcircle); u++){
		angerChaseFear(&rcircle[u], &player, 80, 0);
		for (size_t i = u+1; i < ARRAY_LENGTH(rcircle); i++){
		
			circleCollisionDetection(&rcircle[u], &rcircle[i], true, wallWidth);
		}
	}
		
	
	
	
	
	//Fear and anger makes the WATERMELON!!
	if (circleCollisionDetection(&fear, &anger, true, wallWidth) && !showScore)
	{
		for (size_t i = 0; i < ARRAY_LENGTH(watermelons); i++)
		{
			if (!watermelons[i].enabled)
			{
			watermelons[i].enabled = true;
			watermelons[i].position_x = (anger.position_x + fear.position_x)/2;
			watermelons[i].position_y = (anger.position_y + fear.position_y)/2;
			watermelons[i].radius = 100;
			break;
			}
		}
	}
	for (size_t i = 0; i < ARRAY_LENGTH(watermelons); i++)
		{
			if (watermelons[i].enabled)
			{
			watermelons[i].radius -= .5;
			
			if(circleCollisionDetection(&watermelons[i], &player, false, wallWidth) && !showScore) {
				player.radius += 2;
				for (size_t u = 0; u < ARRAY_LENGTH(rcircle); u++){
					rcircle[u].radius += 1;
				}
				score += watermelons[i].radius;
				watermelons[i].enabled = false;
			}
				if (watermelons[i].radius < 2)
				{
				watermelons[i].enabled = false;
			    }
			}
		}
	//Player and anger
	if (circleCollisionDetection(&player, &anger, true, wallWidth))
	{
		showScore = true;
	}
	//Player and fear
	if (circleCollisionDetection(&player, &fear, true, wallWidth))
	{
		showScore = true;
	}
	if (player.radius < 32)
		showScore = true;

	//Player random seed
	playerMakesRandomSeed(&player, &fear, playerSeedStrength);
	
	
	//++++++++++++++ Final circle logic
	
	//Final player
	
	circleLogicCombined(&player, wallWidth);
	//Final anger
	circleLogicCombined(&anger, wallWidth);
	//Final fear
	circleLogicCombined(&fear, wallWidth);
	//Final rcircle
	for (size_t i = 0; i < ARRAY_LENGTH(rcircle); i++){
		circleLogicCombined(&rcircle[i], wallWidth);
	}
	
	//Yin and yang circle logic
	yinAndYangCircleLogic(&anger, &fear, &yinAndYangCircle);
	
	
	
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDERER I SAID RENDERER RENDERER I WISHED I COULD DOUBLE CAPS LOCK BUT THAT IS IMPOSSIBLE 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void one_player_game_mode::render(SDL_Renderer *ren, TTF_Font *font){
	
	SDL_RenderCopy(ren, tex_map, NULL, NULL);
	
	SDL_Rect dst;
	
	
	
	
	//Background balls being rendered
	for (size_t i = 0; i < ARRAY_LENGTH(rcircle); i++){
			dst.x = rcircle[i].position_x - rcircle[i].radius;
			dst.y = rcircle[i].position_y - rcircle[i].radius;
			dst.w = dst.h = rcircle[i].radius * 2;
			SDL_RenderCopyEx(ren, tex_rcircle, NULL, &dst, rcircle[i].angle, NULL, SDL_FLIP_NONE);
		
	}
	
	// Watermelon particles.... yeah Watermelons because I can do whatever I want
	for (size_t i = 0; i < ARRAY_LENGTH(watermelons); i++)
		if (watermelons[i].enabled)
		{
			dst.x = watermelons[i].position_x - watermelons[i].radius;
			dst.y = watermelons[i].position_y - watermelons[i].radius;
			dst.w = dst.h = watermelons[i].radius * 2;
			SDL_RenderCopyEx(ren, tex_watermelon, NULL, &dst, watermelons[i].angle, NULL, SDL_FLIP_NONE);
		}
	
	
	
	
	//Yin and yang circle RENDER
	yinAndYangCircle.render(ren);

	// Player one RENDER
	player.render(ren);
	
	// Anger ball renderer RENDER
	anger.render(ren);
	
	
	//Fear RENDER
	fear.render(ren);
	
	SDL_RenderCopy(ren, tex_wall, NULL, NULL);
	
	
	//Score being displayed constantly
	SDL_Color white = {255, 255, 255};
	SDL_Surface *score_surface = TTF_RenderText_Solid(font, ("Score: " + str((int)score)).c_str(), white);
	SDL_Texture *tex_score = SDL_CreateTextureFromSurface(ren, score_surface);
	SDL_FreeSurface(score_surface);
	SDL_QueryTexture(tex_score, NULL, NULL, &dst.w, &dst.h);
	dst.y = 30;
	dst.x = (1920 - dst.w)/2;
	SDL_RenderCopy(ren, tex_score, NULL, &dst);
	SDL_DestroyTexture(tex_score);
	//Ending score
	if (showScore) 
	{
		SDL_Color black = {0, 0, 0};
		SDL_Surface *score_surface = TTF_RenderText_Solid(font, ("Hah, you lose " + str((int)score)).c_str(), black);
		SDL_Texture *tex_score = SDL_CreateTextureFromSurface(ren, score_surface);
		SDL_FreeSurface(score_surface);
		SDL_QueryTexture(tex_score, NULL, NULL, &dst.w, &dst.h);
		dst.y = (1080 - dst.h)/2;
		dst.x = (1920 - dst.w)/2;
		SDL_RenderCopy(ren, tex_score, NULL, &dst);
		SDL_DestroyTexture(tex_score);
		
		SDL_RenderCopy(ren, tex_endGame, NULL, NULL);
	}
}

one_player_game_mode::~one_player_game_mode(){
	SDL_DestroyTexture(tex_map);
	SDL_DestroyTexture(tex_wall);
	SDL_DestroyTexture(tex_watermelon);
	SDL_DestroyTexture(tex_rcircle);
	SDL_DestroyTexture(tex_endGame);
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



//Maybe later on make this take up less room now that I know how to do it - Oh I did it alrgiht
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


static bool circleCollisionDetection(circle *circleOne, circle *circleTwo, bool collidePhysics, double wallWidth){
	
	double x_distance = circleTwo->position_x - circleOne->position_x;
	double y_distance = circleTwo->position_y - circleOne->position_y;
	double firstToSecondDistance = sqrt(pow(x_distance, 2) + pow(y_distance, 2));
	double radiusCombined = circleOne->radius + circleTwo->radius;
	//Circle physics collison response in here using http://en.wikipedia.org/wiki/Elastic_collision at 2 and three-dimensional
	if(firstToSecondDistance <= radiusCombined){
		
		if (collidePhysics)
		{
                        double circleOne_new_velocity_x;
                        double circleOne_new_velocity_y;
                        circleCollisionResponse(circleOne, circleTwo, wallWidth, &circleOne_new_velocity_x, &circleOne_new_velocity_y);
                        // Don’t need to preserve circleTwo’s
                        // velocity, can just write it directly.
                        circleCollisionResponse(circleTwo, circleOne, wallWidth, &circleTwo->velocity_x, &circleTwo->velocity_y);
                        circleOne->velocity_x = circleOne_new_velocity_x;
                        circleOne->velocity_y = circleOne_new_velocity_y;
		}
		return true;
	}
	return false;
}

static void circleCollisionResponse(circle *circleOne, circle *circleTwo, double wallWidth, double *circleOne_new_velocity_x, double *circleOne_new_velocity_y) {
	
	//Setting up variables to put into the weird equation found from wikipedia at http://en.wikipedia.org/wiki/Elastic_collision at 2 and three-dimensional
	double v1 = circleOne->vectorSpeed();
	double v2 = circleTwo->vectorSpeed();
	double o1 = circleOne->vectorAngle();
	double o2 = circleTwo->vectorAngle();
	double m1 = pow(circleOne->radius, 2) *M_PI;
	double m2 = pow(circleTwo->radius, 2) *M_PI;
	double contactAngle = cart2angle((circleTwo->position_x - circleOne->position_x) , (circleTwo->position_y - circleOne->position_y));
	
	double topPart = v1 * cos((o1 - contactAngle) /180*M_PI) * (m1-m2) + 2*m2*v2 * cos((o2 - contactAngle)/180*M_PI);
	double bottomPart = m1 + m2;
	double sidePartForX =   v1 * sin((o1 - contactAngle)/180*M_PI) * cos((contactAngle)/180*M_PI + M_PI / 2);
	double sidePartForY =   v1 * sin((o1 - contactAngle)/180*M_PI) * sin((contactAngle)/180*M_PI  + M_PI / 2);

	// Put in velocities
	*circleOne_new_velocity_x = (topPart/bottomPart * cos(contactAngle/180*M_PI) + sidePartForX);
	*circleOne_new_velocity_y = (topPart/bottomPart * sin(contactAngle/180*M_PI) + sidePartForY);

	//Make it so that it cannot get stuck in the wall with this change in placement
	wallHit(circleOne , wallWidth);
	
	//Move away if overlapping 
	circleTwo->position_x = circleOne->position_x + cos(contactAngle/180*M_PI) * (circleTwo->radius + circleOne->radius );
	circleTwo->position_y = circleOne->position_y + sin(contactAngle/180*M_PI) * (circleTwo->radius + circleOne->radius );
}

// Yin and yang circle in the background to follow the yin and yang circles
static void yinAndYangCircleLogic(circle *anger, circle *fear, circle *yinAndYangCircle){
	
	//Find the value in the middle of yin and yang
	yinAndYangCircle->position_x = (anger->position_x + fear->position_x)/2;
	yinAndYangCircle->position_y = (anger->position_y + fear->position_y)/2;
	double x_distance =  anger->position_x - fear->position_x;
	double y_distance =  anger->position_y - fear->position_y;
	yinAndYangCircle->radius = sqrt(pow(x_distance, 2) + pow(y_distance,2));
	yinAndYangCircle->angle = cart2angle(x_distance, y_distance);
}






