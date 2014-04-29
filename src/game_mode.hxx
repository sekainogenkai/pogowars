#ifndef _TESTSDL_GAME_MODE_HXX
#define _TESTSDL_GAME_MODE_HXX

extern "C" {
#include <SDL.h>
#include <SDL_ttf.h>
}

class game_mode {
public:
	virtual bool processEvents(SDL_Event *event, int *current_game_mode) = 0;
	virtual void animate() = 0;
	virtual void render(SDL_Renderer *ren, TTF_Font *font) = 0;
	virtual ~game_mode();
};

#endif /* _TESTSDL_GAME_MODE_HXX */
