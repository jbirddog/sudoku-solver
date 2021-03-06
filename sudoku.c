#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// https://norvig.com/sudoku.html
// https://github.com/norvig/pytudes/blob/master/py/sudoku.py

static void print_grid(uint_fast16_t grid[81]);

static void run_tests();

static void profile_example_puzzles();

/*

00 01 02 | 03 04 05 | 06 07 08
09 10 11 | 12 13 14 | 15 16 17
18 19 20 | 21 22 23 | 24 25 26
------------------------------
27 28 29 | 30 31 32 | 33 34 35
36 37 38 | 39 40 41 | 42 43 44
45 46 47 | 48 49 50 | 51 52 53
------------------------------
54 55 56 | 57 58 59 | 60 61 62
63 64 65 | 66 67 68 | 69 70 71
72 73 74 | 75 76 77 | 78 79 80

*/

#define NUMBER_OF_SQUARES 81
#define NUMBER_OF_PEERS 20

static const uint_fast8_t PEERS[][NUMBER_OF_PEERS] =
  {
   {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 18, 19, 20, 27, 36, 45, 54, 63, 72 }, 
   {  0,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 18, 19, 20, 28, 37, 46, 55, 64, 73 }, 
   {  0,  1,  3,  4,  5,  6,  7,  8,  9, 10, 11, 18, 19, 20, 29, 38, 47, 56, 65, 74 }, 
   {  0,  1,  2,  4,  5,  6,  7,  8, 12, 13, 14, 21, 22, 23, 30, 39, 48, 57, 66, 75 }, 
   {  0,  1,  2,  3,  5,  6,  7,  8, 12, 13, 14, 21, 22, 23, 31, 40, 49, 58, 67, 76 }, 
   {  0,  1,  2,  3,  4,  6,  7,  8, 12, 13, 14, 21, 22, 23, 32, 41, 50, 59, 68, 77 }, 
   {  0,  1,  2,  3,  4,  5,  7,  8, 15, 16, 17, 24, 25, 26, 33, 42, 51, 60, 69, 78 }, 
   {  0,  1,  2,  3,  4,  5,  6,  8, 15, 16, 17, 24, 25, 26, 34, 43, 52, 61, 70, 79 }, 
   {  0,  1,  2,  3,  4,  5,  6,  7, 15, 16, 17, 24, 25, 26, 35, 44, 53, 62, 71, 80 }, 
   {  0,  1,  2, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 27, 36, 45, 54, 63, 72 }, 
   {  0,  1,  2,  9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 28, 37, 46, 55, 64, 73 }, 
   {  0,  1,  2,  9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 20, 29, 38, 47, 56, 65, 74 }, 
   {  3,  4,  5,  9, 10, 11, 13, 14, 15, 16, 17, 21, 22, 23, 30, 39, 48, 57, 66, 75 }, 
   {  3,  4,  5,  9, 10, 11, 12, 14, 15, 16, 17, 21, 22, 23, 31, 40, 49, 58, 67, 76 }, 
   {  3,  4,  5,  9, 10, 11, 12, 13, 15, 16, 17, 21, 22, 23, 32, 41, 50, 59, 68, 77 }, 
   {  6,  7,  8,  9, 10, 11, 12, 13, 14, 16, 17, 24, 25, 26, 33, 42, 51, 60, 69, 78 }, 
   {  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 17, 24, 25, 26, 34, 43, 52, 61, 70, 79 }, 
   {  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 24, 25, 26, 35, 44, 53, 62, 71, 80 }, 
   {  0,  1,  2,  9, 10, 11, 19, 20, 21, 22, 23, 24, 25, 26, 27, 36, 45, 54, 63, 72 }, 
   {  0,  1,  2,  9, 10, 11, 18, 20, 21, 22, 23, 24, 25, 26, 28, 37, 46, 55, 64, 73 }, 
   {  0,  1,  2,  9, 10, 11, 18, 19, 21, 22, 23, 24, 25, 26, 29, 38, 47, 56, 65, 74 }, 
   {  3,  4,  5, 12, 13, 14, 18, 19, 20, 22, 23, 24, 25, 26, 30, 39, 48, 57, 66, 75 }, 
   {  3,  4,  5, 12, 13, 14, 18, 19, 20, 21, 23, 24, 25, 26, 31, 40, 49, 58, 67, 76 }, 
   {  3,  4,  5, 12, 13, 14, 18, 19, 20, 21, 22, 24, 25, 26, 32, 41, 50, 59, 68, 77 }, 
   {  6,  7,  8, 15, 16, 17, 18, 19, 20, 21, 22, 23, 25, 26, 33, 42, 51, 60, 69, 78 }, 
   {  6,  7,  8, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 26, 34, 43, 52, 61, 70, 79 }, 
   {  6,  7,  8, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 35, 44, 53, 62, 71, 80 }, 
   {  0,  9, 18, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 54, 63, 72 }, 
   {  1, 10, 19, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 55, 64, 73 }, 
   {  2, 11, 20, 27, 28, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 56, 65, 74 }, 
   {  3, 12, 21, 27, 28, 29, 31, 32, 33, 34, 35, 39, 40, 41, 48, 49, 50, 57, 66, 75 }, 
   {  4, 13, 22, 27, 28, 29, 30, 32, 33, 34, 35, 39, 40, 41, 48, 49, 50, 58, 67, 76 }, 
   {  5, 14, 23, 27, 28, 29, 30, 31, 33, 34, 35, 39, 40, 41, 48, 49, 50, 59, 68, 77 }, 
   {  6, 15, 24, 27, 28, 29, 30, 31, 32, 34, 35, 42, 43, 44, 51, 52, 53, 60, 69, 78 }, 
   {  7, 16, 25, 27, 28, 29, 30, 31, 32, 33, 35, 42, 43, 44, 51, 52, 53, 61, 70, 79 }, 
   {  8, 17, 26, 27, 28, 29, 30, 31, 32, 33, 34, 42, 43, 44, 51, 52, 53, 62, 71, 80 }, 
   {  0,  9, 18, 27, 28, 29, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 54, 63, 72 }, 
   {  1, 10, 19, 27, 28, 29, 36, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 55, 64, 73 }, 
   {  2, 11, 20, 27, 28, 29, 36, 37, 39, 40, 41, 42, 43, 44, 45, 46, 47, 56, 65, 74 }, 
   {  3, 12, 21, 30, 31, 32, 36, 37, 38, 40, 41, 42, 43, 44, 48, 49, 50, 57, 66, 75 }, 
   {  4, 13, 22, 30, 31, 32, 36, 37, 38, 39, 41, 42, 43, 44, 48, 49, 50, 58, 67, 76 }, 
   {  5, 14, 23, 30, 31, 32, 36, 37, 38, 39, 40, 42, 43, 44, 48, 49, 50, 59, 68, 77 }, 
   {  6, 15, 24, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44, 51, 52, 53, 60, 69, 78 }, 
   {  7, 16, 25, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 44, 51, 52, 53, 61, 70, 79 }, 
   {  8, 17, 26, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 51, 52, 53, 62, 71, 80 }, 
   {  0,  9, 18, 27, 28, 29, 36, 37, 38, 46, 47, 48, 49, 50, 51, 52, 53, 54, 63, 72 }, 
   {  1, 10, 19, 27, 28, 29, 36, 37, 38, 45, 47, 48, 49, 50, 51, 52, 53, 55, 64, 73 }, 
   {  2, 11, 20, 27, 28, 29, 36, 37, 38, 45, 46, 48, 49, 50, 51, 52, 53, 56, 65, 74 }, 
   {  3, 12, 21, 30, 31, 32, 39, 40, 41, 45, 46, 47, 49, 50, 51, 52, 53, 57, 66, 75 }, 
   {  4, 13, 22, 30, 31, 32, 39, 40, 41, 45, 46, 47, 48, 50, 51, 52, 53, 58, 67, 76 }, 
   {  5, 14, 23, 30, 31, 32, 39, 40, 41, 45, 46, 47, 48, 49, 51, 52, 53, 59, 68, 77 }, 
   {  6, 15, 24, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 52, 53, 60, 69, 78 }, 
   {  7, 16, 25, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 53, 61, 70, 79 }, 
   {  8, 17, 26, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 62, 71, 80 }, 
   {  0,  9, 18, 27, 36, 45, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 }, 
   {  1, 10, 19, 28, 37, 46, 54, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 }, 
   {  2, 11, 20, 29, 38, 47, 54, 55, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 }, 
   {  3, 12, 21, 30, 39, 48, 54, 55, 56, 58, 59, 60, 61, 62, 66, 67, 68, 75, 76, 77 }, 
   {  4, 13, 22, 31, 40, 49, 54, 55, 56, 57, 59, 60, 61, 62, 66, 67, 68, 75, 76, 77 }, 
   {  5, 14, 23, 32, 41, 50, 54, 55, 56, 57, 58, 60, 61, 62, 66, 67, 68, 75, 76, 77 }, 
   {  6, 15, 24, 33, 42, 51, 54, 55, 56, 57, 58, 59, 61, 62, 69, 70, 71, 78, 79, 80 }, 
   {  7, 16, 25, 34, 43, 52, 54, 55, 56, 57, 58, 59, 60, 62, 69, 70, 71, 78, 79, 80 }, 
   {  8, 17, 26, 35, 44, 53, 54, 55, 56, 57, 58, 59, 60, 61, 69, 70, 71, 78, 79, 80 }, 
   {  0,  9, 18, 27, 36, 45, 54, 55, 56, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 }, 
   {  1, 10, 19, 28, 37, 46, 54, 55, 56, 63, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 }, 
   {  2, 11, 20, 29, 38, 47, 54, 55, 56, 63, 64, 66, 67, 68, 69, 70, 71, 72, 73, 74 }, 
   {  3, 12, 21, 30, 39, 48, 57, 58, 59, 63, 64, 65, 67, 68, 69, 70, 71, 75, 76, 77 }, 
   {  4, 13, 22, 31, 40, 49, 57, 58, 59, 63, 64, 65, 66, 68, 69, 70, 71, 75, 76, 77 }, 
   {  5, 14, 23, 32, 41, 50, 57, 58, 59, 63, 64, 65, 66, 67, 69, 70, 71, 75, 76, 77 }, 
   {  6, 15, 24, 33, 42, 51, 60, 61, 62, 63, 64, 65, 66, 67, 68, 70, 71, 78, 79, 80 }, 
   {  7, 16, 25, 34, 43, 52, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 71, 78, 79, 80 }, 
   {  8, 17, 26, 35, 44, 53, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 78, 79, 80 }, 
   {  0,  9, 18, 27, 36, 45, 54, 55, 56, 63, 64, 65, 73, 74, 75, 76, 77, 78, 79, 80 }, 
   {  1, 10, 19, 28, 37, 46, 54, 55, 56, 63, 64, 65, 72, 74, 75, 76, 77, 78, 79, 80 }, 
   {  2, 11, 20, 29, 38, 47, 54, 55, 56, 63, 64, 65, 72, 73, 75, 76, 77, 78, 79, 80 }, 
   {  3, 12, 21, 30, 39, 48, 57, 58, 59, 66, 67, 68, 72, 73, 74, 76, 77, 78, 79, 80 }, 
   {  4, 13, 22, 31, 40, 49, 57, 58, 59, 66, 67, 68, 72, 73, 74, 75, 77, 78, 79, 80 }, 
   {  5, 14, 23, 32, 41, 50, 57, 58, 59, 66, 67, 68, 72, 73, 74, 75, 76, 78, 79, 80 }, 
   {  6, 15, 24, 33, 42, 51, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 79, 80 }, 
   {  7, 16, 25, 34, 43, 52, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 80 }, 
   {  8, 17, 26, 35, 44, 53, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79 }, 
  };

