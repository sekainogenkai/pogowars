#include <iostream>

extern "C" {
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
}

#include "camera.hxx"
#include "one_player_game_mode.hxx"
#include "main.hxx"

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

static void vectorSpeedConstant(circle *circ);
static double cart2angle(double x, double y);
static void wallHit (circle *circ);
//Combined circle logic
static void circleLogicCombined(circle *circ );


//static void circleCollisionDetection(circle *circleOne, circle *circleTwo);



static bool circleCollisionDetection(circle *circleOne, circle *circleTwo, bool collidePhysics);

static void circleCollisionResponse(circle *circleOne, circle *circleTwo, double *circleOne_new_velocity_x, double *circleOne_new_velocity_y);


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

void textured_circle::render(SDL_Renderer *ren, camera *my_camera) {
	SDL_Rect dst;
	dst.x = position_x - radius;
	dst.y = position_y - radius;
	dst.w = radius*2;
	dst.h = radius*2;
	my_camera->transform(&dst);
	SDL_RenderCopyEx(ren, tex, NULL, &dst, /*/angle/*/ 0, NULL, SDL_FLIP_NONE);
}

textured_circle::~textured_circle() {
	// HACK: avoid double-free (the wrong way) by checking that we were actually the object that called loadTexture() using the stored this token which gets copied with the automatically-implemented shallow copy.
	if (this_that_should_kill_tex == (void*)this)
	/*{std::cerr << "destrolying thing " << ((size_t)tex) << " because I am " << ((size_t)(void*)this) << std::endl;*/
		SDL_DestroyTexture(tex);
		/*} else std::cerr << "not destryojing thing " << ((size_t)tex) << " because I am " << ((size_t)(void*)this) << " instead of " << ((size_t)this_that_should_kill_tex) << std::endl;*/
}

one_player_game_mode::one_player_game_mode(SDL_Renderer *ren)
: my_camera(1920, 1080, 1920, 1080)
, twoPlayerMode(true)
, defaultPlayer(ren, "blueBody.png", !twoPlayerMode ? 1920*.5 : 1920*.9, !twoPlayerMode ? 1080*.5 : 1080 *.1, 160, 15)   //The radius which is the second to last variable taken should normally be 37
, player(defaultPlayer)
, defaultPlayerTwo(ren, "redBody.png", 1920*.1, 1080*.9, 160, 15)   //The radius which is the second to last variable taken should normally be 37
, playerTwo(defaultPlayerTwo)
{
	
	this->twoPlayerMode = twoPlayerMode;
	
	playerAcceleration = 1.2;
	score = 0;
	lowerRadius = 5;
	radiusDifferentMax = 15 * 2;
	
	
	tex_map = loadTexture(ren, "smokeRoom.png");
	tex_foreground = loadTexture(ren, "smoke_Room_Foreground.png");
	tex_smoke_Room_Furnace = loadTexture(ren, "smoke_Room_Furnace.png");
	
	showScore = false;
	
	
	
	
	left = right = up = down = false;
	left2 = right2 = up2 = down2 = false;
}

void one_player_game_mode::set_map(const char *map){
	// Unload current map here...
	
	// Load new map here...
	if (!strcmp(map, "one")) {
		set_map_one:
		if (true);
	} else {
		// Handle bad map specification here.
		goto set_map_one;
	}
	// Be done.
}

