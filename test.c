// :nmap <buffer> <Leader>a :call MunitTests()<CR>

#include "munit.h"
#include "checkers.c"

#define test(name) \
  MunitResult test_##name(const MunitParameter p[], void *data)

//
// Valid location and live squares
//
test(is_location_valid) {
  munit_assert_true(is_location_valid(0, 0));
  munit_assert_true(is_location_valid(BOARD_WIDTH-1, BOARD_HEIGHT-1));
  munit_assert_false(is_location_valid(-1, 0));
  munit_assert_false(is_location_valid(0, -1));
  munit_assert_false(is_location_valid(BOARD_WIDTH+1, 0));
  munit_assert_false(is_location_valid(0, BOARD_HEIGHT+1));
  return MUNIT_OK;
}

test(is_location_live) {
  munit_assert_true(is_location_live(1,0));
  munit_assert_false(is_location_live(0,0));
  munit_assert_true(is_location_live(0,1));
  munit_assert_false(is_location_live(BOARD_WIDTH-1,BOARD_HEIGHT-1));
  munit_assert_false(is_location_live(-1,0));
  munit_assert_false(is_location_live(0,-1));
  return MUNIT_OK;
}

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
  munit_assert_false(set_piece(-1,1,'!'));
  return MUNIT_OK;
}

test(set_piece_x_off_board) {
  munit_assert_false(set_piece(BOARD_WIDTH,1,'!'));
  return MUNIT_OK;
}

test(set_piece_negative_y) {
  munit_assert_false(set_piece(1,-1,'!'));
  return MUNIT_OK;
}

test(set_piece_y_off_board) {
  munit_assert_false(set_piece(0,BOARD_HEIGHT,'!'));
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
  munit_assert_false(move_piece(3,3, 1,1));
  return MUNIT_OK;
}

test(move_piece_src_empty) {
  clear_board();
  munit_assert_false(move_piece(3,3, 1,1));
  return MUNIT_OK;
}


//
// Init board
//
test(clear_board_clear_no_pieces) {
  clear_board();
  int pieces = 0;
  for(int y = 0; y < BOARD_HEIGHT; y++)
    for(int x = 0; x < BOARD_WIDTH; x++)
      pieces += get_piece(x,y) != ' ';
  munit_assert_int(pieces,==,0);
  return MUNIT_OK;
}

test(init_board_24_pieces) {
  init_board();
  int pieces = 0;
  for(int y = 0; y < BOARD_HEIGHT; y++)
    for(int x = 0; x < BOARD_WIDTH; x++)
      pieces += get_piece(x,y) != ' ';
  munit_assert_int(pieces,==,24);
  return MUNIT_OK;
}

test(init_board_middle_clear) {
  init_board();
  int pieces = 0;
  for(int y = 3; y <= 4; y++)
    for(int x = 0; x < BOARD_WIDTH; x++)
      pieces += get_piece(x,y) != ' ';
  munit_assert_int(pieces,==,0);
  return MUNIT_OK;
}

test(init_board_white_top) {
  init_board();
  for(int y = 0; y <= 3; y++) {
    for(int x = 0; x < BOARD_WIDTH; x++) {
      int piece = get_piece(x,y);
      if(piece == ' ') continue;
      munit_assert_char(piece,==,'w');
    }
  }
  return MUNIT_OK;
}

test(init_board_black_bottom) {
  init_board();
  for(int y = 5; y <= 7; y++) {
    for(int x = 0; x < BOARD_WIDTH; x++) {
      int piece = get_piece(x,y);
      if(piece == ' ') continue;
      munit_assert_char(piece,==,'b');
    }
  }
  return MUNIT_OK;
}

test(init_board_only_on_live) {
  init_board();

  for(int y = 0; y < BOARD_HEIGHT; y++) {
    for(int x = 0; x < BOARD_WIDTH; x++) {
      if(is_location_live(x, y)) continue;
      int piece = get_piece(x, y);
      munit_assert_char(piece,==,' ');
    }
  }
  return MUNIT_OK;
}


//
// Move validation
//
test(is_move_valid_off_board) {
  clear_board();
  munit_assert_string_contains(
      is_move_valid(-1,0,0,1,'w'),
      "invalid location");
  munit_assert_string_contains(
      is_move_valid(0,1,-1,2,'w'),
      "invalid location");
  return MUNIT_OK;
}

 test(is_move_valid_dead_square) {
   clear_board();
   set_piece(0,0,'w');
   munit_assert_string_contains(
       is_move_valid(0,0,1,1,'w'),
       "not a dark square");
   return MUNIT_OK;
 }
 
test(is_move_valid_same_color) {
  clear_board();
  set_piece(1,0,'w');
  munit_assert_string_contains(
      is_move_valid(1,0,0,1,'b'),
      "wrong color");
  munit_assert_null(is_move_valid(1,0,0,1,'w'));
  return MUNIT_OK;
}

test(is_move_valid_destination_empty) {
  clear_board();
  set_piece(1,0,'w');
  set_piece(0,1,'1');
  munit_assert_string_contains(
      is_move_valid(1,0,0,1,'w'),
      "not empty");
  return MUNIT_OK;
}

test(is_move_valid_known_piece) {
  clear_board();
  set_piece(1,0,'q');
  munit_assert_string_contains(
      is_move_valid(1,0,0,1,'b'),
      "unknown type");
  return MUNIT_OK;
}

test(is_move_valid_correct_direction) {
  clear_board();
  set_piece(1,0,'w');
  munit_assert_null(is_move_valid(1,0,0,1,'w'));
  return MUNIT_OK;
}

test(is_move_valid_incorrect_direction) {
  clear_board();
  set_piece(1,0,'b');
  munit_assert_string_contains(
      is_move_valid(1,0,0,1,'b'),
      "wrong direction");
  return MUNIT_OK;
}

test(is_move_valid_king_direction) {
  clear_board();
  set_piece(1,0,'B');
  munit_assert_null(is_move_valid(1,0,0,1,'b'));
  return MUNIT_OK;
}

test(is_move_valid_diagonal) {
  clear_board();
  set_piece(1,0,'w');
  munit_assert_string_contains(
      is_move_valid(1,0,6,1,'w'),
      "must be diagonal");
  return MUNIT_OK;
}

test(is_move_valid_must_jump) {
  clear_board();
  set_piece(1,0,'w');
  munit_assert_string_contains(
      is_move_valid(1,0,3,2,'w'),
      "jump piece to move");
  return MUNIT_OK;
}

test(is_move_valid_can_jump) {
  clear_board();
  set_piece(1,0,'w');
  set_piece(2,1,'b');
  munit_assert_null(is_move_valid(1,0,3,2,'w'));
  return MUNIT_OK;
}

test(is_move_valid_cant_jump_own) {
  clear_board();
  set_piece(1,0,'w');
  set_piece(2,1,'w');
  munit_assert_string_contains(
      is_move_valid(1,0,3,2,'w'),
      "own piece");
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