static inline void default_grid_values(uint_fast16_t grid[81]) {
  for (int i = 0; i < NUMBER_OF_SQUARES; ++i) {
    grid[i] = 0x1FF;
  }
}

static _Bool eliminate_from_peers(uint_fast16_t grid[81], int square, uint_fast16_t value) {
  for (int i = 0; i < NUMBER_OF_PEERS; ++i) {
    uint_fast16_t grid_value = grid[PEERS[square][i]];

    if ((grid_value & value) == 0) {
      continue;
    }
    
    grid_value &= ~value;
    
    if (grid_value == 0) {
      return false;
    }

    grid[PEERS[square][i]] = grid_value;

    if ((grid_value & (grid_value - 1)) == 0 && !eliminate_from_peers(grid, PEERS[square][i], grid_value)) {
      return false;
    }
  }
  
  return true;
}

static _Bool assign(uint_fast16_t grid[81], int square, uint_fast16_t value) {
  grid[square] = value;
  
  return eliminate_from_peers(grid, square, value);
}

static _Bool search_target(const uint_fast16_t grid[81], int *square) {
  static const int none_found = 10;
  int min_remaining = none_found;
  int min_square = 0;

  for (int i = 0; i < NUMBER_OF_SQUARES; ++i) {
    const int values_remaining = __builtin_popcount(grid[i]);

    if (values_remaining > 1 && values_remaining < min_remaining) {
      min_remaining = values_remaining;
      min_square = i;

      if (values_remaining == 2) {
	break;
      }
    }
  }

  if (min_remaining == none_found) {
    return false;
  }

  *square = min_square;

  return true;
}

