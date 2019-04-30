// :nmap <buffer> <Leader>a :call MunitTests()<CR>

#include "munit.h"
#include "checkers.c"

#define test(name) \
  MunitResult test_##name(const MunitParameter p[], void *data)


//
// Set piece
//
test(set_piece) {
  clear_board();
  set_piece(3,3,'!');
  munit_assert_char(board[3][3],==,'!');
  return MUNIT_OK;
}

test(set_piece_negative_x) {
  munit_assert(set_piece(-1,1,'!') == -1);
  return MUNIT_OK;
}

test(set_piece_x_off_board) {
  munit_assert(set_piece(BOARD_WIDTH,1,'!') == -1);
  return MUNIT_OK;
}

test(set_piece_negative_y) {
  munit_assert(set_piece(1,-1,'!') == -1);
  return MUNIT_OK;
}

test(set_piece_y_off_board) {
  munit_assert(set_piece(0,BOARD_HEIGHT,'!') == -1);
  return MUNIT_OK;
}


//
// Get piece
//
test(get_piece) {
  clear_board();
  board[2][2] = '!';
  munit_assert_char(get_piece(2,2),==,'!');
  return MUNIT_OK;
}

test(get_piece_negative_x) {
  munit_assert(get_piece(-1,1) == -1);
  return MUNIT_OK;
}

test(get_piece_x_off_board) {
  munit_assert(get_piece(BOARD_WIDTH,1) == -1);
  return MUNIT_OK;
}

test(get_piece_negative_y) {
  munit_assert(get_piece(1,-1) == -1);
  return MUNIT_OK;
}

test(get_piece_y_off_board) {
  munit_assert(get_piece(0,BOARD_HEIGHT) == -1);
  return MUNIT_OK;
}


//
// Move piece
//
test(move_piece_moves_piece) {
  clear_board();
  board[3][3] = '!';
  move_piece(3,3, 1,1);
  munit_assert_char(board[1][1],==,'!');
  return MUNIT_OK;
}

test(move_piece_removes_piece) {
  clear_board();
  board[3][3] = '!';
  move_piece(3,3, 1,1);
  munit_assert_char(board[3][3],==,' ');
  return MUNIT_OK;
}

test(move_piece_dest_not_empty) {
  clear_board();
  board[3][3] = board[1][1] = '!';
  munit_assert(move_piece(3,3, 1,1) == -1);
  return MUNIT_OK;
}

test(move_piece_src_empty) {
  clear_board();
  munit_assert(move_piece(3,3, 1,1) == -1);
  return MUNIT_OK;
}


#undef test
#define test(t) {#t, test_##t},
#define TESTS_BEGIN
#define TESTS_END

static const MunitSuite board_suite = {
  (char*)"",
  (MunitTest[]){
#include "tests.h"
    {0}
  },
  0,
  1,
  MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char *argv[]) {
  return munit_suite_main(&board_suite, 0, argc, argv);
}