void one_player_game_mode::clear(){
	player = defaultPlayer;
	playerTwo = defaultPlayerTwo;
	showScore = false;
	score = 0;
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
		case SDLK_KP_4:
			left = true;
			break;
		case SDLK_RIGHT:
		case SDLK_KP_6:
			right = true;
			break;
		case SDLK_UP:
		case SDLK_KP_8:
			up = true;
			break;
		case SDLK_DOWN:
		case SDLK_KP_5:
			down = true;
			break;
		//Two player key down
		case SDLK_a:
			left2 = true;
			break;
		case SDLK_d:
			right2 = true;
			break;
		case SDLK_w:
			up2 = true;
			break;
		case SDLK_s:
			down2 = true;
			break;
		}
		break;
		
		//KEY UP
	case SDL_KEYUP:
		if (event->key.repeat)
			break;
		switch (event->key.keysym.sym)
		{
		case SDLK_LEFT:
		case SDLK_KP_4:
			left = false;
			break;
		case SDLK_RIGHT:
		case SDLK_KP_6:
			right = false;
			break;
		case SDLK_UP:
		case SDLK_KP_8:
			up = false;
			break;
		case SDLK_DOWN:
		case SDLK_KP_5:
			down = false;
			break;
			
			// Second player key up
		case SDLK_a:
			left2 = false;
			break;
		case SDLK_d:
			right2 = false;
			break;
		case SDLK_w:
			up2 = false;
			break;
		case SDLK_s:
			down2 = false;
			break;
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
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
	//Player one Key things
	
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
	//Player two key things
	if (twoPlayerMode){
		if (up2 && !down2){
			playerTwo.velocity_y -= playerAcceleration;
		}
		if (down2 && !up2){
			playerTwo.velocity_y += playerAcceleration;
		}
		if (!up2 && !down2){
			playerTwo.velocity_y /= 1.025;
			if (fabs(playerTwo.velocity_y) < .05)
				playerTwo.velocity_y = 0;
		}	
		if (left2 && !right2){
			playerTwo.velocity_x -= playerAcceleration;
		}
		if (right2 && !left2){
			playerTwo.velocity_x += playerAcceleration;
		}
		if (!left2 && !right2){
			playerTwo.velocity_x /= 1.025;
			if (fabs(playerTwo.velocity_x) < .05)
				playerTwo.velocity_x = 0;
		}
	}
	
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    After player input logic
	circleCollisionDetection(&player, &playerTwo, true);
	
	//++++++++++++++ Final circle logic
	
	//Final player
	circleLogicCombined(&player);
	
	circleLogicCombined(&playerTwo);
	
	
	// Configure the camera.
	my_camera.clear();
	my_camera.considerObject(playerTwo.position_x, playerTwo.position_y, defaultPlayer.radius * 2.5);
	my_camera.considerObject(player.position_x, player.position_y, defaultPlayer.radius * 2.5);
        my_camera.calculateTransforms();
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDERER I SAID RENDERER RENDERER I WISHED I COULD DOUBLE CAPS LOCK BUT THAT IS IMPOSSIBLE 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void one_player_game_mode::render(SDL_Renderer *ren, TTF_Font *font){
	SDL_Rect dst;
	SDL_QueryTexture(tex_map, NULL, NULL, &dst.w, &dst.h);
	dst.x = 0;
	dst.y =0;
	my_camera.transform(&dst);
	SDL_RenderCopy(ren, tex_map, NULL, &dst);

	//(<expression> <bool expression>) ? <if true> : <if false>
	
	// Player one RENDER
	player.render(ren, &my_camera);
	
	playerTwo.render(ren, &my_camera);
	



	//Score being displayed constantly
	if (!twoPlayerMode){
		SDL_Color white = {255, 255, 255};
		SDL_Surface *score_surface = TTF_RenderText_Solid(font, ("Score: " + str((int)score)).c_str(), white);
		SDL_Texture *tex_score = SDL_CreateTextureFromSurface(ren, score_surface);
		SDL_FreeSurface(score_surface);
		SDL_QueryTexture(tex_score, NULL, NULL, &dst.w, &dst.h);
		dst.y = 30;
		dst.x = (1920 - dst.w)/2;
		SDL_RenderCopy(ren, tex_score, NULL, &dst);
		SDL_DestroyTexture(tex_score);
		
	}
	if (twoPlayerMode){
		SDL_Color blue = {0, 0, 255};
		SDL_Color yellow = {255, 255, 0};
		SDL_Color white = {255, 255, 255};
		double playerRadiusDifference = player.radius - playerTwo.radius;
		SDL_Surface *score_surface;
		if (playerRadiusDifference == 0){
			score_surface = TTF_RenderText_Solid(font, "TIED", white);
		}
		else {
			SDL_Color *score_color;
			if (playerRadiusDifference < 0){
				score_color = &blue;
			} else {
				score_color = &yellow;
			}
			score_surface = TTF_RenderText_Solid(font, ("Winning by: " + str((int)abs(playerRadiusDifference))).c_str(), *score_color);
		}
			SDL_Texture *tex_score = SDL_CreateTextureFromSurface(ren, score_surface);
			SDL_FreeSurface(score_surface);
			SDL_QueryTexture(tex_score, NULL, NULL, &dst.w, &dst.h);
			dst.y = 30;
			dst.x = (1920 - dst.w)/2;
			SDL_RenderCopy(ren, tex_score, NULL, &dst);
			SDL_DestroyTexture(tex_score);
			
		//Foreground
		SDL_QueryTexture(tex_foreground, NULL, NULL, &dst.w, &dst.h);
		dst.x = dst.y = 0;
		my_camera.transform(&dst);
		SDL_RenderCopy(ren, tex_foreground, NULL, &dst);
			
		//Furnace
		
		SDL_QueryTexture(tex_smoke_Room_Furnace, NULL, NULL, &dst.w, &dst.h);
		dst.x = 0;
		dst.y = 0;
		my_camera.transform(&dst);
		SDL_RenderCopy(ren, tex_smoke_Room_Furnace, NULL, &dst);
		
	}
}

one_player_game_mode::~one_player_game_mode(){
	SDL_DestroyTexture(tex_map);
	SDL_DestroyTexture(tex_foreground);
	SDL_DestroyTexture(tex_smoke_Room_Furnace);
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

static void wallHit (circle *circ)
{
	
	int topWall = 221;
	int botWall = 168;
	int leftWall = 331;
	int rightWall = 331;
	
	//left wall hit
	if(circ->position_x - circ->radius < leftWall){
		circ->position_x = leftWall + circ->radius;
		circ->velocity_x = fabs(circ->velocity_x);
	}
	//right wall hit
	if(circ->position_x + circ->radius > 1920*1.5 - rightWall){
		circ->position_x = 1920*1.5 - (rightWall + circ->radius);
		circ->velocity_x = -fabs(circ->velocity_x);
	}
	//top wall hit
	if(circ->position_y - circ->radius < topWall){
		circ->position_y = topWall + circ->radius;
		circ->velocity_y = fabs(circ->velocity_y);
	}
	//bottom wall hit
	if(circ->position_y + circ->radius > 1080*1.5 - botWall){
		circ->velocity_y = - fabs(circ->velocity_y);
		circ->position_y = 1080*1.5 - (botWall + circ->radius);
	}
}

//Combine all the simple circle logic into this
static void circleLogicCombined(circle *circ){
	
	vectorSpeedConstant(circ);
	if (fabs(circ->velocity_x) > 0.25 || fabs(circ->velocity_y) > 0.25)
		circ->angle = cart2angle(circ->velocity_x, circ->velocity_y);
	wallHit(circ);
	
	circ->position_x += circ->velocity_x;
	circ->position_y += circ->velocity_y;
}



//Maybe later on make this take up less room now that I know how to do it - Oh I did it alrgiht




static bool circleCollisionDetection(circle *circleOne, circle *circleTwo, bool collidePhysics){
	
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
                        circleCollisionResponse(circleOne, circleTwo, &circleOne_new_velocity_x, &circleOne_new_velocity_y);
                        // Don’t need to preserve circleTwo’s
                        // velocity, can just write it directly.
                        circleCollisionResponse(circleTwo, circleOne, &circleTwo->velocity_x, &circleTwo->velocity_y);
                        circleOne->velocity_x = circleOne_new_velocity_x;
                        circleOne->velocity_y = circleOne_new_velocity_y;
		}
		return true;
	}
	return false;
}

static void circleCollisionResponse(circle *circleOne, circle *circleTwo, double *circleOne_new_velocity_x, double *circleOne_new_velocity_y) {
	
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
	wallHit(circleOne);
	
	//Move away if overlapping 
	circleTwo->position_x = circleOne->position_x + cos(contactAngle/180*M_PI) * (circleTwo->radius + circleOne->radius );
	circleTwo->position_y = circleOne->position_y + sin(contactAngle/180*M_PI) * (circleTwo->radius + circleOne->radius );
}






