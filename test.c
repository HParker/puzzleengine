#include <check.h>
#include <stdio.h>
#include "puzzle.tab.c"

START_TEST (test_parses_basic)
{
  FILE *file;
  file = fopen("./puzzles/basic.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
  ck_assert_int_eq(pd.objectCount, 5);
  ck_assert_int_eq(pd.legendCount, 6);
  ck_assert_int_eq(pd.layerCount, 3);
  ck_assert_int_eq(pd.ruleCount, 1);
  ck_assert_int_eq(pd.levelCount, 2);
}
END_TEST

START_TEST (test_parses_block_faker)
{
  FILE *file;
  file = fopen("./puzzles/block_faker.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Block Faker");
  ck_assert_int_eq(pd.objectCount, 11);
  ck_assert_int_eq(pd.legendCount, 16);
  ck_assert_int_eq(pd.layerCount, 4);
  ck_assert_int_eq(pd.ruleCount, 7);
  ck_assert_int_eq(pd.levelCount, 5);
}
END_TEST

START_TEST (test_parses_by_your_side)
{
  FILE *file;
  file = fopen("./puzzles/by_your_side.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "By Your Side");
  ck_assert_int_eq(pd.objectCount, 11);
  ck_assert_int_eq(pd.legendCount, 16);
  ck_assert_int_eq(pd.layerCount, 4);
  ck_assert_int_eq(pd.ruleCount, 7);
  ck_assert_int_eq(pd.levelCount, 5);
}
END_TEST

START_TEST (test_parses_constellation_z)
{
  FILE *file;
  file = fopen("./puzzles/constellation_z.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
}
END_TEST

START_TEST (test_parses_eyeball)
{
  FILE *file;
  file = fopen("./puzzles/eyeball.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
}
END_TEST

START_TEST (test_parses_kettle)
{
  FILE *file;
  file = fopen("./puzzles/kettle.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
}
END_TEST

START_TEST (test_parses_lime_rick)
{
  FILE *file;
  file = fopen("./puzzles/lime_rick.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
}
END_TEST

START_TEST (test_parses_match3)
{
  FILE *file;
  file = fopen("./puzzles/match3.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
}
END_TEST

START_TEST (test_parses_micro_ban)
{
  FILE *file;
  file = fopen("./puzzles/micro_ban.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
}
END_TEST

START_TEST (test_parses_minimum)
{
  FILE *file;
  file = fopen("./puzzles/minimum.puzz", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
}
END_TEST

START_TEST (test_parses_neko_puzzle)
{
  FILE *file;
  file = fopen("./puzzles/neko_puzzle.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
}
END_TEST

START_TEST (test_parses_not_snake)
{
  FILE *file;
  file = fopen("./puzzles/not_snake.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
}
END_TEST

START_TEST (test_parses_octat)
{
  FILE *file;
  file = fopen("./puzzles/octat.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
}
END_TEST

START_TEST (test_parses_random_robots)
{
  FILE *file;
  file = fopen("./puzzles/random_robots.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "Simple Block Pushing Game");
}
END_TEST


START_TEST (test_parses_random_robots_spawner)
{
  FILE *file;
  file = fopen("./puzzles/random_robots_spawner.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "EYE EYE EYE");
}
END_TEST

START_TEST (test_parses_zen_puzzle_garden)
{
  FILE *file;
  file = fopen("./puzzles/zen_puzzle_garden.ps", "r");
  yyin = file;
  yyparse();
  ck_assert_str_eq(pd.title, "EYE EYE EYE");
}
END_TEST

Suite * puzzle_script_parser_suite(void)
{
    Suite * s;
    TCase * test_Parser;

    s = suite_create("PuzzleScript");

    test_Parser = tcase_create("Parser");
    tcase_add_test(test_Parser, test_parses_basic);
    tcase_add_test(test_Parser, test_parses_block_faker);
    /* tcase_add_test(test_Parser, test_parses_by_your_side); */
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
