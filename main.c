#include <stdio.h>
#include <string.h>
#include <curses.h>
#include "puzzleData.h"
#include "render.h"

int main(int argc, char ** argv) {
  FILE *file;
  if (argc > 1) {
    file = fopen(argv[1], "r");
    if (!file) {
      fprintf(stderr,"could not open %s\n", argv[1]);
      return 1;
    }
  } else {
    printf("Please provide a puzzlescript file\n");
    return 1;
  }
  Runtime rt;
  rt.objectCount = 0;
  startGame(&rt, file);
  rt.toMoveCount = 0;
  initRenderer();

  int input;
  while (1) {
    render(&rt);
    input = getch();
    update(&rt, handleInput(&rt, input));
    if (rt.gameWon == 1) {
      break;
    }
  }
  endwin();
  printHistory(&rt);
  return 0;
}
