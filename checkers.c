#include "checkers.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

int sign(int x) {
  return (x > 0) - (x < 0);
}

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


// Move a piece from x1,y1 to x2,y2 according the to the rules
// of checkers. If a piece jumps another piece, that piece is
// removed from the game. If the move lands the piece on the
// enemy's back row, that piece is promoted.
//
// This makes no attempt to validate the move for other rules,
// it's assumed that the move is validated with is_move_valid
// first.
//
// Returns the number of -1 on error, 0 on success, 1 if
// piece captured
int move_piece(int x1, int y1, int x2, int y2) {
  // Bounds checking
  if(!is_location_valid(x1,y1) || !is_location_valid(x2,y2))
    return -1;

  int p = get_piece(x1,y1);

  // Promote
  if(p == 'b' && y2 == 0)
    p = 'B';
  else if(p == 'w' && y2 == BOARD_HEIGHT-1)
    p = 'W';

  // Move and capture
  set_piece(x2,y2,p);
  set_piece(x1,y1,' ');
  if(abs(x2-x1) == 2) {
    set_piece(x1+sign(x2-x1), y1+sign(y2-y1), ' ');
    return 1;
  }

  return 0;
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


// Returns NULL if the move from x1,y1 to x2,y2 is valid for player p
// p is a char representing the normal piece representation for that player,
// 'b' or 'w'
// If invalid, returns const char * string to reason why it's invalid
// This pointer is to an internal buffer that is overwritten on the
// next call to is_move_valid.

#define BUF 256
const char *is_move_valid(int x1, int y1, int x2, int y2, char p) {
  static char str[BUF] = {0};

  // Both must be valid locations
  if(!is_location_valid(x1, y1)) {
    snprintf(str, BUF, "%d,%d is an invalid location", x1, y1);
    return str;
  }

  if(!is_location_valid(x2, y2)) {
    snprintf(str, BUF, "%d,%d is an invalid location", x2, y2);
    return str;
  }

  // Both must be live locations
  if(!is_location_live(x1, y1)) {
    snprintf(str, BUF, "%d,%d is not a dark square", x1, y1);
    return str;
  }

  if(!is_location_live(x2, y2)) {
    snprintf(str, BUF, "%d,%d is not a dark square", x2, y2);
    return str;
  }

  int piece = get_piece(x1, y1);
  bool is_king = isupper(piece);
  piece = tolower(piece);
  int distance = abs(x2-x1);

  // It must be a known piece
  if(piece != 'w' && piece != 'b') {
    snprintf(str, BUF, "Piece at %d,%d is an unknown type (%c)", x1, y1, piece);
    return str;
  }

  // There must be a piece of the same color at location 1
  if(tolower(piece) != p) {
    snprintf(str, BUF, "Piece at %d,%d is the wrong color (%c)", x1, y1, piece);
    return str;
  }

  int ydir;
  if(piece == 'w')
    ydir = 1;
  else
    ydir = -1;

  // There must be no piece at location 2
  if(get_piece(x2, y2) != ' ') {
    snprintf(str, BUF, "Destination %d,%d is not empty", x2, y2);
    return str;
  }

  // It must move in the correct direction
  if(!is_king && sign(y2-y1) != ydir) {
    snprintf(str, BUF, "Move is wrong direction");
    return str;
  }

  // Move must be diagonal
  if(abs(x2-x1) != abs(y2-y1)) {
    snprintf(str, BUF, "Move must be diagonal");
    return str;
  }

  // Jump piece
  if(distance == 2) {
    int jumped_piece = get_piece(x1+sign(x2-x1), y1+sign(y2-y1));
    if(jumped_piece == ' ') {
      snprintf(str, BUF, "Must jump piece to move 2 squares");
      return str;
    }

    jumped_piece = tolower(jumped_piece);
    if(jumped_piece == p) {
      snprintf(str, BUF, "Cannot jump your own piece");
      return str;
    }
  }

  str[0] = 0;
  return NULL;
}
