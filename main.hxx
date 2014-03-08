#ifndef _TESTSDL_MAIN_HXX
#define _TESTSDL_MAIN_HXX

#include <sstream>
#include <string> 

extern "C" {
#include <SDL.h>
}

//Number to string done  Copyright © 2014 Nathan Phillip Brink <ohnobinki@ohnopublishing.net>
template <class T> static std::string str(const T &o)
{
  std::stringstream sb;
  sb << o;
  return sb.str();
}

SDL_Texture *loadTexture(SDL_Renderer *ren, const char *filename);

#endif /* _TESTSDL_MAIN_HXX */
