#include "checkers.h"

// Empty spaces are ' ', occupied spaces are w,W,b,B
static char board[BOARD_WIDTH][BOARD_HEIGHT];


// Return true if the location is valid
bool is_location_valid(int x, int y) {
  return x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT;
}


// Return true if the location is live (dark squares where pieces may move)
bool is_location_live(int x, int y) {
  return is_location_valid(x,y) && ((x%2) != (y%2));
}


// Get piece at board location x, y
// Returns -1 on error
int get_piece(int x, int y) {
  if(!is_location_valid(x,y))
    return -1;

  return board[y][x];
}


// Set piece at board location x, y
// Returns false on error
bool set_piece(int x, int y, char piece) {
  if(!is_location_valid(x,y))
    return false;

  board[y][x] = piece;
  return true;
}


// Move a piece from x1,y1 to x2,y2
// Returns false on error
bool move_piece(int x1, int y1, int x2, int y2) {
  if(!is_location_valid(x1,y1) || !is_location_valid(x2,y2))
    return false;

  int p = get_piece(x1,y1);
  if(p == ' ')
    return false;
  if(get_piece(x2,y2) != ' ')
    return false;

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

