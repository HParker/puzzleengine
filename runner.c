#include <stdio.h>
#include <string.h>
#include "puzzle.tab.c"

// TODO objIndex && direction only
void addToMove(Runtime * rt, int objId, int source, int destination, Direction direction)  {
  printf("addToMove '%s' %i: (%i -> %i)\n", objectName(objId), direction, source, destination);

  rt->toMove[rt->toMoveCount].objId = objId;
  rt->toMove[rt->toMoveCount].sourceLoc = source;
  rt->toMove[rt->toMoveCount].destinationLoc = destination;
  rt->toMove[rt->toMoveCount].direction = direction;
  rt->toMoveCount++;
}

int isMoving(Runtime * rt, int objId, int sourceLoc, Direction direction) {
  for (int i = 0; i < rt->toMoveCount; i++) {
    if (rt->toMove[i].sourceLoc == sourceLoc && rt->toMove[i].objId == objId && direction == rt->toMove[i].direction) {
      return 1;
    }
  }
  return 0;
}

Direction directionMoving(Runtime * rt, int objId, int sourceLoc) {
  for (int i = 0; i < rt->toMoveCount; i++) {
    if (rt->toMove[i].sourceLoc == sourceLoc && rt->toMove[i].objId == objId) {
      return rt->toMove[i].direction;
    }
  }
  return NONE;
}

int locDeltaFor(Runtime * rt, Direction dir) {
  switch (dir) {
  case UP:
    return (rt->width * -1);
  case DOWN:
    return rt->width;
  case LEFT:
    return -1;
  case RIGHT:
    return 1;
  default:
    return 0;
  }
}

char nameToLegendKey(char * name) {
  for (int i = 0; i < pd.objectCount; i++) {
    if (strcmp(pd.objects[i].name, name) == 0) {
      return pd.objects[i].legendKey;
    }
  }
  return '%'; // TODO: what do I put here?
}

int keyToObjId(char * key) {
  for (int i = 0; i < pd.legendCount; i++) {
    if (strcmp(pd.legend[i].key, key) == 0) {
      if (pd.legend[i].objectCount > 1) {
        // legend key with multiple objects
        // if this is an `and`, we have to do them all,
        // if not, it is invalid
        printf("err: multi object key '%s'\n", key);
      } else {
        return pd.legend[i].objectIndex[0];
      }
    }
  }
  printf("err: no legend for '%s'\n", key);
  return -1;
}

int keyCharToObjId(char key) {
  for (int i = 0; i < pd.legendCount; i++) {
    if (pd.legend[i].key[0] == key) {
      if (pd.legend[i].objectCount > 1) {
        // legend key with multiple objects
        // if this is an `and`, we have to do them all,
        // if not, it is invalid
        printf("err: multi object key '%c'\n", key);
      } else {
        return pd.legend[i].objectIndex[0];
      }
    }
  }
  printf("err: no legend for '%c'\n", key);
  return -1;
}

char objIdToChar(int id) {
  for (int i = 0; i < pd.legendCount; i++) {
    for (int j = 0; j < pd.legend[i].objectCount; j++) {
      if (pd.legend[i].objectIndex[j] == id) {
        return pd.legend[i].key[0];
      }
    }
  }
  printf("err: no key found\n");
  return '!';
}

void loadLevel(Runtime * rt, Level * level) {
  // TODO: height and width
  rt->height = level->height;
  rt->width = level->width;
  for (int i = 0; i < level->cellIndex; i++) {
    if (keyCharToObjId(level->cells[i]) != -1) {
      rt->objects[rt->objectCount].objId = keyCharToObjId(level->cells[i]);
      rt->objects[rt->objectCount].loc = i;
      rt->objectCount++;
    }
  }
  printf("Finished Loading %i objects\n", rt->objectCount);
}

Runtime startGame() {
  Runtime rt;
  rt.levelIndex = 0;
  rt.toMoveCount = 0;
  rt.objectCount = 0;

  loadLevel(&rt, &pd.levels[0]);
  return rt;
}

