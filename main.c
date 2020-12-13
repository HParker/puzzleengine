#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
  startGame(&rt, file);
  rt.toMoveCount = 0;
  initRenderer();

  char curInput;
  while (1) {
    render(&rt);
    curInput = input();
    if (curInput != -1) {
      Direction dir = handleInput(&rt, curInput);
      if (dir != NONE) {
        if (dir == QUIT) {
          break;
        }
        if (dir == UNDO && rt.statesCount > 0) {
          undo(&rt, 0);
        } else {
          update(&rt, dir);
        }

        if (rt.gameWon == 1) {
          break;
        }
      }
    }
  }

  printHistory(&rt);
  closeRenderer();
  endGame(&rt);
  return 0;
}
