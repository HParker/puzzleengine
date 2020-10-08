#include <stdio.h>
#include <string.h>
/* #include "puzzleData.h" */
#include "puzz.tab.c"

typedef struct Runtime {
  int levelIndex;
  Level currentLevel;
  // move history?
} Runtime;


char nameToLegendKey(const char * name) {
  for (int i = 0; i < pd.objectCount; i++) {
    if (strcmp(pd.objects[i].name, name) == 0) {
      return pd.objects[i].legendKey;
    }
  }
  return '\t';
}

// TODO: lets try to never use this to avoid managing the strings it could create
/* char * legendKeyToName(char key, ) { */

/* } */

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
      printf("P = %i\n", i);
      return i;
    }
  }
  return -1;
}

int isMovable(Runtime * rt, int loc) {
  if (rt->currentLevel.cells[loc] == '.') {
    return 1;
  } else {
    return 0;
  }
}

typedef enum Action
  {
   ACTION_UP,
   ACTION_DOWN,
   ACTION_LEFT,
   ACTION_RIGHT,
   ACTION_USE
  } Action;

int cellMatches(Runtime * rt, int cellIndex, int cellMultiplier, RuleState * rs) {
  // TODO: this ignores direction and shouldn't
  int matches = 0;
  for (int i = 0; i < rs->partCount; i++) {
    if (rt->currentLevel.cells[cellIndex + (i * cellMultiplier)] != nameToLegendKey(rs->parts[i].identifier)) {
      matches = 1;
    } else {
      /* printf("matched: %i (%c == %s)\n", cellIndex + i, rt->currentLevel.cells[cellIndex + (i * cellMultiplier)], rs->parts[i].identifier); */
    }
  }
  return matches;
}

int doResultState(Runtime * rt, Action act, int ruleIndex, int cellIndex, int cellMultipler) {
  // TODO: direction
  for (int i = 0; i < pd.rules[ruleIndex].resultStateCount; i++) {
    for (int j = 0; j < pd.rules[ruleIndex].resultStates[i].partCount; j++) {
      rt->currentLevel.cells[cellIndex + (j * cellMultipler)] = nameToLegendKey(pd.rules[ruleIndex].resultStates[i].parts[j].identifier);
    }
  }
  return 0;
}

int applyRules(Runtime * rt, ExecutionTime execTime, Action act) {
  // do things like move blocks when you push them
  for (int ruleIndex = 0; ruleIndex < pd.ruleCount; ruleIndex++) {
    int appliedSomething = 1;
    while (appliedSomething == 1) {
      appliedSomething = 0;
      if (pd.rules[ruleIndex].executionTime == execTime) {
        // applies to the right
        for (int cellIndex = 0; cellIndex < rt->currentLevel.cellIndex; cellIndex++) {
          // right
          for (int matchIndex = 0; matchIndex < pd.rules[ruleIndex].matchStateCount; matchIndex++) {
            if (cellMatches(rt, cellIndex, 1, &pd.rules[ruleIndex].matchStates[matchIndex]) == 0) {
              printf("APPLYING RIGHT\n");
              appliedSomething = 1;
              doResultState(rt, act, ruleIndex, cellIndex, 1);
            }
          }

          // left
          for (int cellIndex = 0; cellIndex < rt->currentLevel.cellIndex; cellIndex++) {
            for (int matchIndex = 0; matchIndex < pd.rules[ruleIndex].matchStateCount; matchIndex++) {
              if (cellMatches(rt, cellIndex, -1, &pd.rules[ruleIndex].matchStates[matchIndex]) == 0) {
                printf("APPLYING LEFT\n");
                appliedSomething = 1;
                doResultState(rt, act, ruleIndex, cellIndex, -1);
              }
            }
          }

          // up
          for (int cellIndex = 0; cellIndex < rt->currentLevel.cellIndex; cellIndex++) {
            for (int matchIndex = 0; matchIndex < pd.rules[ruleIndex].matchStateCount; matchIndex++) {
              if (cellMatches(rt, cellIndex, (rt->currentLevel.width * -1), &pd.rules[ruleIndex].matchStates[matchIndex]) == 0) {
                printf("APPLYING UP\n");
                appliedSomething = 1;
                doResultState(rt, act, ruleIndex, cellIndex, (rt->currentLevel.width * -1));
              }
            }
          }

          // up
          for (int cellIndex = 0; cellIndex < rt->currentLevel.cellIndex; cellIndex++) {
            for (int matchIndex = 0; matchIndex < pd.rules[ruleIndex].matchStateCount; matchIndex++) {
              if (cellMatches(rt, cellIndex, rt->currentLevel.width, &pd.rules[ruleIndex].matchStates[matchIndex]) == 0) {
                printf("APPLYING DOWN\n");
                appliedSomething = 1;
                doResultState(rt, act, ruleIndex, cellIndex, rt->currentLevel.width);
              }
            }
          }
        }
      }
    }


    // applies down
    // applies left
    // applies up
  }
  return 0;
}

void update(Runtime * rt, char * input) {
  Action act;
  // mark player to move
  if (strcmp(input, "right") == 0) {
    act = ACTION_RIGHT;
  } else if (strcmp(input, "up") == 0) {
    act = ACTION_UP;
  } else if (strcmp(input, "left") == 0) {
    act = ACTION_LEFT;
  } else if (strcmp(input, "down") == 0) {
    act = ACTION_DOWN;
  } else if (strcmp(input, "use") == 0) {
    act = ACTION_USE;
  }


  // apply rule
  applyRules(rt, NORMAL, act);

  // move player
  if (strcmp(input, "right") == 0) {
    int playerLoc = playerLocation(rt);
    if (isMovable(rt, playerLoc + 1)) {
      swapSquares(rt, playerLoc, playerLoc + 1);
    }
  }
  if (strcmp(input, "left") == 0) {
    int playerLoc = playerLocation(rt);
    if (isMovable(rt, playerLoc - 1)) {
      swapSquares(rt, playerLoc, playerLoc - 1);
    }
  }

  if (strcmp(input, "up") == 0) {
    int playerLoc = playerLocation(rt);
    if (isMovable(rt, playerLoc - rt->currentLevel.width)) {
      swapSquares(rt, playerLoc, playerLoc - rt->currentLevel.width);
    }
  }

  if (strcmp(input, "down") == 0) {
    int playerLoc = playerLocation(rt);
    if (isMovable(rt, playerLoc + rt->currentLevel.width)) {
      swapSquares(rt, playerLoc, playerLoc + rt->currentLevel.width);
    }
  }

  // apply late rules
  applyRules(rt, LATE, act);
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
