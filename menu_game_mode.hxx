#ifndef _TESTSDL_MENU_GAME_MODE_HXX
#define _TESTSDL_MENU_GAME_MODE_HXX

extern "C" {
#include <SDL.h>
}

#include "game_mode.hxx"

class menu_game_mode : public game_mode {
public:
	menu_game_mode(SDL_Renderer *ren);

	virtual bool processEvents(SDL_Event *event, int *current_game_mode);
	virtual void animate();
	virtual void render(SDL_Renderer *ren, TTF_Font *font);
	virtual ~menu_game_mode();

private:
	int menuPosition;
	int textPos_x;
	SDL_Texture *tex;
	SDL_Texture *tex_menuSelectBar;
	SDL_Texture *tex_yinAndYang;
	SDL_Texture *tex_menuBack;
	int yinAndYangAngle;
};

#endif /* _TESTSDL_MENU_GAME_MODE_HXX */
