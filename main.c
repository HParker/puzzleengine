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

  Runtime rt = startGame(file);
  printf("game ready: %i levels\n", levelCount());

  char input[100];
  while (1) {
    if (checkWinConditions(&rt) == 1) {
      if (rt.levelIndex < levelCount() - 1) {
        nextLevel(&rt);
      } else {
        render(&rt);
        printf("you won!\n");
        return 0;
        // TODO: return to menu
      }
    }
    render(&rt);
    printf("Enter Move: ");
    gets(input);
    update(&rt, handleInput(&rt, input));
    setLevel(&rt);
    rt.toMoveCount = 0;
  }
  return 0;
}
