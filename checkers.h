#ifndef CHECKERS_H
#define CHECKERS_H
#include <stdbool.h>

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8

int get_piece(int x, int y);
bool set_piece(int x, int y, char piece);
bool move_piece(int x1, int y1, int x2, int y2);
void init_board();

#endif
