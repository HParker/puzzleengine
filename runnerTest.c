#include <check.h>
#include <stdio.h>
#include "puzzleData.h"

void test_solution(Runtime * rt, int moveCount, int moves[]) {
  for (int i = 0; i < moveCount; i++) {
    update(rt, moves[i]);
    setLevel(rt);
  }
  if (rt->gameWon == 0) {
    printf("Game incomplete %i/%i\n", rt->levelIndex, rt->pd->levelCount);
    render(rt);
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
  file = fopen("./puzzles/basic.ps", "r");
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

START_TEST (test_runs_block_faker)
{
  FILE *file;
  file = fopen("./puzzles/block_faker.ps", "r");
  Runtime rt;
  startGame(&rt, file);

  int moves[] = {
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
    /* tcase_add_test(test_Parser, test_runs_block_faker); */
    /* tcase_add_test(test_Parser, test_parses_constellation_z); */
    /* tcase_add_test(test_Parser, test_parses_eyeball); */
    /* tcase_add_test(test_Parser, test_parses_kettle); */
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
