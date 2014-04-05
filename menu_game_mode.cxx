#include <iostream>

#include "menu_game_mode.hxx"
#include "main.hxx"


menu_game_mode::menu_game_mode(SDL_Renderer *ren){
	menuPosition = 1;
	mainMenu = true;
	textPos_x = 0;
	menuRod_y = 0;
	blinkingLights = 0;
	numberOfShade = 0;
    numberOfShade_wanted = 0;
    mainMenuVisible = true;
	up = down = left = right = up2 = down2 = left2 = right2 = false;
	tex_menuBack = loadTexture(ren, "menuBack.png");
	tex_startGame = loadTexture(ren, "startGame.png");
	tex_settings = loadTexture(ren, "settings.png");
	tex_credits = loadTexture(ren, "credits.png");
	tex_quit = loadTexture(ren, "quit.png");
	tex_menuRod = loadTexture(ren, "menuRod.png");
	tex_menuGear = loadTexture(ren, "menuGear.png");
	tex_lightForeground = loadTexture(ren, "lightForeground.png");
	tex_menuP = loadTexture(ren, "menuP.png");
	tex_keyboardInstructions = loadTexture (ren, "keyboardInstructions.png");
	tex_shade = loadTexture (ren, "shade.png");
	tex_startGame_clickToJoin = loadTexture (ren, "startGame_clickToJoin.png");
	tex_startGame_blinkingLights = loadTexture (ren, "startGame_blinkingLights.png");
	tex_inCredits = loadTexture (ren, "inCredits.png");
	tex_blueJoin = loadTexture (ren, "blueJoin.png");
	tex_redJoin = loadTexture (ren, "redJoin.png");
	tex_READY = loadTexture (ren, "READY.png");
	tex_READY2 = loadTexture (ren, "READY2.png");
	tex_inQuit = loadTexture (ren, "inQuit.png");
	tex_BACK = loadTexture (ren, "BACK.png");
	tex_BACK2 = loadTexture (ren, "BACK2.png");
	yinAndYangAngle = 10;
}

