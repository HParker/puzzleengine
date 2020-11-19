#include <check.h>
#include <stdio.h>
#include "puzzleData.h"

void test_solution(Runtime * rt, int moveCount, int moves[]) {
  for (int i = 0; i < moveCount; i++) {
    update(rt, moves[i]);
  }
  if (rt->gameWon == 0) {
    printf("Game incomplete %i/%i\n", rt->levelIndex, rt->pd->levelCount);
  }
  ck_assert_int_eq(1, rt->gameWon);
}

START_TEST (test_runs_stumper)
{
  FILE *file;
  file = fopen("./puzzles/stumper.puzz", "r");
  Runtime rt;
  startGame(&rt, file);

  int moves[] = {
                 RIGHT, // 0
                 LEFT, // 1
                 UP, // 2
                 DOWN, // 3
                 RIGHT, // 4
                 LEFT,
                 DOWN,
                 UP, // 5
                 DOWN,
                 LEFT,
                 UP, // 6
                 DOWN,
                 DOWN,
                 UP,
                 LEFT,
                 RIGHT,
                 RIGHT
  };

  test_solution(&rt, sizeof(moves)/sizeof(moves[0]), moves);
}
END_TEST

START_TEST (test_runs_basic)
{
  FILE *file;
  file = fopen("./puzzles/basic.puzz", "r");
  Runtime rt;
  startGame(&rt, file);

  int moves[] = {
                 DOWN,
                 LEFT,
                 UP,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 DOWN,
                 LEFT,
                 UP,
                 LEFT,
                 LEFT,
                 DOWN,
                 DOWN,
                 RIGHT,
                 UP,
                 LEFT,
                 UP,
                 RIGHT,
                 UP,
                 UP,
                 LEFT,
                 DOWN,
                 RIGHT,
                 DOWN,
                 DOWN,
                 RIGHT,
                 RIGHT,
                 UP,
                 LEFT,
                 DOWN,
                 LEFT,
                 UP,
                 UP,
                 RIGHT,
                 DOWN,
                 DOWN,
                 LEFT,
                 RIGHT,
                 UP,
                 UP,
                 LEFT,
                 DOWN,
                 UP,
                 UP,
                 LEFT,
                 LEFT,
                 DOWN,
                 DOWN,
                 RIGHT
  };
  test_solution(&rt, sizeof(moves)/sizeof(moves[0]), moves);
}
END_TEST

START_TEST (test_runs_eyeball_walk)
{
  FILE *file;
  file = fopen("./puzzles/eyeball_walk.puzz", "r");
  Runtime rt;
  startGame(&rt, file);

  int moves[] = {
                 UP,
                 LEFT,
                 DOWN,
                 DOWN,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 UP,
                 UP,
                 DOWN,
                 DOWN,
                 DOWN,
                 LEFT,
                 LEFT,
                 RIGHT,
                 LEFT,
                 LEFT
  };
  test_solution(&rt, sizeof(moves)/sizeof(moves[0]), moves);
}
END_TEST

START_TEST (test_runs_match3)
{
  FILE *file;
  file = fopen("./puzzles/match3.puzz", "r");
  Runtime rt;
  startGame(&rt, file);

  int moves[] = {
                 DOWN,
                 DOWN,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 UP,
                 LEFT,
                 LEFT,
                 DOWN,
                 LEFT,
                 UP,
                 UP,
                 UP,
                 LEFT,
                 UP,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 UP,
                 UP,
                 RIGHT,
                 DOWN,
                 RIGHT,
                 DOWN,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 DOWN,
                 LEFT,
                 UP,
                 UP
  };
  test_solution(&rt, sizeof(moves)/sizeof(moves[0]), moves);
}
END_TEST

