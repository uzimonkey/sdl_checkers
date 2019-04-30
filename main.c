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

typedef struct {
  SDL_Texture *tex;
  int w, h;
} Texture;

Texture tex_board;
Texture tex_white, tex_white_king;
Texture tex_black, tex_black_king;

void init_sdl() {
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
}

void load_texture(const char* filename, Texture *tex) {
  cp_image_t img = cp_load_png(filename);
  if(img.pix == 0)
    die(cp_error_reason);

  SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(
      img.pix,
      img.w,
      img.h,
      32,
      img.w*4,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      0xff000000,
      0x00ff0000,
      0x0000ff00,
      0x000000ff
#else
      0x000000ff,
      0x0000ff00,
      0x00ff0000,
      0xff000000
#endif
  );
  
  if(surf == NULL)
    die(SDL_GetError());

  tex->tex = SDL_CreateTextureFromSurface(renderer, surf);
  if(tex->tex == NULL)
    die(SDL_GetError());
  tex->w = surf->w;
  tex->h = surf->h;

  SDL_FreeSurface(surf);
  free(img.pix);
}

void load_resources() {
  load_texture("assets/board.png", &tex_board);
}

int main(int argc, char *argv[]) {
  init_sdl();
  load_resources();

  while(1) {
    for(SDL_Event e; SDL_PollEvent(&e);) {
      if(e.type == SDL_QUIT) goto done;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderCopy(
        renderer,
        tex_board.tex,
        NULL,
        &(SDL_Rect){.x=0, .y=0, .w=tex_board.w, .h=tex_board.h}
    );
    SDL_RenderPresent(renderer);
  }
done:

  SDL_Quit();
}