bool menu_game_mode::processEvents(SDL_Event *event, int *current_game_mode){
	switch (event->type)
	{
	case SDL_KEYDOWN:
		if (event->key.repeat)
			break;
		switch (event->key.keysym.sym)
		{
		// case Up for both players
		case SDLK_UP:
		case SDLK_KP_8:
			up = true;
			break;
		case SDLK_w:
			up2 = true;
			break;
		
		// case Down for both players
		case SDLK_DOWN:
		case SDLK_KP_5:
			down = true;
			break;
		case SDLK_s:
			down2 = true;
			break;
		
		// case left for both players
		case SDLK_LEFT:
		case SDLK_KP_4:
			left = true;
			break;
		case SDLK_a:
			left2 = true;
			break;
		
		// case right for both players
		case SDLK_RIGHT:
		case SDLK_KP_6:
			right = true;
			break;
		case SDLK_d:
			right2 = true;
			break;
		
		// case show help information
		case SDLK_h:
			if(showHelp){
				showHelp = false;
			}else{
				showHelp = true;
			}
			break;
		// pressing M brings you back to the main menu
		case SDLK_m:
			mainMenu = true;
			showHelp = false;
			break;
			
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
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
	
	SDL_Rect dst;
	
	
	//Main menu logic
	if (mainMenu && !showHelp){
		redJoin_y = -1080;
		blueJoin_y = -1080;
		credits_x = -1920;
		blueJoin = false;
		redJoin = false;
		redReady = false;
        blueReady = false;
		mainMenuVisible = true;
		redBack = false;
		blueBack = false;
	// Menu position
		if ((up || up2) && menuPosition > 1)
			menuPosition--;
		if ((down || down2) && menuPosition < 4)
			menuPosition++;
		if (right || right2)
			mainMenu = false;
	//This is for the menu rod
		for (int i=0; i<6; i++){
			int menuRod_y_wanted = menuPosition * 67 - 64;
			if (menuRod_y_wanted == menuRod_y)
				break;
			if (menuRod_y < menuRod_y_wanted){
				menuRod_y++;
				gearAngle -= .72;
			}
			if (menuRod_y > menuRod_y_wanted){
				menuRod_y--;
				gearAngle += .72;
			}
		}
	}
	if (mainMenuVisible){
		
		//back of menu
		SDL_RenderCopy(ren, tex_menuBack, NULL, NULL);
		// Display what part of main menu on
		if(menuPosition == 1)
			SDL_RenderCopy(ren, tex_startGame, NULL, NULL);
		else if (menuPosition == 2)
				SDL_RenderCopy(ren, tex_settings, NULL, NULL);
		else if (menuPosition == 3)
			SDL_RenderCopy(ren, tex_credits, NULL, NULL);
		else if (menuPosition == 4)
			SDL_RenderCopy(ren, tex_quit, NULL, NULL);
		
		//RENDER menu gear
		dst.x = 674;
		dst.y = 91;
		SDL_QueryTexture(tex_menuGear, NULL, NULL, &dst.w, &dst.h);
		SDL_RenderCopyEx(ren, tex_menuGear, NULL, &dst, gearAngle, NULL, SDL_FLIP_NONE);
	
		//RENDER menuRod
		dst.x = 648;
	
		dst.y = menuRod_y - 200;
		SDL_QueryTexture(tex_menuRod, NULL, NULL, &dst.w, &dst.h);
		SDL_RenderCopy(ren, tex_menuRod,NULL, &dst);
	
		//RENDER menuP
		dst.x = 633;
		dst.y = 6;
		SDL_QueryTexture(tex_menuP, NULL, NULL, &dst.w, &dst.h);
		SDL_RenderCopy(ren, tex_menuP,NULL, &dst);
	
		//RENDER light foreground
		SDL_RenderCopy(ren, tex_lightForeground, NULL, NULL);
	
	
		// menu shading
		if (numberOfShade < numberOfShade_wanted){
			numberOfShade++;
		}
		if (numberOfShade > numberOfShade_wanted){
			numberOfShade--;
		}
	for (int i = 0; i < numberOfShade; i++){
		SDL_RenderCopy(ren, tex_shade, NULL, NULL);
	}
	}
	
	// If help instructions
		if (showHelp && mainMenu){
			if (left || left2){
				showHelp = false;
			}
			numberOfShade_wanted = 50;
			SDL_RenderCopy(ren, tex_keyboardInstructions, NULL, NULL);
		}else{
			numberOfShade_wanted = 0;
		}
	// If out of main menu
	if (!mainMenu){
		if (menuPosition == 1){   //Start Game
				SDL_RenderCopy(ren, tex_startGame_clickToJoin, NULL, NULL);
				
				//Blinking keyboard instructions
				blinkingLights += 1;
				if (blinkingLights > 50)
					blinkingLights = 0;
				if (blinkingLights > 25)
					SDL_RenderCopy(ren, tex_startGame_blinkingLights, NULL, NULL);
					
				numberOfShade_wanted = 50;
			if (down)
				blueJoin = true;
			if (down2)
				redJoin = true;
			if (up)
				blueJoin = false;
			if (up2)
				redJoin = false;
			if (blueJoin && blueJoin_y < 0){
				blueJoin_y += 90;
			}
			if (!blueJoin && blueJoin_y > -1080){
				blueJoin_y -= 90;
			}
			if (redJoin && redJoin_y < 0){
				redJoin_y += 90;
			}
			if (!redJoin && redJoin_y > -1080){
				redJoin_y -= 90;
			}
			if (blueJoin || redJoin){
				mainMenuVisible = false;
			}
			
			//RENDER both blue and red join
			dst.x = 0;
			dst.y = redJoin_y;
			SDL_QueryTexture(tex_redJoin, NULL, NULL, &dst.w, &dst.h);
			SDL_RenderCopy(ren, tex_redJoin,NULL, &dst);
			
			dst.x = 1184;
			dst.y = blueJoin_y;
			SDL_QueryTexture(tex_blueJoin, NULL, NULL, &dst.w, &dst.h);
			SDL_RenderCopy(ren, tex_blueJoin,NULL, &dst);
			
			//The I am ready logic
			if (right){
				blueReady = true;
				blueBack = false;
			}
			if (left){
				blueReady = false;
				blueBack = true;
			}
			if (blueJoin_y < 0)
				blueReady = false;
			if (right2){
				redReady = true;
				redBack = false;
			}
			if (left2){
				redReady = false;
				redBack = true;
			}
			if (redJoin_y < 0)
				redReady = false;
				//Display ready stuff
				dst.y = 924;
			if (redBack){
				dst.x = 22;
				SDL_QueryTexture(tex_BACK, NULL, NULL, &dst.w, &dst.h);
				SDL_RenderCopy(ren, tex_BACK,NULL, &dst);
			}
			if (blueBack){
				dst.x = 1211;
				SDL_QueryTexture(tex_BACK2, NULL, NULL, &dst.w, &dst.h);
				SDL_RenderCopy(ren, tex_BACK2,NULL, &dst);
			}
			if (blueBack && redBack){
				mainMenu = true;
			}
			if (redReady){
				dst.x = 471;
				SDL_QueryTexture(tex_READY, NULL, NULL, &dst.w, &dst.h);
				SDL_RenderCopy(ren, tex_READY,NULL, &dst);
			}
			if (blueReady){
				dst.x = 1658;
				SDL_QueryTexture(tex_READY2, NULL, NULL, &dst.w, &dst.h);
				SDL_RenderCopy(ren, tex_READY2,NULL, &dst);
			}
			
		}
		if (menuPosition == 2){  //Settings
			if (left || left2){
				mainMenu = true;
			}
		}
		if (menuPosition == 3){  //Credits
			if (credits_x <0){
				credits_x += 100;
			}
			dst.x = credits_x;
			dst.y = 0;
			SDL_QueryTexture(tex_inCredits, NULL, NULL, &dst.w, &dst.h);
			SDL_RenderCopy(ren, tex_inCredits,NULL, &dst);
			numberOfShade_wanted = 50;
			if (left || left2){
				mainMenu = true;
			}
		}
		if (menuPosition == 4){   //Quit
			numberOfShade_wanted = 60;
			dst.x = 640;
			dst.y = 360;
			SDL_QueryTexture(tex_inQuit, NULL, NULL, &dst.w, &dst.h);
			SDL_RenderCopy(ren, tex_inQuit,NULL, &dst);
			
			if (left || left2){
				mainMenu = true;
			}
			if (right || right2){
				
			}
		}
	}
	
	// Must be done after all input
	up = down = left = right = up2 = down2 = left2 = right2 = false;
	
	//RENDER menu position
	SDL_Color colorBlack = {45,45,45};
	SDL_Surface *text_surface = TTF_RenderText_Solid(font, str(menuPosition).c_str(),colorBlack);
	SDL_Texture *tex_place = SDL_CreateTextureFromSurface(ren, text_surface);
	SDL_FreeSurface(text_surface);
	dst.x = 499;
	dst.y = 500;
	SDL_QueryTexture(tex_place, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopy(ren, tex_place, NULL, &dst);
	SDL_DestroyTexture(tex_place);
}

menu_game_mode::~menu_game_mode(){
	SDL_DestroyTexture(tex_menuBack);
	SDL_DestroyTexture(tex_startGame);
	SDL_DestroyTexture(tex_settings);
	SDL_DestroyTexture(tex_credits);
	SDL_DestroyTexture(tex_quit);
	SDL_DestroyTexture(tex_menuRod);
	SDL_DestroyTexture(tex_menuGear);
	SDL_DestroyTexture(tex_lightForeground);
	SDL_DestroyTexture(tex_menuP);
	SDL_DestroyTexture(tex_keyboardInstructions);
	SDL_DestroyTexture(tex_shade);
	SDL_DestroyTexture(tex_startGame_clickToJoin);
	SDL_DestroyTexture(tex_startGame_blinkingLights);
	SDL_DestroyTexture(tex_inCredits);
	SDL_DestroyTexture(tex_blueJoin);
	SDL_DestroyTexture(tex_redJoin);
	SDL_DestroyTexture(tex_inQuit);
	SDL_DestroyTexture(tex_BACK);
	SDL_DestroyTexture(tex_BACK2);
}
