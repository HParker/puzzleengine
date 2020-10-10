#include <stdio.h>
#include <string.h>
/* #include "puzzleData.h" */
#include "puzzle.tab.c"

typedef struct ToMove {
  int from;
  int to;
} ToMove;

typedef struct Runtime {
  int levelIndex;
  Level currentLevel;
  int toMoveCount;
  ToMove toMove[100];
  // move history?
} Runtime;

void addToMove(Runtime * rt, int from, int to)  {
  printf("adding '%c' f:%i t:%i\n", rt->currentLevel.cells[from], from, to);
  rt->toMove[rt->toMoveCount].from = from;
  rt->toMove[rt->toMoveCount].to = to;
  rt->toMoveCount++;
}

int isMoving(Runtime * rt, int loc) {
  for (int i = 0; i < rt->toMoveCount; i++) {
    if (rt->toMove[i].from == loc) {
      return 1;
    }
  }
  return 0;
}

char nameToLegendKey(char * name) {
  for (int i = 0; i < pd.objectCount; i++) {
    if (strcmp(pd.objects[i].name, name) == 0) {
      return pd.objects[i].legendKey;
    }
  }
  return '\t';
}

Runtime startGame() {
  Runtime rt;
  rt.levelIndex = 0;
  rt.toMoveCount = 0;
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
   ACTION_USE,
   ACTION_NONE
  } Action;

int doResultState(Runtime * rt, Action act, int ruleIndex, int cellIndex, int cellMultipler) {
  // TODO: direction
  for (int i = 0; i < pd.rules[ruleIndex].resultStateCount; i++) {
    for (int j = 0; j < pd.rules[ruleIndex].resultStates[i].partCount; j++) {
      if (pd.rules[ruleIndex].resultStates[i].parts[j].direction != NONE) {
        if (isMoving(rt, (cellIndex + (j * cellMultipler))) != 1) {
          printf("ADDING MOVEMENT TO %s\n", pd.rules[ruleIndex].resultStates[i].parts[j].identifier);
          addToMove(rt, (cellIndex + (j * cellMultipler)), (cellIndex + (j * cellMultipler)) + cellMultipler);
        }
      }
      rt->currentLevel.cells[cellIndex + (j * cellMultipler)] = nameToLegendKey(pd.rules[ruleIndex].resultStates[i].parts[j].identifier);
    }
  }
  return 0;
}

int ruleApplies(Runtime * rt, ExecutionTime execTime, Action act, Rule * rule) {
  if (rule->executionTime != execTime) {
    return 0;
  }

  //if (rule->hasDirectionConstraint == 1) {
    // TODO: rule->directionContraint is an enum now! yay!
    /* switch (act) { */
    /* case ACTION_UP: */
    /*   if (strcmp(rule->directionConstraint, "up") != 0) { */
    /*     return 0; */
    /*   } */
    /*   break; */
    /* case ACTION_DOWN: */
    /*   if (strcmp(rule->directionConstraint, "down") != 0) { */
    /*     return 0; */
    /*   } */
    /*   break; */
    /* case ACTION_LEFT: */
    /*   if (strcmp(rule->directionConstraint, "left") != 0) { */
    /*     return 0; */
    /*   } */
    /*   break; */
    /* default: */
    /*   break; */
    /* } */
  //}
  return 1;
}

int partMatches(Runtime * rt, Action act, int cellIndex, RuleStatePart * rsp) {
  if (rt->currentLevel.cells[cellIndex] != nameToLegendKey(rsp->identifier)) {
    return 0;
  }
  printf("ids match! '%c'\n", rt->currentLevel.cells[cellIndex]);
  if (rsp->direction != NONE) {
    switch (act) {
    case ACTION_UP:
      printf("action is up direction is %i\n", rsp->direction);
      if (rsp->direction != UP) {
        return 0;
      }
      break;
    case ACTION_DOWN:
      printf("action is down direction is %i\n", rsp->direction);
      if (rsp->direction != DOWN) {
        return 0;
      }
      break;
    case ACTION_LEFT:
      printf("action is left direction is %i\n", rsp->direction);
      if (rsp->direction != LEFT) {
        return 0;
      }
    case ACTION_RIGHT:
      printf("action is right direction is %i\n", rsp->direction);
      if (rsp->direction != RIGHT) {
        return 0;
      }
      break;
    default:
      break;
    }
  } else {
      // should not be moving
      if (isMoving(rt, cellIndex) != 0) {
        return 0;
      }
    }
  return 1;
}

