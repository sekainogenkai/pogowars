#include <iostream>

#include "menu_game_mode.hxx"
#include "main.hxx"


menu_game_mode::menu_game_mode(SDL_Renderer *ren, one_player_game_mode *my_one_player_game_mode)
: my_one_player_game_mode(my_one_player_game_mode)
{
	menuPosition = MENU_POSITION_START;
	mainMenu = true;
	textPos_x = 0;
	menuRod_y = 0;
	blinkingLights = 0;
	numberOfShade = 0;
    numberOfShade_wanted = 0;
    mainMenuVisible = true;
    blueReady = false;
    redReady = false;
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
	tex_NAMES_MAPS = loadTexture (ren, "NAMES_MAPS.png");
	tex_THUMB_SkyBridge = loadTexture (ren, "THUMB_SkyBridge.png");
	tex_THUMB_SmokeRoom =  loadTexture (ren, "THUMB_SmokeRoom.png");
	tex_inMapChoose = loadTexture (ren, "inMapChoose.png");
	tex_greenUp = loadTexture (ren, "greenUp.png");
	tex_greenDown = loadTexture (ren, "greenDown.png");
	
	
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
			if (!mainMenu && menuPosition == MENU_POSITION_QUIT)
				*current_game_mode = -1;
			break;
		case SDLK_d:
			right2 = true;
			if (!mainMenu && menuPosition == MENU_POSITION_QUIT)
				*current_game_mode = -1;
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
			
		
		}
		break;
	}
	if (inMapChoose && (right || right2)){
		*current_game_mode = 1;
	}
	if (false)
		my_one_player_game_mode->set_map("one");
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
		map_text_pos = 0;
		map_menu_pos = 1;
		map_text_pos = 503;
		map_text_pos_actual = 503;
		arrowShowUp = 0;
	    arrowShowDown = 0;
		blueJoin = false;
		redJoin = false;
		redReady = false;
        blueReady = false;
		mainMenuVisible = true;
		redBack = false;
		blueBack = false;
		inMapChoose = false;
	// Menu position
		if ((up || up2) && menuPosition > MENU_POSITION_START)
			menuPosition = static_cast<menu_position>(menuPosition - 1);
		if ((down || down2) && menuPosition < MENU_POSITION_QUIT)
			menuPosition = static_cast<menu_position>(menuPosition + 1);
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
		if(menuPosition == MENU_POSITION_START)
			SDL_RenderCopy(ren, tex_startGame, NULL, NULL);
		else if (menuPosition == MENU_POSITION_SETTINGS)
				SDL_RenderCopy(ren, tex_settings, NULL, NULL);
		else if (menuPosition == MENU_POSITION_CREDITS)
			SDL_RenderCopy(ren, tex_credits, NULL, NULL);
		else if (menuPosition == MENU_POSITION_QUIT)
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
		const int shadesPerTick = 12;
		if (numberOfShade < numberOfShade_wanted - shadesPerTick + 1){
			numberOfShade += shadesPerTick;
		}
		if (numberOfShade > numberOfShade_wanted + shadesPerTick - 1){
			numberOfShade -= shadesPerTick;
		}
		SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(ren, 0, 0,0, numberOfShade);
        dst.x = dst.y = 0;
        dst.w = 1920;
        dst.h = 1080;
        SDL_RenderFillRect(ren, &dst);
        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);
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
		
		if (inMapChoose){  // MAP CHOOSE +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ MAP CHOOSE BEACON Because it is in a weird spot
			
				SDL_RenderCopy(ren, tex_inMapChoose,NULL, NULL);
				
				//Moving up and down
				if ((up || up2) && map_menu_pos > 1){
					
					map_menu_pos -= 1;
				}
				if ((down || down2) && map_menu_pos < 2){
					
					map_menu_pos += 1;
				}
				
				//Map text display
				dst.x = 223;
				dst.y = map_text_pos_actual;
				SDL_QueryTexture(tex_NAMES_MAPS, NULL, NULL, &dst.w, &dst.h);
				SDL_RenderCopy(ren, tex_NAMES_MAPS, NULL, &dst);
				
				//Display up and or down arrows
				
				if (up || up2){
					arrowShowUp = 10;
				}
				if (down || down2){
					arrowShowDown = 10;
				}
				if (arrowShowUp > 0){
					arrowShowUp--;
					SDL_RenderCopy(ren, tex_greenUp, NULL, NULL);
				}
				if (arrowShowDown > 0){
					arrowShowDown--;
					SDL_RenderCopy(ren, tex_greenDown, NULL, NULL);
				}
				
				
				//map display
				dst.x = 970;
				dst.y = 140;
				
				//Displaying the thumbnails for the maps
				if (map_menu_pos == 1){
					SDL_QueryTexture(tex_THUMB_SmokeRoom, NULL, NULL, &dst.w, &dst.h);
					SDL_RenderCopy(ren, tex_THUMB_SmokeRoom, NULL, &dst);
				}
				if (map_menu_pos == 2){
					SDL_QueryTexture(tex_THUMB_SkyBridge, NULL, NULL, &dst.w, &dst.h);
					SDL_RenderCopy(ren, tex_THUMB_SkyBridge, NULL, &dst);
				}
				
				//Map text logic
				map_text_pos = 610 - map_menu_pos*113;
				for (int i=0; i<12; i++){
					if (map_text_pos < map_text_pos_actual)
						map_text_pos_actual--;
					if (map_text_pos > map_text_pos_actual)
						map_text_pos_actual++;
				}
				
				
				
		}
		
		
		
	// If out of main menu
	if (!mainMenu && !inMapChoose){
		switch (menuPosition)
		{
		case MENU_POSITION_START: //Start Game
			SDL_RenderCopy(ren, tex_startGame_clickToJoin, NULL, NULL);

			//Blinking keyboard instructions
			blinkingLights += 1;
			if (blinkingLights > 50)
				blinkingLights = 0;
			if (blinkingLights > 25)
				SDL_RenderCopy(ren, tex_startGame_blinkingLights, NULL, NULL);

			numberOfShade_wanted = 255;
			if (numberOfShade == 255){
				mainMenuVisible = false;
			}
			
			
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
			if (redReady && blueReady){
					inMapChoose = true;
				}
			}
			break;
		case MENU_POSITION_SETTINGS: //Settings
			if (left || left2){
				mainMenu = true;
			}
			break;
		case MENU_POSITION_CREDITS: //Credits
			if (credits_x <0){
				credits_x += 110;
			}
			dst.x = credits_x;
			dst.y = 0;
			SDL_QueryTexture(tex_inCredits, NULL, NULL, &dst.w, &dst.h);
			SDL_RenderCopy(ren, tex_inCredits,NULL, &dst);
			numberOfShade_wanted = 128+64;
			if (left || left2){
				mainMenu = true;
			}
			break;
		case MENU_POSITION_QUIT: //Quit
			numberOfShade_wanted = 128+64;
			dst.x = 640;
			dst.y = 360;
			SDL_QueryTexture(tex_inQuit, NULL, NULL, &dst.w, &dst.h);
			SDL_RenderCopy(ren, tex_inQuit,NULL, &dst);
			
			if (left || left2){
				mainMenu = true;
			}
			if (right || right2){
				
			}
			break;
			
			
			
		} // switch (menuPosition)
			
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
	SDL_DestroyTexture(tex_startGame_clickToJoin);
	SDL_DestroyTexture(tex_startGame_blinkingLights);
	SDL_DestroyTexture(tex_inCredits);
	SDL_DestroyTexture(tex_blueJoin);
	SDL_DestroyTexture(tex_redJoin);
	SDL_DestroyTexture(tex_inQuit);
	SDL_DestroyTexture(tex_BACK);
	SDL_DestroyTexture(tex_BACK2);
	SDL_DestroyTexture(tex_NAMES_MAPS);
	SDL_DestroyTexture(tex_THUMB_SkyBridge);
	SDL_DestroyTexture(tex_THUMB_SmokeRoom);
	SDL_DestroyTexture(tex_inMapChoose);
	SDL_DestroyTexture(tex_greenUp);
	SDL_DestroyTexture(tex_greenDown);
}
