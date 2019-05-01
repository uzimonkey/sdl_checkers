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

#define TARGET_WIDTH ((TILE_WIDTH) * (BOARD_WIDTH+2))
#define TARGET_HEIGHT ((TILE_HEIGHT) * (BOARD_WIDTH+2))

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
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

// SDL stuff
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *render_target;
int render_target_scale;

// Resources
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


// SDL functions
void update_render_target(void) {
  if(render_target != NULL)
    SDL_DestroyTexture(render_target);
  
  int win_width, win_height;
  SDL_GetWindowSize(window, &win_width, &win_height);

  render_target_scale = ceil(min(
        win_width / (double)TARGET_WIDTH,
        win_height / (double)TARGET_HEIGHT));
  if(render_target_scale < 1)
    render_target_scale = 1;

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  render_target = SDL_CreateTexture(
      renderer, 
      SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_TARGET,
      TARGET_WIDTH * render_target_scale,
      TARGET_HEIGHT * render_target_scale);

  if(render_target == NULL)
    die(SDL_GetError());
}


void init_sdl(void) {
  if(SDL_Init(SDL_INIT_EVERYTHING))
    die(SDL_GetError());

  window = SDL_CreateWindow(
      "Checkers",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH, WINDOW_HEIGHT,
      SDL_WINDOW_RESIZABLE);
  if(!window)
    die(SDL_GetError());

  renderer = SDL_CreateRenderer(
      window,
      -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(!renderer)
    die(SDL_GetError());

  update_render_target();
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
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
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


void load_resources(void) {
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


// Checkers functions
void draw_board(void) {
  // Draw board
  SDL_RenderCopy(
      renderer,
      tex_board.tex,
      NULL,
      NULL);

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
            .x=((x+1) * TILE_WIDTH) * render_target_scale,
            .y=((y+1) * TILE_HEIGHT) * render_target_scale,
            .w=tex->w * render_target_scale,
            .h=tex->h * render_target_scale
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
      switch(e.type) {
      case SDL_QUIT:
        goto done;

      case SDL_WINDOWEVENT:
        if(e.window.windowID != SDL_GetWindowID(window))
          break;
        switch(e.window.event) {
        case SDL_WINDOWEVENT_CLOSE:
          goto done;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          update_render_target();
          break;
        }
        break;
      }
    }

    SDL_SetRenderTarget(renderer, render_target);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    draw_board();
    draw_string(&font, 0, 0, "Hello, world!");

    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    double scale = min(
        (double)window_width/(TARGET_WIDTH * render_target_scale),
        (double)window_height/(TARGET_HEIGHT * render_target_scale));

    SDL_RenderCopy(
      renderer,
      render_target,
      NULL,
      &(SDL_Rect){
        .x=(window_width - TARGET_WIDTH*render_target_scale*scale)/2,
        .y=(window_height - TARGET_HEIGHT*render_target_scale*scale)/2,
        .w=TARGET_WIDTH*render_target_scale*scale,
        .h=TARGET_HEIGHT*render_target_scale*scale
      });
    SDL_RenderPresent(renderer);
  }
done:

  SDL_Quit();
}