static inline void copy_grid(const uint_fast16_t src[81], uint_fast16_t dest[81]) {
  for (int i = 0; i < NUMBER_OF_SQUARES; ++i) {
    dest[i] = src[i];
  }
}

static _Bool search(uint_fast16_t grid[81]) {
  int square = 0;

  if (!search_target(grid, &square)) {
    return true;
  }

  const uint_fast16_t values = grid[square];

  for (int i = 0; i < 9; ++i) {
    const uint_fast16_t value = 1 << i;

    if ((values & value) == 0) {
      continue;
    }

    uint_fast16_t new_grid[NUMBER_OF_SQUARES] = {0};
    copy_grid(grid, new_grid);

    if (assign(new_grid, square, value) && search(new_grid)) {
      return true;
    }
  }

  return false;
}

static _Bool solve(const char puzzle[82], uint_fast16_t grid[81]) {
  for (int i = 0; i < NUMBER_OF_SQUARES; ++i) {
    const char value = puzzle[i];
    
    if (value == '.' || value == '0') {
      continue;
    }
    
    if (value >= '1' && value <= '9') {
      const uint_fast16_t value_to_assign = 1 << (value - 49);

      if (!assign(grid, i, value_to_assign)) {
	return false;
      }
    }
    else {
      fprintf(stderr, "Invalid input: %c\n", value);
      return false;
    }
  }
  
  return search(grid);
}

