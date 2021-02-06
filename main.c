#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "puzzleData.h"
#include "render.h"

int main(int argc, char ** argv) {
  Direction inputDir;
  int frameCounter = 0;
  FILE *file;
  if (argc > 1) {
    file = fopen(argv[1], "r");
    if (!file) {
      fprintf(stderr, "could not open %s\n", argv[1]);
      return 1;
    }
  } else {
    fprintf(stderr, "Please provide a puzzlescript file\n");
    return 1;
  }
  Runtime rt;
  startGame(&rt, file);
  rt.toMoveCount = 0;
  initRenderer();

  while (1) {
    if (rt.pd->realTimeInterval > 0 && rt.pd->realTimeInterval*60.0f < frameCounter) {
      tick(&rt);
      frameCounter = 0;
    }
    render(&rt);
    frameCounter++;
    if (rt.doAgain) {
      continue;
    }

    inputDir = handleInput(&rt, input());

    if (inputDir != UNSPECIFIED) {
      if (inputDir == QUIT) {
        break;
      }
      if (inputDir == UNDO && rt.statesCount > 0) {
        undo(&rt, 0);
      } else {
        update(&rt, inputDir);
      }

      if (rt.gameWon == 1) {
        break;
      }
    }
  }

  printHistory(&rt);
  closeRenderer();
  endGame(&rt);
  return 0;
}
