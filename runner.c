#include <stdio.h>
#include <string.h>
#include "puzzle.tab.c"

int objectIndex(Runtime * rt, int id, int loc) {
  for (int i = 0; i < rt->objectCount; i++) {
    if (id == rt->objects[i].objId) {
      return i;
    }
  }
  printf("NEVER GET HERE\n");
  return -1;
}

// TODO: is moving and is moving direction should split
int isMoving(Runtime * rt, int objIndex, Direction direction) {
  for (int i = 0; i < rt->toMoveCount; i++) {
    if (rt->toMove[i].objIndex == objIndex && rt->toMove[i].direction == direction) {
      return 1;
    }
  }
  return 0;
}

Direction directionMoving(Runtime * rt, int objIndex) {
  for (int i = 0; i < rt->toMoveCount; i++) {
    if (rt->toMove[i].objIndex == objIndex) {
      return rt->toMove[i].direction;
    }
  }
  return NONE;
}

Direction absoluteDirection(Direction applicationDirection, Direction ruleDir) {
  switch (ruleDir) {
  case UP:
    return ruleDir;
  case DOWN:
    return ruleDir;
  case LEFT:
    return ruleDir;
  case RIGHT:
    return ruleDir;
  case REL_RIGHT:
    printf("rel_right to %i -> %i\n", applicationDirection, ((applicationDirection + 0) % 4));
    return (Direction)((applicationDirection + 0) % 4);
  case REL_UP:
    printf("rel_up to %i -> %i\n", applicationDirection, ((applicationDirection + 1) % 4));
    return (Direction)((applicationDirection + 1) % 4);
  case REL_LEFT:
    printf("rel_left to %i -> %i\n", applicationDirection, ((applicationDirection + 2) % 4));
    return (Direction)((applicationDirection + 2) % 4);
  case REL_DOWN:
    printf("rel_down to %i -> %i\n", applicationDirection, ((applicationDirection + 3) % 4));
    return (Direction)((applicationDirection + 3) % 4);
  case NONE:
    return applicationDirection;
  default:
    printf("err: (absoluteDirection) unsupported direction (ad: %i rd: %i)\n", applicationDirection, ruleDir);
    return NONE;
  }
}

void addToMove(Runtime * rt, Direction applicationDirection, int objIndex, Direction direction) {
  for (int i = 0; i < rt->toMoveCount; i++) {
    if (rt->toMove[i].objIndex == objIndex) {
      // skip if you already have this object index
      return;
    }
  }
  rt->toMove[rt->toMoveCount].objIndex = objIndex;
  rt->toMove[rt->toMoveCount].direction = absoluteDirection(applicationDirection, direction);
  rt->toMoveCount++;
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
        return pd.legend[i].objectValues[0].id;
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
        return pd.legend[i].objectValues[0].id;
      }
    }
  }
  printf("err: no legend for '%c'\n", key);
  return -1;
}

