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

  while (1) {
    render(&rt);
    Direction dir = handleInput(&rt, input());
    if (dir == QUIT) {
      break;
    }
    if (dir == UNDO && rt.statesCount > 0) {
      undo(&rt);
    } else {
      update(&rt, dir);
    }

    if (rt.gameWon == 1) {
      break;
    }
  }

  printHistory(&rt);
  closeRenderer();
  return 0;
}
