#ifndef _TESTSDL_ONE_PLAYER_GAME_MODE_HXX
#define _TESTSDL_ONE_PLAYER_GAME_MODE_HXX

extern "C" {
#include <SDL.h>
}

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
	
	circle(SDL_Renderer *ren, const char *texfile, double start_x, double start_y, double radius, double max_speed);
	virtual ~circle();
};

class one_player_game_mode : public game_mode {
public:
	one_player_game_mode(SDL_Renderer *ren);

	virtual bool processEvents(SDL_Event *event, int *current_game_mode);
	virtual void animate();
	virtual void render(SDL_Renderer *ren, TTF_Font *font);
	virtual ~one_player_game_mode();

private:
	circle player;
	circle anger;
	circle fear;
	circle yinAndYangCircle;
	double wallWidth;
	double fearAccelerationRatio;
	double angerAccelerationRatio;
	double playerSeedStrength;
	double playerAcceleration;
	SDL_Texture *tex_map;
	SDL_Texture *tex_wall;
	bool left, right, up, down;
};

#endif /* _TESTSDL_ONE_PLAYER_GAME_MODE_HXX */