char objIdToChar(int id) {
  for (int i = 0; i < pd.legendCount; i++) {
    for (int j = 0; j < pd.legend[i].objectCount; j++) {
      if (pd.legend[i].objectValues[j].id == id) {
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
      printf("found '%s'\n", objectName(rt->objects[i].objId));
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

int ruleApplies(Runtime * rt, Rule * rule, ExecutionTime execTime) {
  // TODO: this should check global direction constraints
  if (rule->executionTime != execTime) {
    return 0;
  }
  return 1;
}

int locDeltaFor(Runtime * rt, Direction applicationDirection, Direction dir) {
  switch (absoluteDirection(applicationDirection, dir)) {
  case UP:
    return (rt->width * -1);
  case DOWN:
    return rt->width;
  case LEFT:
    return -1;
  case RIGHT:
    return 1;
  case NONE:
    printf("IN THE NONE locdeltafor\n");
    return 0;
  default:
    return 0;
  }
}

int doResultState(Runtime * rt, Direction applicationDirection, int ruleIndex, int loc) {
  for (int i = 0; i < pd.rules[ruleIndex].resultStateCount; i++) {
    for (int j = 0; j < pd.rules[ruleIndex].resultStates[i].partCount; j++) {
      if (pd.rules[ruleIndex].resultStates[i].parts[j].direction != NONE) {
        int source = (loc + locDeltaFor(rt, applicationDirection, pd.rules[ruleIndex].resultStates[i].parts[j].direction));
        printf("Adding Movement\n");
        addToMove(rt, applicationDirection, objectIndex(rt, pd.rules[ruleIndex].resultStates[i].parts[j].legendId, source), pd.rules[ruleIndex].resultStates[i].parts[j].direction);
      }

      for (int k = 0; k < rt->objectCount; k++) {
        if (pd.rules[ruleIndex].matchStates[i].parts[j].legendId == rt->objects[k].objId && rt->objects[k].loc == loc + (j * locDeltaFor(rt, applicationDirection, pd.rules[ruleIndex].resultStates[i].parts[j].direction))) {
          printf("Doing swap (loc: %i modifier: %i) (before: %i after: %i)\n", rt->objects[k].loc, locDeltaFor(rt, applicationDirection, pd.rules[ruleIndex].resultStates[i].parts[j].direction), rt->objects[k].objId, pd.rules[ruleIndex].resultStates[i].parts[j].legendId);
          rt->objects[k].objId = pd.rules[ruleIndex].resultStates[i].parts[j].legendId;
        }
      }
    }
  }
  return 0;
}

int matchesDirection(Direction ruleDir, Direction applicationDirection, Direction dir) {
  switch (ruleDir) {
  case UP:
    return (dir == UP);
  case DOWN:
    return (dir == DOWN);
  case LEFT:
    return (dir == LEFT);
  case RIGHT:
    return (dir == RIGHT);
  case HORIZONTAL:
    return (dir == LEFT || dir == RIGHT);
  case VIRTICAL:
    return (dir == UP || dir == DOWN);
  case REL_RIGHT:
    printf("ad: %i, dir: %i\n", applicationDirection, dir);
    return ((applicationDirection + 0) % 4 == dir);
  case REL_UP:
    return ((applicationDirection + 1) % 4 == dir);
  case REL_LEFT:
    return ((applicationDirection + 2) % 4 == dir);
  case REL_DOWN:
    return ((applicationDirection + 3) % 4 == dir);
  case NONE:
    return (dir == NONE);
  default:
    printf("err: (matchesDirection) unsupported direction (%i)\n", ruleDir);
    return -1;
  }
}

int partMatches(Runtime * rt, Direction applicationDirection, int loc, RuleStatePart * rsp) {
  for (int i = 0; i < rt->objectCount; i++) {

    if (rsp->legendId == rt->objects[i].objId && rt->objects[i].loc == loc && matchesDirection(rsp->direction, applicationDirection, directionMoving(rt, i))){
      printf("(loc %i):- part '%i' matched\n", loc, rsp->legendId);
      return 1;
    }
  }
  return 0;
}

int cellsMatch(Runtime * rt, int loc, Direction applicationDirection, RuleState * rs) {
  for (int i = 0; i < rs->partCount; i++) {
    if (partMatches(rt, applicationDirection, loc + i * locDeltaFor(rt, applicationDirection, applicationDirection), &rs->parts[i]) != 1) {
      // TODO: this locDeltafor call is bad...
      return 0;
      }
  }
  return 1;
}

int checkMatches(Runtime * rt, Direction applicationDirection, int ruleIndex, int loc) {
  int appliedSomething = 0;
  for (int matchIndex = 0; matchIndex < pd.rules[ruleIndex].matchStateCount; matchIndex++) {
    if (cellsMatch(rt, loc, applicationDirection, &pd.rules[ruleIndex].matchStates[matchIndex]) == 1) {
      printf("found a match rule Index: %i\n", ruleIndex);
      doResultState(rt, applicationDirection, ruleIndex, loc);
      appliedSomething = 1;
    }
  }
  return appliedSomething;
}

int applyRules(Runtime * rt, ExecutionTime execTime, Direction act) {
  for (int ruleIndex = 0; ruleIndex < pd.ruleCount; ruleIndex++) {
    if (ruleApplies(rt, &pd.rules[ruleIndex], execTime)) {
      int totalApplications = 0;
      int appliedSomething = 1;
      while (appliedSomething == 1 && totalApplications < 6) {
        appliedSomething = 0;
        // applies to the right
        for (int cellIndex = 0; cellIndex < rt->height * rt->width; cellIndex++) {
          // up
          appliedSomething = appliedSomething || checkMatches(rt, UP, ruleIndex, cellIndex);
          //down
          appliedSomething = appliedSomething || checkMatches(rt, DOWN, ruleIndex, cellIndex);
          // left
          appliedSomething = appliedSomething || checkMatches(rt, LEFT, ruleIndex, cellIndex);
          // right
          appliedSomething = appliedSomething || checkMatches(rt, RIGHT, ruleIndex, cellIndex);
          totalApplications++;
        }
      }
      if (totalApplications >= 6) {
        printf("performed max rule executions\n");
      }
    }
  }
  return 0;
}

void moveObjects(Runtime * rt) {
  int moveApplied[rt->toMoveCount];
  for (int i = 0; i < rt->toMoveCount; i++) {
    moveApplied[i] = 0;
  }

  int somethingApplied = 1;
  while (somethingApplied == 1) {
    somethingApplied = 0;

    for (int i = 0; i < rt->toMoveCount; i++) {
      printf("before %i\n", rt->objects[rt->toMove[i].objIndex].loc);
      if (isMovable(rt, rt->objects[rt->toMove[i].objIndex].loc + locDeltaFor(rt, rt->toMove[i].direction, rt->toMove[i].direction)) && moveApplied[i] == 0) {
        rt->objects[rt->toMove[i].objIndex].loc += locDeltaFor(rt, rt->toMove[i].direction, rt->toMove[i].direction);
        // TODO: fix this locDeltaFor too ^
        moveApplied[i] = 1;
        somethingApplied = 1;
      }

      printf("after %i\n", rt->objects[rt->toMove[i].objIndex].loc);
    }
  }
  rt->toMoveCount = 0;
}

void update(Runtime * rt, char * input) {
  Direction dir;
  // mark player to move
  if (input[0] == 'u' || input[0] == 'U') {
    dir = UP;
  } else if (input[0] == 'd' || input[0] == 'D') {
    dir = DOWN;
  } else if (input[0] == 'l' || input[0] == 'L') {
    dir = LEFT;
  } else if (input[0] == 'r' || input[0] == 'R') {
    dir = RIGHT;
  } else if (input[0] == 'u' || input[0] == 'U') {
    dir = USE;
  } else {
    dir = NONE;
  }

  addToMove(rt, NONE, objectIndex(rt, legendId("Player"), playerLocation(rt)), dir);

  // apply rule
  applyRules(rt, NORMAL, dir);

  // apply marked for move
  printf("moves: %i\n", rt->toMoveCount);
  for (int i = 0; i < rt->toMoveCount; i++) {
    printf("need to move '%s' (id: %i) dir %i\n", objectName(rt->objects[rt->toMove[i].objIndex].objId), rt->toMove[i].objIndex, rt->toMove[i].direction);
  }
  moveObjects(rt);
  rt->toMoveCount = 0;

  // apply late rules
  applyRules(rt, LATE, dir);
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
