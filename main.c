#include "checkers.h"
#include <stdio.h>
#include <stdarg.h>
#include <SDL.h>

#define CUTE_PNG_IMPLEMENTATION
#include "cute_png.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define TILE_WIDTH 32
#define TILE_HEIGHT 32

#define FONT_CHARS ( \
    "!\"#$%&'()*+,-./0123456789:;<=>?@" \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`" \
    "abcdefghijklmnopqrstuvwxyz{|}~" )

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RGBA_MASK 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
#else
#define RGBA_MASK 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#endif

#define die(msg) do { perror(msg); exit(EXIT_FAILURE); } while(0)

SDL_Window *window;
SDL_Renderer *renderer;

typedef struct {
  SDL_Texture *tex;
  int w, h;
} Texture;

Texture tex_board;
Texture tex_white, tex_white_king;
Texture tex_black, tex_black_king;

typedef struct {
  Texture tex;
  SDL_Rect *src_rects;
  const char *charset;
} Font;

Font font;


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


SDL_Surface *load_png(const char *filename) {
  cp_image_t img = cp_load_png(filename);
  if(img.pix == 0)
    die(cp_error_reason);

  SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(
      img.pix, img.w, img.h, 32, img.w*4, RGBA_MASK);
  
  if(surf == NULL)
    die(SDL_GetError());

  return surf;
}


void load_texture_from_surface(SDL_Surface *surf, Texture *tex) {
  tex->tex = SDL_CreateTextureFromSurface(renderer, surf);
  if(tex->tex == NULL)
    die(SDL_GetError());
  tex->w = surf->w;
  tex->h = surf->h;
}


void load_texture(const char *filename, Texture *tex) {
  SDL_Surface *surf = load_png(filename);
  load_texture_from_surface(surf, tex);

  free(surf->pixels);
  SDL_FreeSurface(surf);
}


void load_font(const char *filename, const char *charset, Font *fnt) {
  SDL_Surface *surf = load_png(filename);
  load_texture_from_surface(surf, &fnt->tex);

  fnt->charset = charset;
  fnt->src_rects = malloc(sizeof(SDL_Rect) * strlen(charset));

  for(int left = 0, right = 0, idx = 0;
      idx < strlen(charset) && right < fnt->tex.w;
      left = right, right++, idx++)
  {
    while(((Uint32*)surf->pixels)[right+1] == 0) right++;
    fnt->src_rects[idx] = (SDL_Rect){
      .x = left,
      .y = 1,
      .w = right-left,
      .h = surf->h-1
    };
  }

  free(surf->pixels);
  SDL_FreeSurface(surf);
}


void load_resources() {
  load_texture("assets/board.png", &tex_board);
  load_texture("assets/white.png", &tex_white);
  load_texture("assets/white_king.png", &tex_white_king);
  load_texture("assets/black.png", &tex_black);
  load_texture("assets/black_king.png", &tex_black_king);
  load_font("assets/good_neighbors.png", FONT_CHARS, &font);
}


void draw_string(Font *fnt, int x, int y, const char *fmt, ...) {
  static char *buf = NULL;
  static size_t buf_size = 0;

  if(buf == NULL) {
    buf = malloc(128);
    buf_size = 128;
  }

  va_list args;
  va_start(args, fmt);
  while(vsnprintf(buf, buf_size, fmt, args) == buf_size) {
    buf_size *= 2;
    buf = realloc(buf, buf_size);
  }

  for(char *c = buf; *c; c++) {
    // Spaces are 1 n wide
    if(*c == ' ') {
      char *p = index(fnt->charset, 'n');
      int idx = (int)(p - fnt->charset);
      x += fnt->src_rects[idx].w;
      continue;
    }

    char *p = index(fnt->charset, *c);
    if(p == NULL) {
      *c = '?';
      c--;
      continue;
    }

    int idx = (int)(p - fnt->charset);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderCopy(
        renderer,
        fnt->tex.tex,
        &fnt->src_rects[idx],
        &(SDL_Rect){
          .x=x,
          .y=y,
          .w=fnt->src_rects[idx].w,
          .h=fnt->src_rects[idx].h
        }
    );

    x += fnt->src_rects[idx].w;
  }
}


void draw_board() {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  // Draw board
  SDL_RenderCopy(
      renderer,
      tex_board.tex,
      NULL,
      &(SDL_Rect){
        .x=(WINDOW_WIDTH-tex_board.w)/2,
        .y=(WINDOW_HEIGHT-tex_board.h)/2,
        .w=tex_board.w,
        .h=tex_board.h
      }
  );

  // Top left of the board
  int board_x = (WINDOW_WIDTH-tex_board.w)/2 + TILE_WIDTH;
  int board_y = (WINDOW_HEIGHT-tex_board.h)/2 + TILE_HEIGHT;

  for(int y = 0; y < BOARD_HEIGHT; y++) {
    for(int x = 0; x < BOARD_WIDTH; x++) {
      Texture *tex = NULL;
      switch(get_piece(x,y)) {
        case 'w': tex = &tex_white; break;
        case 'W': tex = &tex_white_king; break;
        case 'b': tex = &tex_black; break;
        case 'B': tex = &tex_black_king; break;
      }

      if(tex == NULL)
        continue;

      SDL_RenderCopy(
          renderer,
          tex->tex,
          NULL,
          &(SDL_Rect){
            .x=board_x + x*TILE_WIDTH,
            .y=board_y + y*TILE_WIDTH,
            .w=tex->w,
            .h=tex->h
          }
      );
    }
  }
}

int main(int argc, char *argv[]) {
  init_sdl();
  load_resources();
  init_board();

  while(1) {
    for(SDL_Event e; SDL_PollEvent(&e);) {
      if(e.type == SDL_QUIT) goto done;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    draw_board();
    draw_string(&font, 0, 0, "Hello, world!");
    SDL_RenderPresent(renderer);
  }
done:

  SDL_Quit();
}