int main(int argc, char **argv) {
  run_tests();

  /*
  //const char puzzle[] = "003020600900305001001806400008102900700000008006708200002609500800203009005010300";
  //const char puzzle[] = "4.....8.5.3..........7......2.....6.....8.4......1.......6.3.7.5..2.....1.4......";
  const char puzzle[] = "..1..6.9.5.....1.....3..4.6..9..7....4.6.3.7....2..9..4.6..9.....8.....5.3.4..8..";

  uint_fast16_t grid[81] = {0};
  default_grid_values(grid);

  (void)solve(puzzle, grid);
  */

  profile_example_puzzles();

  return 0;
}


static void print_grid(uint_fast16_t grid[81]) {
  for (int i = 0; i < NUMBER_OF_SQUARES; ++i) {
    const uint_fast16_t values = grid[i];

    for (int j = 0; j < 9; ++j) {
      if ((values & 1 << j) == 0) {
	continue;
      }

      printf("%d", j + 1);
    }

    printf(" ");

    if ((i + 1) % 9 == 0) {
      printf("\n");
    }
  }
}


/*

  Logic Tests

 */

static void test_can_eliminate_value_from_peers() {
  uint_fast16_t grid[81] = {0};
  default_grid_values(grid);

  const uint_fast16_t known_value = 0x08;
  const int square = 0;

  grid[square] = known_value;

  assert(eliminate_from_peers(grid, square, known_value));
  assert(grid[square] == known_value);

  for (int i = 1; i < NUMBER_OF_SQUARES; ++i) {
    if ((i > 0 && i < 12) || (i > 17 && i < 21) ||
	i == 27 || i == 36 || i == 45 ||
	i == 54 || i == 63 || i == 72) {
      assert(grid[i] == 0x1F7);
    }
    else {
      assert(grid[i] == 0x1FF);
    }
  }
}

static void test_eliminate_only_modifies_peers_with_values_to_remove() {
  uint_fast16_t grid[81] = {0};
  default_grid_values(grid);

  const uint_fast16_t known_value = 0x01;
  const uint_fast16_t peer_value = 0x02;
  const int square = 0;
  const int peer = 4;

  grid[square] = known_value;
  grid[peer] = peer_value;

  assert(eliminate_from_peers(grid, square, known_value));
  assert(grid[square] == known_value);
  assert(grid[peer] == peer_value);
}


