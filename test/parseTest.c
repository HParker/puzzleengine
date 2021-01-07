#include <check.h>
#include <stdio.h>
#include "../puzzleData.h"

PuzzleData * loadPuzzle(char * filename) {
  printf("parsing: %s\n", filename);
  FILE *file;
  file = fopen(filename, "r");
  PuzzleData * pf = parsePuzzle(file);
  fclose(file);
  return pf;
}


START_TEST (test_parses_basic)
{
  PuzzleData * pz = loadPuzzle("./puzzles/basic.puzz");
  ck_assert_str_eq(pz->title, "Simple Block Pushing Game");
  ck_assert_int_eq(pz->layerCount, 3);
  ck_assert_int_eq(pz->levelCount, 2);
  freePuzzle();
}
END_TEST

START_TEST (test_parses_block_faker)
{
  PuzzleData * pz = loadPuzzle("./puzzles/block_faker.puzz");
  ck_assert_str_eq(pz->title, "Block Faker");
  ck_assert_int_eq(pz->layerCount, 4);
  ck_assert_int_eq(pz->levelCount, 10);
  freePuzzle();
}
END_TEST

START_TEST (test_parses_by_your_side)
{
  PuzzleData * pz = loadPuzzle("./puzzles/by_your_side.puzz");
  ck_assert_str_eq(pz->title, "By Your Side");
  ck_assert_int_eq(pz->layerCount, 4);
  ck_assert_int_eq(pz->levelCount, 12);
  freePuzzle();
}
END_TEST

START_TEST (test_parses_constellation_z)
{
  PuzzleData * pz = loadPuzzle("./puzzles/constellation_z.puzz");
  ck_assert_str_eq(pz->title, "constellation z");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_eyeball)
{
  PuzzleData * pz = loadPuzzle("./puzzles/eyeball.puzz");
  ck_assert_str_eq(pz->title, "EYE EYE EYE");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_kettle)
{
  PuzzleData * pz = loadPuzzle("./puzzles/kettle.puzz");
  ck_assert_str_eq(pz->title, "Kettle");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_lime_rick)
{
  PuzzleData * pz = loadPuzzle("./puzzles/lime_rick.puzz");
  ck_assert_str_eq(pz->title, "Lime Rick");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_match3)
{
  PuzzleData * pz = loadPuzzle("./puzzles/match3.puzz");
  ck_assert_str_eq(pz->title, "Match 3 Block Push");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_micro_ban)
{
  PuzzleData * pz = loadPuzzle("./puzzles/micro_ban.puzz");
  ck_assert_str_eq(pz->title, "Microban");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_minimum)
{
  PuzzleData * pz = loadPuzzle("./puzzles/minimum.puzz");
  ck_assert_str_eq(pz->title, "Simple Block Pushing Game");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_neko_puzzle)
{
  PuzzleData * pz = loadPuzzle("./puzzles/neko_puzzle.puzz");
  ck_assert_str_eq(pz->title, "neko puzzle");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_not_snake)
{
  PuzzleData * pz = loadPuzzle("./puzzles/not_snake.puzz");
  ck_assert_str_eq(pz->title, "Notsnake");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_octat)
{
  PuzzleData * pz = loadPuzzle("./puzzles/octat.puzz");
  ck_assert_str_eq(pz->title, "Octat");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_random_robots)
{
  PuzzleData * pz = loadPuzzle("./puzzles/random_robots.puzz");
  ck_assert_str_eq(pz->title, "Random Robots");
  freePuzzle();
}
END_TEST


START_TEST (test_parses_random_robots_spawner)
{
  PuzzleData * pz = loadPuzzle("./puzzles/random_robots_spawner.puzz");
  ck_assert_str_eq(pz->title, "Robot Spawner");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_zen_puzzle_garden)
{
  PuzzleData * pz = loadPuzzle("./puzzles/zen_puzzle_garden.puzz");
  ck_assert_str_eq(pz->title, "Zen Puzzle Garden");
  freePuzzle();
}
END_TEST

START_TEST (test_parses_enqueue)
{
  PuzzleData * pz = loadPuzzle("./puzzles/enqueue.puzz");
  ck_assert_str_eq(pz->title, "Enqueue");
  freePuzzle();
}
END_TEST

Suite * puzzle_script_parser_suite(void)
{
  Suite * s;
  TCase * test_Parser;

  s = suite_create("Parser");

  test_Parser = tcase_create("Parser");
  tcase_set_timeout(test_Parser, 100);
  tcase_add_test(test_Parser, test_parses_basic);
  tcase_add_test(test_Parser, test_parses_block_faker);
  tcase_add_test(test_Parser, test_parses_by_your_side);
  tcase_add_test(test_Parser, test_parses_constellation_z);
  tcase_add_test(test_Parser, test_parses_eyeball);
  tcase_add_test(test_Parser, test_parses_kettle);
  tcase_add_test(test_Parser, test_parses_lime_rick);
  tcase_add_test(test_Parser, test_parses_match3);
  tcase_add_test(test_Parser, test_parses_micro_ban);
  tcase_add_test(test_Parser, test_parses_minimum);
  tcase_add_test(test_Parser, test_parses_neko_puzzle);
  tcase_add_test(test_Parser, test_parses_not_snake);
  tcase_add_test(test_Parser, test_parses_octat);
  tcase_add_test(test_Parser, test_parses_random_robots);
  tcase_add_test(test_Parser, test_parses_random_robots_spawner);
  tcase_add_test(test_Parser, test_parses_zen_puzzle_garden);
  tcase_add_test(test_Parser, test_parses_enqueue);

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
