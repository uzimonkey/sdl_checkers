#include "checkers.h"

// Empty spaces are ' ', occupied spaces are w,W,b,B
static char board[BOARD_WIDTH][BOARD_HEIGHT];


// Get piece at board location x, y
// Returns -1 on error
int get_piece(int x, int y) {
  if(x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT)
    return -1;
  return board[y][x];
}


// Set piece at board location x, y
// Returns -1 on error
int set_piece(int x, int y, char piece) {
  if(x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT)
    return -1;

  board[y][x] = piece;
  return 0;
}


// Move a piece from x1,y1 to x2,y2
// Returns -1 on error
int move_piece(int x1, int y1, int x2, int y2) {
  int p = get_piece(x1,y1);
  if(p == ' ' || p == -1) return -1;
  if(get_piece(x2,y2) != ' ') return -1;

  set_piece(x1,y1,' ');
  return set_piece(x2,y2,p);
}


static void place_ranks(int y, char piece) {
  for(int x = 0, p = 0; p < 12; p++) {
    set_piece(x + (y%2 == 0), y, piece);

    x += 2;
    if(x >= BOARD_WIDTH) {
      y++;
      x = 0;
    }
  }
}


// Clears the board
void clear_board() {
  for(int y = 0; y < BOARD_HEIGHT; y++)
    for(int x = 0; x < BOARD_WIDTH; x++)
      set_piece(x, y, ' ');
}


// Initialize the board for a new game
// Clears all spaces and sets initial pieces
void init_board() {
  clear_board();
  place_ranks(0, 'w');
  place_ranks(5, 'b');
}

