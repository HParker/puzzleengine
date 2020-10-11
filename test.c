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
}
END_TEST

START_TEST (test_parses_eyeball)
{
  FILE *file;
  file = fopen("./puzzles/eyeball.ps", "r");
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
    tcase_add_test(test_Parser, test_parses_eyeball);

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
