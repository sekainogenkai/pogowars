#ifndef _TESTSDL_ONE_PLAYER_GAME_MODE_HXX
#define _TESTSDL_ONE_PLAYER_GAME_MODE_HXX

extern "C" {
#include <SDL.h>
#include <math.h>
}

#include "camera.hxx"
#include "game_mode.hxx"

class circle {
public:
	double position_x;
	double position_y;
	double velocity_x;
	double velocity_y;
	double radius;
	double max_speed;
	double angle;
	//Vector speed function inside circle class to make it easier to get the vector
	double vectorSpeed (void) {
		return sqrt (pow(velocity_x, 2) + pow(velocity_y,2)); 
		};
	double vectorAngle (void) {
		double x = velocity_x;
		double y = velocity_y;
		if (fabs(x) < 0.0001)
		return y > 0 ? 90 : 270;
	else
		return fmod(180 * atanf(y/x) / M_PI + (x > 0 ? 0 : 180) + 360, 360);
	};
	SDL_Texture *tex;
	
	circle(double start_x, double start_y, double radius, double max_speed);
	virtual ~circle();
};

// This subclass of circle handles tracking, rendering, cleaning its own texture.
class textured_circle : public circle {
private:
	void *this_that_should_kill_tex; // HACK
	SDL_Texture *tex;
	
public:
	textured_circle(SDL_Renderer *ren, const char *texfile, double start_x, double start_y, double radius, double max_speed);
	
	void render(SDL_Renderer *ren, camera *my_camera);
	
	virtual ~textured_circle();
};

class particle_circle : public circle {
public:
	particle_circle();
	virtual ~particle_circle();
	bool enabled;
};
	

class one_player_game_mode : public game_mode {
public:
	one_player_game_mode(SDL_Renderer *ren);

	void set_map(const char *map);
	virtual bool processEvents(SDL_Event *event, int *current_game_mode);
	virtual void animate();
	virtual void render(SDL_Renderer *ren, TTF_Font *font);
	virtual ~one_player_game_mode();

private:
	void clear();

	camera my_camera; 
	bool twoPlayerMode;
	textured_circle defaultPlayer, player;
	textured_circle defaultPlayerTwo, playerTwo;
	textured_circle defaultAnger, anger;
	textured_circle defaultFear, fear;
	textured_circle yinAndYangCircle;
	particle_circle watermelons[3];
	particle_circle rcircle[50];
	double wallWidth;
	double fearAccelerationRatio;
	double angerAccelerationRatio;
	double playerSeedStrength;
	double playerAcceleration;
	int score;
	int radiusDifferentMax;
	int lowerRadius;
	SDL_Texture *tex_map;
	SDL_Texture *tex_wall;
	SDL_Texture *tex_watermelon;
	SDL_Texture *tex_rcircle;
	SDL_Texture *tex_endGame;
	SDL_Texture *tex_rcircle2;
	SDL_Texture *tex_rightWin;
	SDL_Texture *tex_leftWin;
	bool left, right, up, down;
	bool left2, right2, up2, down2;
	bool showScore;
};

#endif /* _TESTSDL_ONE_PLAYER_GAME_MODE_HXX */
