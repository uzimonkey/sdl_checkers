#include "checkers.h"
#include <stdio.h>
#include <SDL.h>

#define CUTE_PNG_IMPLEMENTATION
#include "cute_png.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define die(msg) do { perror(msg); exit(EXIT_FAILURE); } while(0);

SDL_Window *window;
SDL_Renderer *renderer;

int main(int argc, char *argv[]) {
  if(SDL_Init(SDL_INIT_EVERYTHING))
    die(SDL_GetError());

  if(SDL_CreateWindowAndRenderer(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0,
        &window,
        &renderer)) {
    die(SDL_GetError());
  }

  while(1) {
    for(SDL_Event e; SDL_PollEvent(&e);) {
      if(e.type == SDL_QUIT) goto done;
    }

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }
done:

  SDL_Quit();
}
