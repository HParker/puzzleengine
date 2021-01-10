#include <check.h>
#include <stdio.h>
#include "../puzzleData.h"
#include "../render.h"

void test_solution_file(Runtime * rt, char * puzzleFileName, char * solutionFileName) {
  FILE * puzzleFile = fopen(puzzleFileName, "r");
  FILE * solutionFile = fopen(solutionFileName, "r");

  startGame(rt, puzzleFile);

  printf("%s: ", rt->pd->title);

  int currentLevel = 0;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  Direction dir;
  while ((read = getline(&line, &len, solutionFile)) != -1) {
    if (strcasecmp(line, "RIGHT\n") == 0) {
      dir = RIGHT;
    } else if (strcasecmp(line, "UP\n") == 0) {
      dir = UP;
    } else if (strcasecmp(line, "LEFT\n") == 0) {
      dir = LEFT;
    } else if (strcasecmp(line, "DOWN\n") == 0) {
      dir = DOWN;
    } else if (strcasecmp(line, "USE\n") == 0) {
      dir = USE;
    } else {
      dir = NONE;
      fprintf(stderr, "err: failed to parse direction from solution file '%s'\n", line);
    }

    update(rt, dir);
    render(rt);

    if (rt->levelIndex > currentLevel) {
      currentLevel = rt->levelIndex;
      printf(".");
    }
  }
  /* if (rt->gameWon == 0) { */
  /*   fprintf(stderr, "Game incomplete %i/%i\n", rt->levelIndex, rt->pd->levelCount); */
  /* } */
  printf(".\n");
  /* endGame(rt); */
}

START_TEST (test_runs_stumper)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/stumper.puzz", "./solutions/stumper.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_basic)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/basic.puzz", "./solutions/basic.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_eyeball_walk)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/eyeball_walk.puzz", "./solutions/eyeball_walk.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_block_faker)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/block_faker.puzz", "./solutions/block_faker.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_kettle)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/kettle.puzz", "./solutions/kettle.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_constellation_z)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/constellation_z.puzz", "./solutions/constellation_z.solution");
  ck_assert_int_eq(10, rt.levelIndex);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_micro_ban)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/micro_ban.puzz", "./solutions/micro_ban.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_match3)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/match3.puzz", "./solutions/match3.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_neko_puzzle)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/neko_puzzle.puzz", "./solutions/neko_puzzle.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_lime_rick)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/lime_rick.puzz", "./solutions/lime_rick.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_not_snake)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/not_snake.puzz", "./solutions/not_snake.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_octat)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/octat.puzz", "./solutions/octat.solution");
  ck_assert_int_eq(11, rt.levelIndex);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_zen_puzzle_garden)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/zen_puzzle_garden.puzz", "./solutions/zen_puzzle_garden.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_soliquid)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/soliquid.puzz", "./solutions/soliquid.solution");
  ck_assert_int_eq(10, rt.levelIndex);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_watch_your_step)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/watch_your_step.puzz", "./solutions/watch_your_step.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_block_crusher)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/block_crusher.puzz", "./solutions/block_crusher.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_ihnmaimcboasofm)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/ihnmaimcboasofm.puzz", "./solutions/ihnmaimcboasofm.solution");
  ck_assert_int_eq(12, rt.levelIndex);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_stairways)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/stairways.puzz", "./solutions/stairways.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_enqueue)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/enqueue.puzz", "./solutions/enqueue.solution");
  ck_assert_int_eq(11, rt.levelIndex);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_ooo)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/ooo.puzz", "./solutions/ooo.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

START_TEST (test_runs_pathlines)
{
  Runtime rt;
  test_solution_file(&rt, "./puzzles/pathlines.puzz", "./solutions/pathlines.solution");
  ck_assert_int_eq(1, rt.gameWon);
  endGame(&rt);
}
END_TEST

Suite * puzzle_script_parser_suite(void)
{
  Suite * s;
  TCase * test_Runner;

  s = suite_create("Runner");

  test_Runner = tcase_create("Runner");
  tcase_set_timeout(test_Runner, 100);

  tcase_add_test(test_Runner, test_runs_stumper);
  tcase_add_test(test_Runner, test_runs_basic);
  tcase_add_test(test_Runner, test_runs_eyeball_walk);
  tcase_add_test(test_Runner, test_runs_match3);
  tcase_add_test(test_Runner, test_runs_block_faker);
  tcase_add_test(test_Runner, test_runs_constellation_z);
  tcase_add_test(test_Runner, test_runs_kettle);
  tcase_add_test(test_Runner, test_runs_lime_rick);
  tcase_add_test(test_Runner, test_runs_match3);
  tcase_add_test(test_Runner, test_runs_micro_ban);
  tcase_add_test(test_Runner, test_runs_neko_puzzle);
  tcase_add_test(test_Runner, test_runs_not_snake);
  tcase_add_test(test_Runner, test_runs_octat);

  // TODO: come up with a way of testing these random levels work.
  //       something like what eyeball_walk.
  /* tcase_add_test(test_Runner, test_parses_random_robots); */
  /* tcase_add_test(test_Runner, test_parses_random_robots_spawner); */
  tcase_add_test(test_Runner, test_runs_zen_puzzle_garden);
  tcase_add_test(test_Runner, test_runs_soliquid);
  tcase_add_test(test_Runner, test_runs_watch_your_step);
  tcase_add_test(test_Runner, test_runs_block_crusher);
  tcase_add_test(test_Runner, test_runs_ihnmaimcboasofm);
  tcase_add_test(test_Runner, test_runs_stairways);
  tcase_add_test(test_Runner, test_runs_enqueue);
  tcase_add_test(test_Runner, test_runs_ooo);
  tcase_add_test(test_Runner, test_runs_pathlines);

  suite_add_tcase(s, test_Runner);

  return s;
}


int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;

  initRenderer();

  s = puzzle_script_parser_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  closeRenderer();

  return (number_failed == 0) ? 0 : 1;
}
