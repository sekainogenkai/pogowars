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
	double wallWidth;
	double fearAccelerationRatio;
	double angerAccelerationRatio;
	SDL_Texture *tex_map;
	bool left, right, up, down;
};

#endif /* _TESTSDL_ONE_PLAYER_GAME_MODE_HXX */
