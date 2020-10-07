#include <stdio.h>
#include <string.h>
/* #include "puzzleData.h" */
#include "puzz.tab.c"

typedef struct Runtime {
  int levelIndex;
  Level currentLevel;
  // move history?
} Runtime;



Runtime startGame() {
  Runtime rt;
  rt.levelIndex = 0;
  rt.currentLevel = pd.levels[0];
  return rt;
}

void render(Runtime * rt) {
  for (int i = 0; i < rt->currentLevel.cellIndex; i++) {
    printf("%c", rt->currentLevel.cells[i]);
    if ((i + 1) % (rt->currentLevel.width) == 0 && i != 0) {
      printf("\n");
    }
  }
}

void swapSquares(Runtime * rt, int source, int dest) {
  char temp;
  temp = rt->currentLevel.cells[source];
  rt->currentLevel.cells[source] = rt->currentLevel.cells[dest];
  rt->currentLevel.cells[dest] = temp;
}

int playerLocation(Runtime * rt) {
  for (int i = 0; i < rt->currentLevel.cellIndex; i++) {
    if (rt->currentLevel.cells[i] == 'P') {
      return i;
    }
  }
  return -1;
}

void update(Runtime * rt, char * input) {
  if (strcmp(input, "right") == 0) {
    printf("MOVING RIGHT\n");
    int playerLoc = playerLocation(rt);
    swapSquares(rt, playerLoc, playerLoc + 1);
  }
  if (strcmp(input, "left") == 0) {
    printf("MOVING LEFT\n");
    int playerLoc = playerLocation(rt);
    swapSquares(rt, playerLoc, playerLoc - 1);
  }

  // mark player to move

  // apply rule

  // move player
}

int main(int argc, char ** argv) {
  FILE *file;
  if (argc > 1) {
    file = fopen(argv[1], "r");
    if (!file) {
      fprintf(stderr,"could not open %s\n", argv[1]);
      return 1;
    }
  }
  yyin = file;
  yyparse();
  printf("pd ready: %i levels\n", pd.levelCount);
  printf("DONE!\n");

  Runtime rt = startGame();

  char input[100];
  while (1) {
    render(&rt);
    printf("Enter Move: ");
    gets(input);
    update(&rt, input);
  }
  return 0;
}
