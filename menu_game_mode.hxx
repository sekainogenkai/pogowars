#ifndef _TESTSDL_MENU_GAME_MODE_HXX
#define _TESTSDL_MENU_GAME_MODE_HXX

extern "C" {
#include <SDL.h>
}

#include "game_mode.hxx"
#include "one_player_game_mode.hxx"

class menu_game_mode : public game_mode {
public:
	menu_game_mode(SDL_Renderer *ren, one_player_game_mode *my_one_player_game_mode);

	virtual bool processEvents(SDL_Event *event, int *current_game_mode);
	virtual void animate();
	virtual void render(SDL_Renderer *ren, TTF_Font *font);
	virtual ~menu_game_mode();

private:
	one_player_game_mode *my_one_player_game_mode;
	bool up, down, left, right;
	bool up2, down2, left2, right2;
	enum menu_position {
		MENU_POSITION_START = 1,
		MENU_POSITION_SETTINGS,
		MENU_POSITION_CREDITS,
		MENU_POSITION_QUIT,
		} menuPosition;
	int menuRod_y;
	int blinkingLights;
	int credits_x;
	int redJoin_y;
	int blueJoin_y;
	bool redJoin;
	bool blueJoin;
	bool redReady;
	bool blueReady;
	bool redBack;
	bool blueBack;
	double gearAngle;
	bool  mainMenu;
	bool mainMenuVisible;
	bool showHelp;
	int textPos_x;
	int numberOfShade;
    int numberOfShade_wanted;
	SDL_Texture *tex_menuBack;
	SDL_Texture *tex_startGame;
	SDL_Texture *tex_settings;
	SDL_Texture *tex_credits;
	SDL_Texture *tex_quit;
	SDL_Texture *tex_menuRod;
	SDL_Texture *tex_menuGear;
	SDL_Texture *tex_lightForeground;
	SDL_Texture *tex_menuP;
	SDL_Texture *tex_keyboardInstructions;
	SDL_Texture *tex_shade;
	SDL_Texture *tex_startGame_clickToJoin;
	SDL_Texture *tex_startGame_blinkingLights;
	SDL_Texture *tex_inCredits;
	SDL_Texture *tex_redJoin;
	SDL_Texture *tex_blueJoin;
	SDL_Texture *tex_READY;
	SDL_Texture *tex_READY2;
	SDL_Texture *tex_inQuit;
	SDL_Texture *tex_BACK;
	SDL_Texture *tex_BACK2;
	int yinAndYangAngle;
};

#endif /* _TESTSDL_MENU_GAME_MODE_HXX */