static const char *EASY_50_PUZZLES[] =
  {
   "003020600900305001001806400008102900700000008006708200002609500800203009005010300",
   "200080300060070084030500209000105408000000000402706000301007040720040060004010003",
   "000000907000420180000705026100904000050000040000507009920108000034059000507000000",
   "030050040008010500460000012070502080000603000040109030250000098001020600080060020",
   "020810740700003100090002805009040087400208003160030200302700060005600008076051090",
   "100920000524010000000000070050008102000000000402700090060000000000030945000071006",
   "043080250600000000000001094900004070000608000010200003820500000000000005034090710",
   "480006902002008001900370060840010200003704100001060049020085007700900600609200018",
   "000900002050123400030000160908000000070000090000000205091000050007439020400007000",
   "001900003900700160030005007050000009004302600200000070600100030042007006500006800",
   "000125400008400000420800000030000095060902010510000060000003049000007200001298000",
   "062340750100005600570000040000094800400000006005830000030000091006400007059083260",
   "300000000005009000200504000020000700160000058704310600000890100000067080000005437",
   "630000000000500008005674000000020000003401020000000345000007004080300902947100080",
   "000020040008035000000070602031046970200000000000501203049000730000000010800004000",
   "361025900080960010400000057008000471000603000259000800740000005020018060005470329",
   "050807020600010090702540006070020301504000908103080070900076205060090003080103040",
   "080005000000003457000070809060400903007010500408007020901020000842300000000100080",
   "003502900000040000106000305900251008070408030800763001308000104000020000005104800",
   "000000000009805100051907420290401065000000000140508093026709580005103600000000000",
   "020030090000907000900208005004806500607000208003102900800605007000309000030020050",
   "005000006070009020000500107804150000000803000000092805907006000030400010200000600",
   "040000050001943600009000300600050002103000506800020007005000200002436700030000040",
   "004000000000030002390700080400009001209801307600200008010008053900040000000000800",
   "360020089000361000000000000803000602400603007607000108000000000000418000970030014",
   "500400060009000800640020000000001008208000501700500000000090084003000600060003002",
   "007256400400000005010030060000508000008060200000107000030070090200000004006312700",
   "000000000079050180800000007007306800450708096003502700700000005016030420000000000",
   "030000080009000500007509200700105008020090030900402001004207100002000800070000090",
   "200170603050000100000006079000040700000801000009050000310400000005000060906037002",
   "000000080800701040040020030374000900000030000005000321010060050050802006080000000",
   "000000085000210009960080100500800016000000000890006007009070052300054000480000000",
   "608070502050608070002000300500090006040302050800050003005000200010704090409060701",
   "050010040107000602000905000208030501040070020901080406000401000304000709020060010",
   "053000790009753400100000002090080010000907000080030070500000003007641200061000940",
   "006080300049070250000405000600317004007000800100826009000702000075040190003090600",
   "005080700700204005320000084060105040008000500070803010450000091600508007003010600",
   "000900800128006400070800060800430007500000009600079008090004010003600284001007000",
   "000080000270000054095000810009806400020403060006905100017000620460000038000090000",
   "000602000400050001085010620038206710000000000019407350026040530900020007000809000",
   "000900002050123400030000160908000000070000090000000205091000050007439020400007000",
   "380000000000400785009020300060090000800302009000040070001070500495006000000000092",
   "000158000002060800030000040027030510000000000046080790050000080004070100000325000",
   "010500200900001000002008030500030007008000500600080004040100700000700006003004050",
   "080000040000469000400000007005904600070608030008502100900000005000781000060000010",
   "904200007010000000000706500000800090020904060040002000001607000000000030300005702",
   "000700800006000031040002000024070000010030080000060290000800070860000500002006000",
   "001007090590080001030000080000005800050060020004100000080000030100020079020700400",
   "000003017015009008060000000100007000009000200000500004000000020500600340340200000",
   "300200000000107000706030500070009080900020004010800050009040301000702000000008006",
  };

static const int EASY_50_COUNT = sizeof(EASY_50_PUZZLES) / sizeof(EASY_50_PUZZLES[0]);

static const char *HARDEST_11_PUZZLES[] =
  {
   "85...24..72......9..4.........1.7..23.5...9...4...........8..7..17..........36.4.",
   "..53.....8......2..7..1.5..4....53...1..7...6..32...8..6.5....9..4....3......97..",
   "12..4......5.69.1...9...5.........7.7...52.9..3......2.9.6...5.4..9..8.1..3...9.4",
   "...57..3.1......2.7...234......8...4..7..4...49....6.5.42...3.....7..9....18.....",
   "7..1523........92....3.....1....47.8.......6............9...5.6.4.9.7...8....6.1.",
   "1....7.9..3..2...8..96..5....53..9...1..8...26....4...3......1..4......7..7...3..",
   "1...34.8....8..5....4.6..21.18......3..1.2..6......81.52..7.9....6..9....9.64...2",
   "...92......68.3...19..7...623..4.1....1...7....8.3..297...8..91...5.72......64...",
   ".6.5.4.3.1...9...8.........9...5...6.4.6.2.7.7...4...5.........4...8...1.5.2.3.4.",
   "7.....4...2..7..8...3..8.799..5..3...6..2..9...1.97..6...3..9...3..4..6...9..1.35",
   "....7..2.8.......6.1.2.5...9.54....8.........3....85.1...3.2.8.4.......9.7..6....",
  };

static const int HARDEST_11_COUNT = sizeof(HARDEST_11_PUZZLES) / sizeof(HARDEST_11_PUZZLES[0]);