void render(Runtime * rt) {
  // build
  int mapIndex = 0;
  char map[rt->height * rt->width];
  for (int i = 0; i < rt->height * rt->width; i++) {
    map[i] = '.';
    for (int j = 0; j < rt->objectCount; j++) {
      if (rt->objects[j].loc == i && rt->objects[j].objId != 0) {
        map[i] = objIdToChar(rt->objects[j].objId);
      }
    }
    // TODO: layers
  }

  // draw
  for (int i = 0; i < rt->height * rt->width; i++) {
    printf("%c", map[i]);
    if ((i + 1) % (rt->width) == 0) {
      printf("\n");
    }
  }
}

int playerLocation(Runtime * rt) {
  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].objId == objectId("Player")) {
      return rt->objects[i].loc;
    }
  }
  return -1;
}

int isMovable(Runtime * rt, int loc) {
  int hasCollidable = 0;
  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].loc == loc && strcmp(objectName(rt->objects[i].objId), "Background") != 0) {
      hasCollidable = 1;
    }
  }
  // TODO: this is dumb
  if (hasCollidable == 1) {
    return 0;
  } else {
    return 1;
  }
}

int doResultState(Runtime * rt, Direction applicationDirection, int ruleIndex, int loc, int cellMultipler) {
  // TODO: combine these two result state application blocks
  for (int i = 0; i < pd.rules[ruleIndex].resultStateCount; i++) {
    for (int j = 0; j < pd.rules[ruleIndex].resultStates[i].partCount; j++) {
      for (int k = 0; k < rt->objectCount; k++) {
        if (objectId(pd.rules[ruleIndex].resultStates[i].parts[j].identifier) == rt->objects[k].objId) {
          rt->objects[k].loc = (loc + (j * cellMultipler));

        }
      }
    }
  }

  for (int i = 0; i < pd.rules[ruleIndex].resultStateCount; i++) {
    for (int j = 0; j < pd.rules[ruleIndex].resultStates[i].partCount; j++) {
      if (pd.rules[ruleIndex].resultStates[i].parts[j].direction != NONE) {
        int source = (loc + (j * cellMultipler));
        int destination = source + locDeltaFor(rt, pd.rules[ruleIndex].resultStates[i].parts[j].direction);
        if (isMoving(rt, objectId(pd.rules[ruleIndex].resultStates[i].parts[j].identifier), source, pd.rules[ruleIndex].resultStates[i].parts[j].direction) != 1) {
          addToMove(rt, objectId(pd.rules[ruleIndex].resultStates[i].parts[j].identifier), source, destination, pd.rules[ruleIndex].resultStates[i].parts[j].direction);
        } else {
        }
      } else {
        printf("err: result direction isn't set. this should never happen\n");
      }

      /* rt->currentLevel.cells[cellIndex + (j * cellMultipler)] = nameToLegendKey(pd.rules[ruleIndex].resultStates[i].parts[j].identifier); */
    }
  }
  return 0;
}

int ruleApplies(Runtime * rt, Rule * rule, ExecutionTime execTime) {
  if (rule->executionTime != execTime) {
    return 0;
  }
  return 1;
}

int partMatches(Runtime * rt, Direction applicationDirection, int loc, int cellMultiplier, RuleStatePart * rsp) {
  for (int i = 0; i < rt->objectCount; i++) {
    if (objectId(rsp->identifier) == rt->objects[i].objId && rt->objects[i].loc == loc) {
      if (directionMoving(rt, rt->objects[i].objId, loc) == rsp->direction) {
        printf(":- part '%s' matched\n", rsp->identifier);
        return 1;
      }
    }
  }
  return 0;
}

int cellsMatch(Runtime * rt, Direction applicationDirection, int loc, int cellMultiplier, RuleState * rs) {
  int matchingCells = 0;
  for (int i = 0; i < rs->partCount; i++) {
    if ((i * cellMultiplier) >= 0) {
      if (partMatches(rt, applicationDirection, loc + (i * cellMultiplier), cellMultiplier, &rs->parts[i]) == 1) {
        matchingCells++;
      }
    }
  }
  // TODO: this is silly
  if (matchingCells == rs->partCount) {
    return 1;
  } else {
    return 0;
  }
}

