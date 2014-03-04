#include <cstdlib>
#include <iostream>
extern "C" {
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
}
static SDL_Texture *loadTexture(SDL_Renderer *ren, const char *filename);
static Uint32 tickTimerCallback(Uint32 interval, void *param);
static double cart2angle(double x, double y);

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
	font=TTF_OpenFont("VeraMono.ttf", 16);
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
    
    
    
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Make writing
    SDL_Color color = {0,255,0};
	SDL_Surface *text_surface = TTF_RenderText_Solid(font,"Hello World",color);
	if (!text_surface){
		std::cout <<"TTF_RenderText_Solid: also Krisotfer is pretty cool" << TTF_GetError() << std::endl;
		return 1;
	}
	
	SDL_Texture *tex_text = SDL_CreateTextureFromSurface(ren, text_surface);
	SDL_FreeSurface(text_surface);
    if (!tex_text)
    {
             std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
             return 1;
    }
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ /end/ make writing
	
    //Loading a png image. Loads the image and output Unable to load if not able to load 
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Load images
    SDL_Texture *tex = loadTexture(ren, "Angry_Chase.png");
    if (!tex)
    {
		std::cerr << "Unable to load Angry_Chase.png" << std::endl;
		return 1;
	}
	
	SDL_Texture *tex_pakman = loadTexture(ren, "pakman.png");
	if (!tex_pakman)
	{
		std::cerr << "Unable to load pakman.png" <<std::endl;
		return 1;
	}
	
	
	SDL_Texture *tex_menuSelectBar = loadTexture(ren, "menuSelectBar.png");
	if(!tex_menuSelectBar);
	{
		std::cerr << "Unable to load menu SelectBar.png" <<std::endl;
		return 1;
	}
	
	
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Load images


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Setting timer
	// Set up a user event for detecting timer events.
	// See http://wiki.libsdl.org/SDL_AddTimer
	Uint32 tickSdlEventCode = SDL_RegisterEvents(1);
	//60 fps
	int tickMilliseconds = 1000/60;
	double tickSeconds = tickMilliseconds / 1000.0;
	SDL_TimerID tickTimerID = SDL_AddTimer(tickMilliseconds, tickTimerCallback, &tickSdlEventCode);
	if (!tickTimerID)
	{
		std::cerr << "SDL_AddTimer() Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Setting timer
	
	

	double pakman_speed = 500;
	double position_x = 64;
	double position_y = 64;
	double direction_x = 0;
	double direction_y = 0;
	double angle = 0;
	double angle_target = 0;
	double angular_speed = 10900;

	bool quit = false;
	bool animate = true;
	bool redraw = true;
	SDL_Event event;
    while (!quit && SDL_WaitEvent(&event))
    {
		do
		{
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
			case SDL_KEYUP:
				if (event.key.repeat)
					break;
				switch (event.key.keysym.sym)
				{
					case SDLK_RIGHT:
						if (event.key.state == SDL_PRESSED)
						{
							direction_x = 1;
							angle_target = 0;
						}
						else
							if (direction_x > 0)
								direction_x = 0;
						break;
					case SDLK_LEFT:
						if (event.key.state == SDL_PRESSED)
							direction_x = -1;
						else
							if (direction_x < 0)
								direction_x = 0;
						break;
					case SDLK_UP:
						if (event.key.state == SDL_PRESSED)
							// (0, 0) is upper-left hand corner.
							direction_y = -1;
						else
							if (direction_y < 0)
								direction_y = 0;
						break;
					case SDLK_DOWN:
						if (event.key.state == SDL_PRESSED)
							// (0, 0) is upper-left hand corner.
							direction_y = 1;
						else
							if (direction_y > 0)
								direction_y = 0;
						break;
					case SDLK_ESCAPE:
						quit = 1;
						break;
				}
				// Recalculate angle because the key handlers may have changed the direction
				if (fabs(direction_x) < 0.01 && fabs(direction_y) > 0.01)
					angle_target = direction_y > 0 ? 90 : 270;
				else if (fabs(direction_x) > 0.01)
					angle_target = cart2angle(direction_x, direction_y);
				else
					// pakman has stopped moving, stop turning too
					angle_target = angle;
				break;
			case SDL_USEREVENT:
				if (event.user.type == tickSdlEventCode)
					animate = true;
				break;
			}
			// Eat all of the other events while we're at it.
		} while (SDL_PollEvent(&event));

		if (animate)
		{
			// Amount pakman will rotate given his velocity
			double angle_motion = angular_speed*tickSeconds;
			if (fabs(angle_target - angle) < angle_motion)
			{
				// If pakman would rotate past the target angle, just set his angle
				angle = angle_target;

				// pakman only moves if he's pointing in the right direction and if he's supposedly moving.
				if (direction_x || direction_y)
				{
					position_x += cosf(angle / 180 * M_PI) * pakman_speed * tickSeconds;
					position_y += sinf(angle / 180 * M_PI) * pakman_speed * tickSeconds;
				}
			}
			else
			{
				// Otherwise, get pakman's angle closer to the target angle
				if (fmod(angle_target - angle + 360, 360) < 180)
					// then go counter-clockwsise
					angle += angle_motion;
				else
					// clockwise
					angle -= angle_motion;
				angle = fmod(angle + 360, 360);
			}

			// After updating animation stuffs, mark that we're ready for a redraw
			redraw = true;
			animate = false;
		}

		if (redraw)
		{
			SDL_RenderClear(ren);
			SDL_RenderCopy(ren, tex, NULL, NULL);
			
			// Load pakman rectangle's size and specify its position.
			SDL_Rect dst;
			dst.x = position_x;
			dst.y = position_y;
			SDL_QueryTexture(tex_pakman, NULL, NULL, &dst.w, &dst.h);
			SDL_RenderCopyEx(ren, tex_pakman, NULL, &dst, 0, NULL, SDL_FLIP_NONE);
			
			SDL_QueryTexture(tex_menuSelectBar, NULL, NULL, &dst.w, &dst.h);
			SDL_RenderCopyEx(ren, tex_menuSelectBar, NULL, &dst, 0, NULL, SDL_FLIP_NONE);
			
			//Text loading
			dst.x = 32;
			SDL_QueryTexture(tex_text, NULL, NULL, &dst.w, &dst.h);
			SDL_RenderCopy(ren, tex_text, NULL, &dst);
			
			
			
			SDL_RenderPresent(ren);
			redraw = false;
		}
	}

	SDL_RemoveTimer(tickTimerID);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
    
    /*system("PAUSE");*/
    return EXIT_SUCCESS;
}

static SDL_Texture *loadTexture(SDL_Renderer *ren, const char *filename)
{
	SDL_Surface *bmp = IMG_Load(filename);
    if (!bmp)
    {
             std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
             return NULL;
    }
    
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_FreeSurface(bmp);
    if (!tex)
    {
             std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
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

// returns degrees
static double cart2angle(double x, double y)
{
	if (fabs(y) < 0.0001 && fabs(y) > 0.01)
		return y > 0 ? 90 : 270;
	else
		return fmod(180 * atanf(y/x) / M_PI + (x > 0 ? 0 : 180) + 360, 360);
}