static const char *TOP_95_PUZZLES[] =
  {
   "4.....8.5.3..........7......2.....6.....8.4......1.......6.3.7.5..2.....1.4......",
   "52...6.........7.13...........4..8..6......5...........418.........3..2...87.....",
   "6.....8.3.4.7.................5.4.7.3..2.....1.6.......2.....5.....8.6......1....",
   "48.3............71.2.......7.5....6....2..8.............1.76...3.....4......5....",
   "....14....3....2...7..........9...3.6.1.............8.2.....1.4....5.6.....7.8...",
   "......52..8.4......3...9...5.1...6..2..7........3.....6...1..........7.4.......3.",
   "6.2.5.........3.4..........43...8....1....2........7..5..27...........81...6.....",
   ".524.........7.1..............8.2...3.....6...9.5.....1.6.3...........897........",
   "6.2.5.........4.3..........43...8....1....2........7..5..27...........81...6.....",
   ".923.........8.1...........1.7.4...........658.........6.5.2...4.....7.....9.....",
   "6..3.2....5.....1..........7.26............543.........8.15........4.2........7..",
   ".6.5.1.9.1...9..539....7....4.8...7.......5.8.817.5.3.....5.2............76..8...",
   "..5...987.4..5...1..7......2...48....9.1.....6..2.....3..6..2.......9.7.......5..",
   "3.6.7...........518.........1.4.5...7.....6.....2......2.....4.....8.3.....5.....",
   "1.....3.8.7.4..............2.3.1...........958.........5.6...7.....8.2...4.......",
   "6..3.2....4.....1..........7.26............543.........8.15........4.2........7..",
   "....3..9....2....1.5.9..............1.2.8.4.6.8.5...2..75......4.1..6..3.....4.6.",
   "45.....3....8.1....9...........5..9.2..7.....8.........1..4..........7.2...6..8..",
   ".237....68...6.59.9.....7......4.97.3.7.96..2.........5..47.........2....8.......",
   "..84...3....3.....9....157479...8........7..514.....2...9.6...2.5....4......9..56",
   ".98.1....2......6.............3.2.5..84.........6.........4.8.93..5...........1..",
   "..247..58..............1.4.....2...9528.9.4....9...1.........3.3....75..685..2...",
   "4.....8.5.3..........7......2.....6.....5.4......1.......6.3.7.5..2.....1.9......",
   ".2.3......63.....58.......15....9.3....7........1....8.879..26......6.7...6..7..4",
   "1.....7.9.4...72..8.........7..1..6.3.......5.6..4..2.........8..53...7.7.2....46",
   "4.....3.....8.2......7........1...8734.......6........5...6........1.4...82......",
   ".......71.2.8........4.3...7...6..5....2..3..9........6...7.....8....4......5....",
   "6..3.2....4.....8..........7.26............543.........8.15........8.2........7..",
   ".47.8...1............6..7..6....357......5....1..6....28..4.....9.1...4.....2.69.",
   "......8.17..2........5.6......7...5..1....3...8.......5......2..4..8....6...3....",
   "38.6.......9.......2..3.51......5....3..1..6....4......17.5..8.......9.......7.32",
   "...5...........5.697.....2...48.2...25.1...3..8..3.........4.7..13.5..9..2...31..",
   ".2.......3.5.62..9.68...3...5..........64.8.2..47..9....3.....1.....6...17.43....",
   ".8..4....3......1........2...5...4.69..1..8..2...........3.9....6....5.....2.....",
   "..8.9.1...6.5...2......6....3.1.7.5.........9..4...3...5....2...7...3.8.2..7....4",
   "4.....5.8.3..........7......2.....6.....5.8......1.......6.3.7.5..2.....1.8......",
   "1.....3.8.6.4..............2.3.1...........958.........5.6...7.....8.2...4.......",
   "1....6.8..64..........4...7....9.6...7.4..5..5...7.1...5....32.3....8...4........",
   "249.6...3.3....2..8.......5.....6......2......1..4.82..9.5..7....4.....1.7...3...",
   "...8....9.873...4.6..7.......85..97...........43..75.......3....3...145.4....2..1",
   "...5.1....9....8...6.......4.1..........7..9........3.8.....1.5...2..4.....36....",
   "......8.16..2........7.5......6...2..1....3...8.......2......7..3..8....5...4....",
   ".476...5.8.3.....2.....9......8.5..6...1.....6.24......78...51...6....4..9...4..7",
   ".....7.95.....1...86..2.....2..73..85......6...3..49..3.5...41724................",
   ".4.5.....8...9..3..76.2.....146..........9..7.....36....1..4.5..6......3..71..2..",
   ".834.........7..5...........4.1.8..........27...3.....2.6.5....5.....8........1..",
   "..9.....3.....9...7.....5.6..65..4.....3......28......3..75.6..6...........12.3.8",
   ".26.39......6....19.....7.......4..9.5....2....85.....3..2..9..4....762.........4",
   "2.3.8....8..7...........1...6.5.7...4......3....1............82.5....6...1.......",
   "6..3.2....1.....5..........7.26............843.........8.15........8.2........7..",
   "1.....9...64..1.7..7..4.......3.....3.89..5....7....2.....6.7.9.....4.1....129.3.",
   ".........9......84.623...5....6...453...1...6...9...7....1.....4.5..2....3.8....9",
   ".2....5938..5..46.94..6...8..2.3.....6..8.73.7..2.........4.38..7....6..........5",
   "9.4..5...25.6..1..31......8.7...9...4..26......147....7.......2...3..8.6.4.....9.",
   "...52.....9...3..4......7...1.....4..8..453..6...1...87.2........8....32.4..8..1.",
   "53..2.9...24.3..5...9..........1.827...7.........981.............64....91.2.5.43.",
   "1....786...7..8.1.8..2....9........24...1......9..5...6.8..........5.9.......93.4",
   "....5...11......7..6.....8......4.....9.1.3.....596.2..8..62..7..7......3.5.7.2..",
   ".47.2....8....1....3....9.2.....5...6..81..5.....4.....7....3.4...9...1.4..27.8..",
   "......94.....9...53....5.7..8.4..1..463...........7.8.8..7.....7......28.5.26....",
   ".2......6....41.....78....1......7....37.....6..412....1..74..5..8.5..7......39..",
   "1.....3.8.6.4..............2.3.1...........758.........7.5...6.....8.2...4.......",
   "2....1.9..1..3.7..9..8...2.......85..6.4.........7...3.2.3...6....5.....1.9...2.5",
   "..7..8.....6.2.3...3......9.1..5..6.....1.....7.9....2........4.83..4...26....51.",
   "...36....85.......9.4..8........68.........17..9..45...1.5...6.4....9..2.....3...",
   "34.6.......7.......2..8.57......5....7..1..2....4......36.2..1.......9.......7.82",
   "......4.18..2........6.7......8...6..4....3...1.......6......2..5..1....7...3....",
   ".4..5..67...1...4....2.....1..8..3........2...6...........4..5.3.....8..2........",
   ".......4...2..4..1.7..5..9...3..7....4..6....6..1..8...2....1..85.9...6.....8...3",
   "8..7....4.5....6............3.97...8....43..5....2.9....6......2...6...7.71..83.2",
   ".8...4.5....7..3............1..85...6.....2......4....3.26............417........",
   "....7..8...6...5...2...3.61.1...7..2..8..534.2..9.......2......58...6.3.4...1....",
   "......8.16..2........7.5......6...2..1....3...8.......2......7..4..8....5...3....",
   ".2..........6....3.74.8.........3..2.8..4..1.6..5.........1.78.5....9..........4.",
   ".52..68.......7.2.......6....48..9..2..41......1.....8..61..38.....9...63..6..1.9",
   "....1.78.5....9..........4..2..........6....3.74.8.........3..2.8..4..1.6..5.....",
   "1.......3.6.3..7...7...5..121.7...9...7........8.1..2....8.64....9.2..6....4.....",
   "4...7.1....19.46.5.....1......7....2..2.3....847..6....14...8.6.2....3..6...9....",
   "......8.17..2........5.6......7...5..1....3...8.......5......2..3..8....6...4....",
   "963......1....8......2.5....4.8......1....7......3..257......3...9.2.4.7......9..",
   "15.3......7..4.2....4.72.....8.........9..1.8.1..8.79......38...........6....7423",
   "..........5724...98....947...9..3...5..9..12...3.1.9...6....25....56.....7......6",
   "....75....1..2.....4...3...5.....3.2...8...1.......6.....1..48.2........7........",
   "6.....7.3.4.8.................5.4.8.7..2.....1.3.......2.....5.....7.9......1....",
   "....6...4..6.3....1..4..5.77.....8.5...8.....6.8....9...2.9....4....32....97..1..",
   ".32.....58..3.....9.428...1...4...39...6...5.....1.....2...67.8.....4....95....6.",
   "...5.3.......6.7..5.8....1636..2.......4.1.......3...567....2.8..4.7.......2..5..",
   ".5.3.7.4.1.........3.......5.8.3.61....8..5.9.6..1........4...6...6927....2...9..",
   "..5..8..18......9.......78....4.....64....9......53..2.6.........138..5....9.714.",
   "..........72.6.1....51...82.8...13..4.........37.9..1.....238..5.4..9.........79.",
   "...658.....4......12............96.7...3..5....2.8...3..19..8..3.6.....4....473..",
   ".2.3.......6..8.9.83.5........2...8.7.9..5........6..4.......1...1...4.22..7..8.9",
   ".5..9....1.....6.....3.8.....8.4...9514.......3....2..........4.8...6..77..15..6.",
   ".....2.......7...17..3...9.8..7......2.89.6...13..6....9..5.824.....891..........",
   "3...8.......7....51..............36...2..4....7...........6.13..452...........8..",
  };