int cellMatches(Runtime * rt, Action act, int cellIndex, int cellMultiplier, RuleState * rs) {
  // TODO: this ignores direction and shouldn't
  int matchingCells = 0;
  for (int i = 0; i < rs->partCount; i++) {
    if (partMatches(rt, act, cellIndex + (i * cellMultiplier), &rs->parts[i]) != 1) {
      return 0;
    }
  }
  return 1;
}

int checkMatches(Runtime * rt, Action act, int ruleIndex, int cellIndex, int cellMultiplier) {
  int appliedSomething = 0;
  for (int matchIndex = 0; matchIndex < pd.rules[ruleIndex].matchStateCount; matchIndex++) {
    if (cellMatches(rt, act, cellIndex, cellMultiplier, &pd.rules[ruleIndex].matchStates[matchIndex]) == 1) {
      doResultState(rt, act, ruleIndex, cellIndex, cellMultiplier);
      appliedSomething = 1;
    }
  }
  return appliedSomething;
}

int applyRules(Runtime * rt, ExecutionTime execTime, Action act) {
  // do things like move blocks when you push them
  for (int ruleIndex = 0; ruleIndex < pd.ruleCount; ruleIndex++) {
    if (pd.rules[ruleIndex].executionTime == execTime) {
      int appliedSomething = 1;
      while (appliedSomething == 1) {
        appliedSomething = 0;
        // applies to the right
        for (int cellIndex = 0; cellIndex < rt->currentLevel.cellIndex; cellIndex++) {
          // up
          appliedSomething = appliedSomething || checkMatches(rt, act, ruleIndex, cellIndex, (rt->currentLevel.width * -1));
          //down
          appliedSomething = appliedSomething || checkMatches(rt, act, ruleIndex, cellIndex, rt->currentLevel.width);
          // left
          appliedSomething = appliedSomething || checkMatches(rt, act, ruleIndex, cellIndex, -1);
          // right
          appliedSomething = appliedSomething || checkMatches(rt, act, ruleIndex, cellIndex, 1);
        }
      }
    }
  }
  return 0;
}

void update(Runtime * rt, char * input) {
  int playerLoc = playerLocation(rt);
  int multiplier = 0;
  Action act;
  // mark player to move
  if (strcmp(input, "up") == 0) {
    multiplier = (rt->currentLevel.width * -1);
    act = ACTION_UP;
  } else if (strcmp(input, "down") == 0) {
    multiplier = rt->currentLevel.width;
    act = ACTION_DOWN;
  } else if (strcmp(input, "left") == 0) {
    multiplier = -1;
    act = ACTION_LEFT;
  } else if (strcmp(input, "right") == 0) {
    multiplier = 1;
       act = ACTION_RIGHT;
  } else if (strcmp(input, "use") == 0) {
    act = ACTION_USE;
  } else {
    act = ACTION_NONE;
  }
  addToMove(rt, playerLoc, (playerLoc + multiplier));


  // apply rule
  applyRules(rt, NORMAL, act);

  // apply marked for move
  printf("moves: %i", rt->toMoveCount);
  for (int i = rt->toMoveCount-1; i >= 0; i--) {

    printf("moving %i to %i\n", rt->toMove[i].from, rt->toMove[i].to);
    swapSquares(rt, rt->toMove[i].from, rt->toMove[i].to);
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
  } else {
    printf("Please provide a puzzlescript file\n");
    return 1;
  }


  yyin = file;
  yyparse();
  printf("pd ready: %i levels\n", pd.levelCount);
  printf("DONE!\n");

  Runtime rt = startGame();

  char input[100];
  /* while (1) { */
  /*   render(&rt); */
  /*   printf("Enter Move: "); */
  /*   gets(input); */
  /*   update(&rt, input); */
  /*   rt.toMoveCount = 0; */
  /* } */
  return 0;
}
