#include <SDL.h>
#include <stdio.h>

// screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// starts up SDL and creates window
bool init();
// frees media and shuts down SDL
void close();
// the window we'll be rendering to
SDL_Window* gWindow = NULL;
// the surface contained by the window
SDL_Surface* gScreenSurface = NULL;
// the image we will load and show on the screen
SDL_Surface* gXOut = NULL;

bool init() {
	//Initialization flag
	bool success = true;
	// init() - 1: SDL_Init()
	if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		// init() - 2: SDL_CreateWindow()
		gWindow = SDL_CreateWindow("EE405 - Get_Key_Var_SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			// init() - 3: SDL_GetWindowSurface()
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}
	return success;
}

bool loadMedia() {
	// loading success flag
	bool success = true;
	// load splash image
	gXOut = SDL_LoadBMP("x.bmp");
	if (gXOut == NULL) {
		printf("Unable to load image %s! SDL Error: %s\n", "x.bmp", SDL_GetError() );
		success = false;
	}
	return success;
}

void close() {
    // deallocate surface
	SDL_FreeSurface( gXOut );
	gXOut = NULL;
	// close() - 1: SDL_DestroyWindow()
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	// close() - 2: SDL_Quit()
	SDL_Quit();
}

int main(int argc, char* args[]) {
	// 1. init()
	if (!init()) printf("Failed to initialize!\n");
	else {
        // load media
		if (!loadMedia()) printf("Failed to load media!\n");
        else {			
            // flag to check if main loop will be exited
            bool quit = false;
            // event handler
            SDL_Event e;
            // main loop... exit only when 'quit' given by user
            while (!quit) {
                // handle events on queue
                while (SDL_PollEvent(&e) != 0) {
                    // user requests quit
                    if (e.type == SDL_QUIT) quit = true;
                    else if (e.type == SDL_KEYDOWN) {
                        // print SDL_Scancode, SDL_Keycode, and SDL_Keymod in hexadecimal for each key pressed
                        printf("Key scancode %xh, keybode %xh, keymod %xh.\n",
                            e.key.keysym.scancode, e.key.keysym.sym, e.key.keysym.mod);
                    }
                }
                // apply the image
				SDL_BlitSurface(gXOut, NULL, gScreenSurface, NULL);
                // update surface
                SDL_UpdateWindowSurface(gWindow);
            }
        }
    }
	// free resources and close SDL
	close();
	return 0;
}