static const int TOP_95_COUNT = sizeof(TOP_95_PUZZLES) / sizeof(TOP_95_PUZZLES[0]);

/*

  Integration Tests

*/

static _Bool puzzle_can_be_solved(const char puzzle[82]) {
  uint_fast16_t grid[81] = {0};
  default_grid_values(grid);

  return solve(puzzle, grid);
}

static void test_invalid_puzzles_do_not_solve() {
  static const char *puzzles[] =
    {
     "111111111111111111111111111111111111111111111111111111111111111111111111111111111",
     "11...............................................................................",
     "11.............................................................................22",
     "11...................................55555.....................................22",
     "11..................44444444444444...55555.33333333..9999999999999999999999999.22",
    };

  static int count = sizeof(puzzles) / sizeof(puzzles[0]);

  for (int i = 0; i < count; ++i) {
    assert(puzzle_can_be_solved(puzzles[i]) == false);
  }
}

static void test_solves_easy_50_samples() {
  assert(puzzle_can_be_solved(EASY_50_PUZZLES[0]));
  assert(puzzle_can_be_solved(EASY_50_PUZZLES[16]));
  assert(puzzle_can_be_solved(EASY_50_PUZZLES[22]));
  assert(puzzle_can_be_solved(EASY_50_PUZZLES[37]));
  assert(puzzle_can_be_solved(EASY_50_PUZZLES[45]));
}

