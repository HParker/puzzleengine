#include <stdio.h>
#include <string.h>
#include "puzzleData.h"

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

  char input[100];
  while (1) {
    render(&rt);
    printf("Enter Move: ");
    gets(input);
    update(&rt, handleInput(&rt, input));
    if (rt.gameWon == 1) {
      return 0;
    }
  }
  return 0;
}
