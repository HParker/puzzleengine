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

  initRenderer();

  while (1) {
    render(&rt);
    frameCounter++;

    if (rt.doAgain) {
      if (rt.pd->againInterval*60.0f < frameCounter) {
        tick(&rt);
        frameCounter = 0;
      }
      continue;
    } else if (rt.pd->setRealtimeInterval && rt.pd->realTimeInterval > 0 && rt.pd->realTimeInterval*60.0f < frameCounter) {
      tick(&rt);
      frameCounter = 0;
    } else {
      inputDir = handleInput(&rt, input());
      if (inputDir != UNSPECIFIED) {
        if (inputDir == QUIT) {
          break;
        }
        if (inputDir == UNDO) {
          if (rt.statesCount > 0) {
            undo(&rt);
          }
          rt.didUndo = 0;
        } else if (inputDir == NEXT_LEVEL) {
          nextLevel(&rt);
        } else {
          update(&rt, inputDir);
          if (rt.doAgain) {
            frameCounter = 0;
          }
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
