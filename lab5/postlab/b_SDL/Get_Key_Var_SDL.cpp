/*
    Algorithm
    1. Init ()
        SDL_Init ()
        SDL_CreateWindow ()
        SDL_GetWindowSurface ()
    3. Event loop in main ()
        A. SDL_PollEvent ()
            print key Vars
    9. close ()
        SDL_DestroyWindow ()
        SDL_Quit ()
*/

#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main (int argc, char *argv[])
{
  SDL_Window *window = NULL;
  SDL_Surface *screenSurface = NULL;

  SDL_Event e;
  bool quit = false;

  //Initialize SDL
  if (SDL_Init (SDL_INIT_VIDEO) < 0) {
    printf ("SDL could not initialize! SDL_Error: %s\n", SDL_GetError ());
  }
  else {
    //Create window
    window = SDL_CreateWindow ("SDL_Tutorial", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError ());
    }
    else {
      //Get window surface
      screenSurface = SDL_GetWindowSurface (window);

      //Fill the surface white
      SDL_FillRect (screenSurface, NULL,
                    SDL_MapRGB (screenSurface->format, 0xFF, 0xFF, 0xFF));

      //Update the surface
      SDL_UpdateWindowSurface (window);

      //Event
      while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
          if (e.type == SDL_QUIT) {
            quit = true;
          }
          else if (e.type == SDL_KEYDOWN) {
            printf ("Key scancode %xh, keycode %xh, keymod %xh.\n",
                    e.key.keysym.scancode, e.key.keysym.sym, e.key.keysym.mod);
          }
        }              
      }
    }
  }

  //Destroy window
  SDL_DestroyWindow (window);

  //Quit SDL subsystems
  SDL_Quit ();
  
  return 0;
}
