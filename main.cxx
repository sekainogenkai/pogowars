#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string> 
#include <streambuf>

extern "C" {
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
}

#include "game_mode.hxx"
#include "menu_game_mode.hxx"
#include "main.hxx"
#include "one_player_game_mode.hxx"


static Uint32 tickTimerCallback(Uint32 interval, void *param);

int main(int argc, char *argv[])
{
    /* http://twinklebear.github.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world/ */
    /* To compile in Geany, run SHIFT-F9 (or Build -> Make) */
    /* To run in Geany, hit F5. (will not automatically compile for you) or click Execute*/
    //This code will be filled with Kristofer's notes on what is happening because he is learning this for the first time.
    
    
    //This starts SDL and all the subsystems needed. Done through SDL_Int. And if SDL_Init does not work then return one to quit SDL and print out problem
	if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
		
	if(TTF_Init()==-1) {
		std::cout <<"TTF_Init: " << TTF_GetError() << std::endl;
		return 1;
	}
	// load font.ttf at size 16 into font
	TTF_Font *font;
	font=TTF_OpenFont("VeraMono.ttf", 20);
	if(!font) {
		std::cout <<"TTF_OpenFont:" << TTF_GetError() << std::endl;
		return 1;
	}
	
	
	SDL_Window *win = SDL_CreateWindow("Angry Chase", 100, 100, 1920/1.5, 1080/1.5, SDL_WINDOW_MAXIMIZED|SDL_WINDOW_RESIZABLE|SDL_WINDOW_FULLSCREEN_DESKTOP);
	void SDL_MaximizeWindow(SDL_Window* window);
    if (win == NULL){
	    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
	    return 1;
    }
    
    
    //Creating a Renderer: Using hardware accelerated rendering and with vsync. If something goes wrong with that what is written in orange will be printed
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL)
    {
	    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
	    std::cerr << "Trying without VSYNC" << std::endl;
	    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	    if (!ren)
	    {
		    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		    std::cerr << "Falling back to low-quality (no alpha blending, game may be unplayable) renderer." << std::endl;
		    ren = SDL_CreateRenderer(win, -1, 0);
		    if (!ren)
		    {
			    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
			    std::cerr << "Well, I need a renderer and I can’t even get a low-quality one. Giving up." << std::endl;
			    return 1;
		    }
	    }
    }

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Setting timer
	// Set up a user event for detecting timer events.
	// See http://wiki.libsdl.org/SDL_AddTimer
	Uint32 tickSdlEventCode = SDL_RegisterEvents(1);
	//60 fps
	int tickMilliseconds = 1000/60;
	//double tickSeconds = tickMilliseconds / 1000.0;
	SDL_TimerID tickTimerID = SDL_AddTimer(tickMilliseconds, tickTimerCallback, &tickSdlEventCode);
	if (!tickTimerID)
	{
		std::cerr << "SDL_AddTimer() Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ /end/ Setting timer
	
	//Start the things needed to run the game
	bool quit = false;
	SDL_Event event;
	SDL_ShowCursor(SDL_DISABLE);
	
	game_mode *game_modes[] = {
		new menu_game_mode(ren),
		new one_player_game_mode(ren, false),
		new one_player_game_mode(ren, true),
	};
	int current_game_mode = 0;


    while (!quit)
    {
		//Set up needed variables
		bool animate = true;
		bool redraw = true;
		
		while(SDL_WaitEvent(&event) && !quit){
			do
			{
				// call mode-specific event handler
				int old_current_game_mode = current_game_mode;
				game_modes[current_game_mode]->processEvents(&event, &current_game_mode);
				if ((size_t)current_game_mode >= sizeof(game_modes)/sizeof(game_modes[0]))
				{
					quit = true;
					break;
				}
				if (old_current_game_mode != current_game_mode)
					redraw = true;
				// run generic event handlers
				switch (event.type)
				{
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_WINDOWEVENT:
					switch (event.window.event)
					{
					case SDL_WINDOWEVENT_RESIZED:
						redraw = true;
						break;
					case SDL_WINDOWEVENT_CLOSE:
						quit = true;
						break;
					}
					break;
				case SDL_KEYDOWN:
					if (event.key.repeat)
						break;
					switch (event.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						quit = true;
						break;
					}
					break;
				case SDL_USEREVENT:
					if (event.user.type == tickSdlEventCode)
						animate = true;
					break;
				}
				
				// Eat all of the other events while we're at it.
			} while (SDL_PollEvent(&event));
			if (quit)
				break;
			
			if (animate)
			{
				game_modes[current_game_mode]->animate();
	
				// After updating animation stuffs, mark that we're ready for a redraw
				redraw = true;
				animate = false;
			}
	
			if (redraw)
			{
				SDL_RenderClear(ren);
				game_modes[current_game_mode]->render(ren, font);
				
				SDL_RenderPresent(ren);
				redraw = false;
			}
		}
	} /* while (!quit) */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ /end player one game
	for (size_t i = 0; i < sizeof(game_modes)/sizeof(game_mode*); i++){
		delete game_modes[i];
	}
	// Cleanup	
	SDL_RemoveTimer(tickTimerID);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
    
    /*system("PAUSE");*/
    return EXIT_SUCCESS;
}

SDL_Texture *loadTexture(SDL_Renderer *ren, const char *filename)
{
	SDL_Surface *bmp = IMG_Load(filename);
    if (!bmp)
    {
             std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
             abort();
             return NULL;
    }
    
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_FreeSurface(bmp);
    if (!tex)
    {
             std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
             abort();
             return NULL;
    }
    return tex;
}

static Uint32 tickTimerCallback(Uint32 interval, void *param)
{
	Uint32 tickSdlEventCode = *(Uint32*)param;
	// http://wiki.libsdl.org/SDL_UserEvent
	SDL_Event event;
	SDL_zero(event);
	event.type = SDL_USEREVENT;
	event.user.type = tickSdlEventCode;
	SDL_PushEvent(&event);
	// Specify the amount of time to wait before running this event again.
	return interval;
}