static void test_solves_hardest_11_samples() {
  assert(puzzle_can_be_solved(HARDEST_11_PUZZLES[1]));
  assert(puzzle_can_be_solved(HARDEST_11_PUZZLES[2]));
  assert(puzzle_can_be_solved(HARDEST_11_PUZZLES[5]));
  assert(puzzle_can_be_solved(HARDEST_11_PUZZLES[9]));
  assert(puzzle_can_be_solved(HARDEST_11_PUZZLES[10]));
}

static void test_solves_top_95_samples() {
  assert(puzzle_can_be_solved(TOP_95_PUZZLES[2]));
  assert(puzzle_can_be_solved(TOP_95_PUZZLES[24]));
  assert(puzzle_can_be_solved(TOP_95_PUZZLES[67]));
  assert(puzzle_can_be_solved(TOP_95_PUZZLES[73]));
  assert(puzzle_can_be_solved(TOP_95_PUZZLES[81]));
}

/*

  Test Runner

*/

static void run_tests() {
  test_can_eliminate_value_from_peers();
  test_eliminate_only_modifies_peers_with_values_to_remove();
  
  test_invalid_puzzles_do_not_solve();
  test_solves_easy_50_samples();
  test_solves_hardest_11_samples();
  test_solves_top_95_samples();
  
  printf("All tests passed.\n");
}

/*

  Profile

 */

static _Bool profile_puzzles(const char* puzzles[], int count) {
  for (int i = 0; i < count; ++i) {
    uint_fast16_t grid[81] = {0};
    default_grid_values(grid);
    
    if (!solve(puzzles[i], grid)) {
      return false;
    }
  }

  return true;
}

static void profile_easy_50_puzzles() {
  if (profile_puzzles(EASY_50_PUZZLES, EASY_50_COUNT)) {
    printf("Solved 50 of 50 easy puzzles\n");
  }
}

static void profile_hardest_11_puzzles() {
  if (profile_puzzles(HARDEST_11_PUZZLES, HARDEST_11_COUNT)) {
    printf("Solved 50 of 50 easy puzzles\n");
  }
}

static void profile_top_95_puzzles() {
  if (profile_puzzles(TOP_95_PUZZLES, TOP_95_COUNT)) {
    printf("Solved 50 of 50 easy puzzles\n");
  }
}

static void profile_example_puzzles() {
  profile_easy_50_puzzles();
  profile_hardest_11_puzzles();
  profile_top_95_puzzles();
}
