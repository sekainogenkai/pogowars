#include <iostream>

#include "menu_game_mode.hxx"
#include "main.hxx"

menu_game_mode::menu_game_mode(SDL_Renderer *ren){
	menuPosition = 1;
	textPos_x = 0;
	tex = loadTexture(ren, "Angry_Chase.png");
	tex_menuSelectBar = loadTexture(ren, "menuSelectBar.png");
}

bool menu_game_mode::processEvents(SDL_Event *event, int *current_game_mode){
	switch (event->type)
	{
	case SDL_KEYDOWN:
		if (event->key.repeat)
			break;
		switch (event->key.keysym.sym)
		{
		case SDLK_UP:
			menuPosition--;
			if(menuPosition < 1)
				menuPosition = 1;
			break;
		case SDLK_DOWN:
			menuPosition++;
			if(menuPosition > 3)
				menuPosition =3;
			break;
		case SDLK_RETURN:
			*current_game_mode = menuPosition;
			break;
		}
		break;
	}
	return false;
}

void menu_game_mode::animate(){
	textPos_x++;
}

void menu_game_mode::render(SDL_Renderer *ren, TTF_Font *font){
	
	int position_x = 564;
	
	int menuBar_y;
	if(menuPosition == 1)
		menuBar_y = 281;
	else if (menuPosition == 2)
		menuBar_y = 522;
	else if (menuPosition == 3)
		menuBar_y = 789;

	SDL_RenderCopy(ren, tex, NULL, NULL);
	// Load menu select bar and specify its position
	SDL_Rect dst;
	dst.x = position_x;
	dst.y = menuBar_y;
	
	SDL_QueryTexture(tex_menuSelectBar, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopyEx(ren, tex_menuSelectBar, NULL, &dst, 0, NULL, SDL_FLIP_NONE);

	//For displaying xPos
	SDL_Color colorBlack = {0,0,0};
	SDL_Surface *text_surface = TTF_RenderText_Solid(font, str(position_x).c_str(),colorBlack);
	SDL_Texture *tex_place = SDL_CreateTextureFromSurface(ren, text_surface);
	SDL_FreeSurface(text_surface);
	dst.x = 499;
	dst.y = 500;
	SDL_QueryTexture(tex_place, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopy(ren, tex_place, NULL, &dst);
	SDL_DestroyTexture(tex_place);
	
	// Y display
	//For display menuBar_y
	SDL_Color colorSomething = {45,1,200};
	text_surface = TTF_RenderText_Solid(font, str(menuBar_y).c_str(), colorSomething);
	SDL_Texture *tex_menuBar_y = SDL_CreateTextureFromSurface(ren, text_surface);
	SDL_FreeSurface(text_surface);
	dst.y = 500;
	dst.x = textPos_x;
	SDL_RenderCopy(ren, tex_menuBar_y, NULL, &dst);
	SDL_DestroyTexture(tex_menuBar_y);
	
}

menu_game_mode::~menu_game_mode(){
	SDL_DestroyTexture(tex);
	SDL_DestroyTexture(tex_menuSelectBar);
}
