#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <sys/time.h>
#include <pthread.h>
#include "UDP.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init_SDL();

//Frees media and shuts down SDL
void close_SDL();

//Loads individual image
SDL_Surface* loadSurface( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Current displayed PNG image
SDL_Surface* gJPGSurface = NULL;

//Maximum JPG file size//
const int FRAME_SIZE = 921600;

//frame buffer
uint8_t *frame = NULL;

//UDP socket pointer//
struct UDP_socket *sock_udp;

bool quit = false;

bool init_SDL(void)
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "TMR_Viewer", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Initialize JPG loading
			int imgFlags = IMG_INIT_JPG;
			if( !( IMG_Init( imgFlags ) & imgFlags ) )
			{
				printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				success = false;
			}
			else
			{
				//Get window surface
				gScreenSurface = SDL_GetWindowSurface( gWindow );

        //Fill the surface white
        SDL_FillRect (gScreenSurface, NULL,
            SDL_MapRGB (gScreenSurface->format, 0xFF, 0xFF, 0xFF));

        //Update the surface
        SDL_UpdateWindowSurface (gWindow);
			}
		}
	}

	return success;
}

void close_SDL()
{
	//Free loaded image
	SDL_FreeSurface( gJPGSurface );
	gJPGSurface = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Surface* loadSurface(void *buf, size_t size)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
  SDL_RWops *rw_jpg = SDL_RWFromMem (buf, size);
	SDL_Surface* loadedSurface = IMG_Load_RW(rw_jpg, 1);
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image from buffer! SDL_image Error: %s\n", IMG_GetError() );
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, NULL );
		if( optimizedSurface == NULL )
		{
			printf( "Unable to optimize image! SDL Error: %s\n", SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return optimizedSurface;
}

void *thread_viewer (void *vargs)
{
  int n;
  bool isJPG;

  while (!quit) {
    //recv UDP data//
    n = UDP_recv (sock_udp, frame, FRAME_SIZE);
    
    //check JPG file//
    if (n < 4)
      continue;
    isJPG = true;
    
    if (frame[0] != 0xFF) isJPG = false;
    if (frame[1] != 0xD8) isJPG = false;
    if (frame[n-2] != 0xFF) isJPG = false;
    if (frame[n-1] != 0xD9) isJPG = false;

    if (!isJPG)
      continue;

    printf ("recv: %d bytes\n", n);

    gJPGSurface = loadSurface (frame, n);
    SDL_BlitSurface (gJPGSurface, NULL, gScreenSurface, NULL);
    SDL_FreeSurface (gJPGSurface);
    gJPGSurface = NULL;
    SDL_UpdateWindowSurface (gWindow);
  }

  return NULL;
}

int main (int argc, char *argv[])
{ 
  //Control value//
  int ivx = 0;
  int ivy = 0;
  int iw  = 0;
  uint8_t LL = 0;
  uint8_t RL = 0;

  //Control string//
  char str_ctrl[128];
  int command_id = 0;
  int sec;
  int msec;
  struct timeval set;
  struct timeval now;

  pthread_t tid;

  //SDL Event//
  SDL_Event e;
  char pressed_key;

  //Init UDP socket//
  sock_udp = UDP_new_socket ();
  UDP_socket_init (sock_udp);
  UDP_set_server (sock_udp, 4960);
  printf ("Got a socket with port %d\n", 4960);

  //Init SDL socket//
  init_SDL ();

  //Init frame buffer//
  frame = (uint8_t *) malloc (FRAME_SIZE);

  pthread_create (&tid, NULL, thread_viewer, NULL);

  gettimeofday (&set, NULL);

  //Event//
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
      else if (e.type == SDL_KEYDOWN) { 
        
        bool pressed = true;

        switch (e.key.keysym.scancode) {
          case SDL_SCANCODE_F12:
            printf ("pressed: F12 (Quit)\n");
            UDP_send (sock_udp, (void *)"End", 4);
            pressed = false;
            quit = true;
            break;

          case SDL_SCANCODE_1:
            pressed_key = '1';
            break;

          case SDL_SCANCODE_2:
            pressed_key = '2';
            break;
  
          case SDL_SCANCODE_3:
            pressed_key = '3';
            break;

          case SDL_SCANCODE_Q:
            pressed_key = 'Q';
            LL ^= 0x01;
            break;

          case SDL_SCANCODE_W:
            pressed_key = 'W';
            ivx++;
            if (ivx > 10)
              ivx = 10;
            break;

          case SDL_SCANCODE_E:
            pressed_key = 'E';
            RL ^= 0x01;
            break;

          case SDL_SCANCODE_A:
            pressed_key = 'A';
            ivy--;
            if (ivy < -10)
              ivy = -10;
            break;

          case SDL_SCANCODE_S:
            pressed_key = 'S';
            ivx = 0;
            ivy = 0;
            iw = 0;
            break;

          case SDL_SCANCODE_D:
            pressed_key = 'D';
            ivy++;
            if (ivy > 10)
              ivy = 10;
            break;

          case SDL_SCANCODE_Z:
            pressed_key = 'Z';
            iw++;
            if (iw > 10)
              iw = 10;
            break;

          case SDL_SCANCODE_X:
            pressed_key = 'X';
            ivx--;
            if (ivx < -10)
              ivx = -10;
            break;

          case SDL_SCANCODE_C:
            pressed_key = 'C';
            iw--;
            if (iw < -10)
              iw = -10;
            break;

          default:
            pressed = false;
        }
        
        if (pressed) {
          command_id++;

          gettimeofday(&now, NULL);
          msec = (now.tv_sec * 1e6 + now.tv_usec) - (set.tv_sec * 1e6 + set.tv_usec);
          msec /= 1000;
          sec = msec / 1000;
          msec = msec % 1000;

          sprintf (str_ctrl, "%d %d.%03d %d %d %d %d %d",
                   command_id, sec, msec, ivx * 10, ivy * 10, iw * 10, LL, RL);

          UDP_send (sock_udp, str_ctrl, strlen (str_ctrl) + 1);

          printf ("\t\t\t\t\t%s\n", str_ctrl);
        }
      }
    }
  }
  pthread_join (tid, NULL);

  free (frame);
  frame = NULL;

  close_SDL ();
  UDP_close (sock_udp);

  return 0;
}
