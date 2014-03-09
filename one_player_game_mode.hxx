#ifndef _TESTSDL_ONE_PLAYER_GAME_MODE_HXX
#define _TESTSDL_ONE_PLAYER_GAME_MODE_HXX

extern "C" {
#include <SDL.h>
}

#include "game_mode.hxx"

class one_player_game_mode : public game_mode {
public:
	one_player_game_mode(SDL_Renderer *ren);

	virtual bool processEvents(SDL_Event *event, int *current_game_mode);
	virtual void animate();
	virtual void render(SDL_Renderer *ren, TTF_Font *font);
	virtual ~one_player_game_mode();

private:
	double position_x;
	double position_y;
	double velocity_x;
	double velocity_y;
	double max_speed;
	double angle;
	SDL_Texture *tex_map;
	SDL_Texture *tex_playerOne;
	bool left, right, up, down;
};

#endif /* _TESTSDL_ONE_PLAYER_GAME_MODE_HXX */