int checkMatches(Runtime * rt, Direction applicationDirection, int ruleIndex, int loc, int cellMultiplier) {
  int appliedSomething = 0;
  for (int matchIndex = 0; matchIndex < pd.rules[ruleIndex].matchStateCount; matchIndex++) {
    if (cellsMatch(rt, applicationDirection, loc, cellMultiplier, &pd.rules[ruleIndex].matchStates[matchIndex]) == 1) {
      printf("found a match rule Index: %i\n", ruleIndex);
      doResultState(rt, applicationDirection, ruleIndex, loc, cellMultiplier);
      appliedSomething = 1;
    }
  }
  return appliedSomething;
}

int applyRules(Runtime * rt, ExecutionTime execTime, Direction act) {
  // do things like move blocks when you push them
  for (int ruleIndex = 0; ruleIndex < pd.ruleCount; ruleIndex++) {
    if (ruleApplies(rt, &pd.rules[ruleIndex], execTime)) {
      int totalApplications = 0;
      int appliedSomething = 1;
      while (appliedSomething == 1 && totalApplications < 5) {
        appliedSomething = 0;
        // applies to the right
        for (int cellIndex = 0; cellIndex < rt->height * rt->width; cellIndex++) {
          // up
          appliedSomething = appliedSomething || checkMatches(rt, UP, ruleIndex, cellIndex, (rt->width * -1));
          //down
          appliedSomething = appliedSomething || checkMatches(rt, DOWN, ruleIndex, cellIndex, rt->width);
          // left
          appliedSomething = appliedSomething || checkMatches(rt, LEFT, ruleIndex, cellIndex, -1);
          // right
          appliedSomething = appliedSomething || checkMatches(rt, RIGHT, ruleIndex, cellIndex, 1);
          totalApplications++;
        }
      }
    }
  }
  return 0;
}

void moveObjects(Runtime * rt) {
  for (int i = 0; i < rt->toMoveCount; i++) {
    for (int j = 0; j < rt->objectCount; j++) {
      if (rt->objects[j].objId == rt->toMove[i].objId) {
        rt->objects[j].loc = rt->toMove[i].destinationLoc;
      }
    }
  }
}

void update(Runtime * rt, char * input) {
  int playerLoc = playerLocation(rt); // we can probably apply around this?
  int multiplier = 0;
  Direction act;
  // mark player to move
  if (input[0] == 'u' || input[0] == 'U') {
    multiplier = (rt->width * -1);
    act = UP;
  } else if (input[0] == 'd' || input[0] == 'D') {
    multiplier = rt->width;
    act = DOWN;
  } else if (input[0] == 'l' || input[0] == 'L') {
    multiplier = -1;
    act = LEFT;
  } else if (input[0] == 'r' || input[0] == 'R') {
    multiplier = 1;
       act = RIGHT;
  } else if (input[0] == 'u' || input[0] == 'U') {
    multiplier = rt->width;
    act = USE;
  } else {
    act = NONE;
  }
  addToMove(rt, objectId("Player"), playerLocation(rt), (playerLoc + multiplier), act);

  // apply rule
  applyRules(rt, NORMAL, act);

  // apply marked for move
  printf("moves: %i\n", rt->toMoveCount);
  for (int i = 0; i < rt->toMoveCount; i++) {
    printf("need to move '%s' (id: %i) from %i to %i\n", objectName(rt->toMove[i].objId), rt->toMove[i].objId, rt->toMove[i].sourceLoc, rt->toMove[i].destinationLoc);
  }
  moveObjects(rt);
  rt->toMoveCount = 0;

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

  Runtime rt = startGame();
  printf("game ready: %i levels\n", pd.levelCount);

  char input[100];
  while (1) {
    render(&rt);
    printf("Enter Move: ");
    gets(input);
    update(&rt, input);
    rt.toMoveCount = 0;
  }
  return 0;
}
