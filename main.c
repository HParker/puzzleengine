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
      rt.statesCount--;
      rt.levelIndex = rt.states[rt.statesCount].levelIndex;
      rt.levelType = levelType(rt.levelIndex);
      rt.height = levelHeight(rt.levelIndex);
      rt.width = levelWidth(rt.levelIndex);
      rt.toMoveCount = 0;


      rt.objectCount = rt.states[rt.statesCount].objectCount;
      if (rt.objectCapacity < rt.states[rt.statesCount].objectCapacity) {
        printf("states rewind realloc\n");
        rt.statesCapacity = rt.states[rt.statesCount].objectCapacity;
        rt.states = realloc(rt.states, sizeof(State) * rt.statesCapacity);
      }
      memcpy(rt.objects, rt.states[rt.statesCount].objects, sizeof(State) * rt.statesCapacity);
    } else {
      update(&rt, dir);
    }




    if (rt.gameWon == 1) {
      break;
    }
  }

  closeRenderer();
  printHistory(&rt);
  return 0;
}
