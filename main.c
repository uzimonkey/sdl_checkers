#include "checkers.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  init_board();

  for(int y = 0; y < BOARD_HEIGHT; y++) {
    for(int x = 0; x < BOARD_WIDTH; x++) {
      printf("%c", get_piece(x, y));
    }
    printf("\n");
  }

  return 0;
}