START_TEST (test_runs_block_faker)
{
  FILE *file;
  file = fopen("./puzzles/block_faker.puzz", "r");
  Runtime rt;
  startGame(&rt, file);

  int moves[] = {
                 UP,
                 LEFT,
                 LEFT,
                 UP,
                 LEFT,
                 DOWN,
                 DOWN,
                 DOWN,
                 LEFT,
                 LEFT,
                 UP,
                 UP,
                 UP,
                 RIGHT,
                 UP,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 UP,
                 RIGHT,
                 DOWN,
                 DOWN,
                 RIGHT,
                 DOWN,
                 RIGHT,
                 USE,
                 UP,
                 UP,
                 UP,
                 UP,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 DOWN,
                 DOWN,
                 RIGHT,
                 UP,
                 LEFT,
                 UP,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 UP,
                 RIGHT,
                 DOWN,
                 DOWN,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 USE,
                 DOWN,
                 RIGHT,
                 DOWN,
                 RIGHT,
                 DOWN,
                 DOWN,
                 RIGHT,
                 DOWN,
                 RIGHT,
                 DOWN,
                 DOWN,
                 DOWN,
                 DOWN,
                 LEFT,
                 LEFT,
                 UP,
                 UP,
                 UP,
                 DOWN,
                 DOWN,
                 DOWN,
                 RIGHT,
                 RIGHT,
                 UP,
                 UP,
                 UP,
                 UP,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 RIGHT,
                 RIGHT,
                 UP,
                 UP,
                 UP,
                 LEFT,
                 LEFT,
                 DOWN,
                 DOWN,
                 DOWN,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 UP,
                 UP,
                 LEFT,
                 DOWN,
                 DOWN,
                 RIGHT,
                 DOWN,
                 LEFT,
                 LEFT,
                 DOWN,
                 LEFT,
                 UP,
                 RIGHT,
                 UP,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 USE,
                 UP,
                 UP,
                 UP,
                 LEFT,
                 UP,
                 RIGHT,
                 RIGHT,
                 UP,
                 RIGHT,
                 DOWN,
                 LEFT,
                 DOWN,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 DOWN,
                 DOWN,
                 LEFT,
                 LEFT,
                 DOWN,
                 LEFT,
                 UP,
                 UP,
                 LEFT,
                 UP,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 LEFT,
                 UP,
                 UP,
                 LEFT,
                 UP,
                 UP,
                 UP,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 UP,
                 RIGHT,
                 DOWN,
                 DOWN,
                 LEFT,
                 DOWN,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 DOWN,
                 DOWN,
                 LEFT,
                 DOWN,
                 DOWN,
                 DOWN,
                 RIGHT,
                 RIGHT,
                 UP,
                 UP,
                 UP,
                 UP,
                 UP,
                 UP,
                 UP,
                 UP,
                 USE,
                 DOWN,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 RIGHT,
                 DOWN,
                 DOWN,
                 DOWN,
                 DOWN,
                 USE
  };
  test_solution(&rt, sizeof(moves)/sizeof(moves[0]), moves);
}
END_TEST

START_TEST (test_runs_kettle)
{
  FILE *file;
  file = fopen("./puzzles/kettle.puzz", "r");
  Runtime rt;
  startGame(&rt, file);

  int moves[] = {
                 USE,
                 USE,
                 USE,
                 USE,
                 UP,
                 RIGHT,
                 LEFT,
                 DOWN,
                 USE,
                 USE,
                 UP,
                 LEFT,
                 USE,
                 USE,
                 UP,
                 LEFT,
                 USE,
                 USE,
                 UP,
                 RIGHT,
                 LEFT,
                 DOWN,
                 USE,
                 USE,
                 LEFT,
                 RIGHT,
                 LEFT,
                 UP,
                 DOWN,
                 USE,
                 USE,
                 RIGHT,
                 UP,
                 DOWN,
                 LEFT,
                 USE,
                 USE,
                 UP,
                 RIGHT,
                 LEFT,
                 LEFT,
                 UP,
                 DOWN,
                 USE,
                 USE,
                 RIGHT,
                 LEFT,
                 LEFT,
                 DOWN,
                 UP,
                 UP,
                 USE,
                 USE,
                 DOWN,
                 RIGHT,
                 DOWN,
                 RIGHT,
                 LEFT,
                 UP,
                 LEFT,
                 UP,
                 USE,
                 USE,
                 RIGHT,
                 LEFT,
                 DOWN,
                 RIGHT,
                 UP,
                 UP,
                 LEFT,
                 LEFT,
                 UP,
                 DOWN,
                 USE,
                 USE,
                 LEFT,
                 DOWN,
                 DOWN,
                 RIGHT,
                 UP,
                 UP,
                 UP,
                 LEFT,
                 LEFT,
                 RIGHT,
                 USE,
                 USE,
                 USE
  };

  test_solution(&rt, sizeof(moves)/sizeof(moves[0]), moves);
}
END_TEST

Suite * puzzle_script_parser_suite(void)
{
  Suite * s;
  TCase * test_Parser;

  s = suite_create("Runner");

  test_Parser = tcase_create("Runner");
  tcase_add_test(test_Parser, test_runs_stumper);
  tcase_add_test(test_Parser, test_runs_basic);
  tcase_add_test(test_Parser, test_runs_eyeball_walk);
  tcase_add_test(test_Parser, test_runs_match3);
  tcase_add_test(test_Parser, test_runs_block_faker);
  /* tcase_add_test(test_Parser, test_parses_constellation_z); */
  tcase_add_test(test_Parser, test_runs_kettle);
  /* tcase_add_test(test_Parser, test_parses_lime_rick); */
  /* tcase_add_test(test_Parser, test_parses_match3); */
  /* tcase_add_test(test_Parser, test_parses_micro_ban); */
  /* tcase_add_test(test_Parser, test_parses_minimum); */
  /* tcase_add_test(test_Parser, test_parses_neko_puzzle); */
  /* tcase_add_test(test_Parser, test_parses_not_snake); */
  /* tcase_add_test(test_Parser, test_parses_octat); */
  /* tcase_add_test(test_Parser, test_parses_random_robots); */
  /* tcase_add_test(test_Parser, test_parses_random_robots_spawner); */
  /* tcase_add_test(test_Parser, test_parses_zen_puzzle_garden); */

  suite_add_tcase(s, test_Parser);
  return s;
}


int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = puzzle_script_parser_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? 0 : 1;
}
