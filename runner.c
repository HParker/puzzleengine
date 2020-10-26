#include <stdio.h>
#include <string.h>
#include "puzzleData.h"

char * objectName(int id) {
  return pd.objects[id].name;
}

int objectIndex(Runtime * rt, int id, int loc) {
  for (int i = 0; i < rt->objectCount; i++) {
    if (id == rt->objects[i].objId && loc == rt->objects[i].loc) {
      return i;
    }
  }
  printf("err: object with id '%i' at %i was not found\n", id, loc);
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
    return (Direction)((applicationDirection + 0) % 4);
  case REL_UP:
    return (Direction)((applicationDirection + 1) % 4);
  case REL_LEFT:
    return (Direction)((applicationDirection + 2) % 4);
  case REL_DOWN:
    return (Direction)((applicationDirection + 3) % 4);
  case NONE:
    return NONE;
  default:
    printf("err: (absoluteDirection) unsupported direction (ad: %i rd: %i)\n", applicationDirection, ruleDir);
    return NONE;
  }
}

int matchesDirection(Direction ruleDir, Direction applicationDirection, Direction dir) {
  if (absoluteDirection(applicationDirection, ruleDir) == dir) {
    return 1;
  } else {
    return 0;
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
    if (strcasecmp(pd.legend[i].key, key) == 0) {
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
    if (pd.legend[i].key[0] == key && strlen(pd.legend[i].key) == 1) {
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
      if (pd.legend[i].objectValues[j].id == id && strlen(pd.legend[i].key) == 1 && pd.legend[i].objectCount == 1) {
        return pd.legend[i].key[0];
      }
    }
  }
  printf("err: no key found %i\n", id);
  return '!';
}

int objectLayer(int objId) {
  for (int i = 0; i < pd.layerCount; i++) {
    for (int j = 0; j < pd.layers[i].width; j++) {
      if (pd.layers[i].objectIds[j] == objId) {
        return i;
      }
    }
  }
  printf("err: layer not found for objid: '%i'\n", objId);
  return -1;
}

char charForLoc(Runtime * rt, int loc) {
  int maxHeight = -1;
  int id = -1;
  int currentHeight;
  for (int i = 0; i < rt->objectCount; i++) {
    currentHeight = objectLayer(rt->objects[i].objId);
    if (rt->objects[i].loc == loc && currentHeight > maxHeight) {
      maxHeight = currentHeight;
      id = rt->objects[i].objId;
    }
  }
  return objIdToChar(id);
}

void fillBackground(Runtime * rt) {
  for (int i = 0; i < rt->height * rt->width; i++) {
    rt->objects[rt->objectCount].objId = legendId("Background");
    rt->objects[rt->objectCount].loc = i;
    rt->objectCount++;
  }
}

void loadCell(Runtime * rt, char cell, int loc) {
  // TODO: I think this can ignore background cells I guess...
  for (int i = 0; i < pd.legendCount; i++) {
    if (pd.legend[i].key[0] == cell && strlen(pd.legend[i].key) == 1) {
      if (pd.legend[i].objectRelation != LEGEND_RELATION_OR) {
        for (int j = 0; j < pd.legend[i].objectCount; j++) {
          if (pd.legend[i].objectValues[j].id != legendId("Background")) {
            rt->objects[rt->objectCount].objId = pd.legend[i].objectValues[j].id;
            rt->objects[rt->objectCount].loc = loc;
            rt->objectCount++;
          }
        }
      }
    }
  }
}

void loadLevel(Runtime * rt, Level * level) {
  // TODO: height and width
  rt->height = level->height;
  rt->width = level->width;

  fillBackground(rt);

  for (int i = 0; i < level->cellIndex; i++) {
    loadCell(rt, level->cells[i], i);
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
    map[i] = charForLoc(rt, i);
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
    if (rt->objects[i].objId == legendId("Player")) {
      return rt->objects[i].loc;
    }
  }
  return -1;
}

int layerIncludes(int layerId, int objId) {
  for (int i = 0; i < pd.layers[layerId].width; i++) {
    if (pd.layers[layerId].objectIds[i] == objId) {
      return 1;
    }
  }
  return 0;
}

int isMovable(Runtime * rt, int loc, int layerIndex) {
  int hasCollidable = 0;
  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].loc == loc && layerIncludes(layerIndex, rt->objects[i].objId)) {
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

int legendContains(int legendId, int objId) {
  for (int i = 0; i < pd.legend[legendId].objectCount; i++) {
    if (pd.legend[legendId].objectValues[i].id == objId) {
      return 1;
    }
  }
  return 0;
}

int doResultState(Runtime * rt, Direction applicationDirection, int ruleIndex, int loc) {
  for (int i = 0; i < pd.rules[ruleIndex].resultStateCount; i++) {
    for (int j = 0; j < pd.rules[ruleIndex].resultStates[i].partCount; j++) {
      if (pd.rules[ruleIndex].resultStates[i].parts[j].direction != NONE) {
        int source = (loc + j * locDeltaFor(rt, applicationDirection, pd.rules[ruleIndex].resultStates[i].parts[j].direction));
        addToMove(rt, applicationDirection, objectIndex(rt, pd.rules[ruleIndex].resultStates[i].parts[j].legendId, source), pd.rules[ruleIndex].resultStates[i].parts[j].direction);
      }

      for (int k = 0; k < rt->objectCount; k++) {
        if (legendContains(pd.rules[ruleIndex].matchStates[i].parts[j].legendId, rt->objects[k].objId) && rt->objects[k].loc == loc + (j * locDeltaFor(rt, applicationDirection, pd.rules[ruleIndex].resultStates[i].parts[j].direction))) {
          rt->objects[k].objId = pd.rules[ruleIndex].resultStates[i].parts[j].legendId;
        }
      }
    }
  }
  return 0;
}

int partMatches(Runtime * rt, Direction applicationDirection, int loc, RuleStatePart * rsp) {
  for (int i = 0; i < rt->objectCount; i++) {
    if (legendContains(rsp->legendId, rt->objects[i].objId) && rt->objects[i].loc == loc && matchesDirection(rsp->direction, applicationDirection, directionMoving(rt, i))){
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
      printf("Rule matched at index: '%i'\n", ruleIndex);
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
        }
        totalApplications++;
      }
      if (totalApplications >= 6) {
        printf("Warning: performed max rule executions\n");
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
      int movingToLoc = rt->objects[rt->toMove[i].objIndex].loc + locDeltaFor(rt, rt->toMove[i].direction, rt->toMove[i].direction);
      int layerIndex = objectLayer(rt->objects[rt->toMove[i].objIndex].objId);
      if (moveApplied[i] == 0 && isMovable(rt, movingToLoc, layerIndex)) {
        rt->objects[rt->toMove[i].objIndex].loc += locDeltaFor(rt, rt->toMove[i].direction, rt->toMove[i].direction);
        // TODO: fix this locDeltaFor too ^
        moveApplied[i] = 1;
        somethingApplied = 1;
      }
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
  moveObjects(rt);
  rt->toMoveCount = 0;

  // apply late rules
  applyRules(rt, LATE, dir);
}

int verifyAll(Runtime * rt, int thing, int container) {
  int satisfied = 1;
  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].objId == thing) {
      satisfied = 0;
      for (int j = 0; j < rt->objectCount; j++) {
        if (rt->objects[j].objId == container && rt->objects[i].loc == rt->objects[j].loc) {
          satisfied = 1;
        }
      }
    }
    if (satisfied == 0) {
      return 0;
    }
  }
  return satisfied;
}

int checkWinCondition(Runtime * rt, int winConditionIndex) {
  switch (pd.winConditions[winConditionIndex].winQualifier) {
  case ALL:
    return verifyAll(rt, pd.winConditions[winConditionIndex].winIdentifier, pd.winConditions[winConditionIndex].onIndentifier);
  default:
    printf("err: unsupported win condition '%i'\n", pd.winConditions[winConditionIndex].winQualifier);
    return 0;
  }
}

int checkWinConditions(Runtime * rt) {
  int satisfied = 0;
  for (int i = 0; i < pd.winConditionCount; i++) {
    satisfied = checkWinCondition(rt, i);
  }
  return satisfied;
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
    if (checkWinConditions(&rt) == 1) {
      printf("you won!\n");

    }
    render(&rt);
    printf("Enter Move: ");
    gets(input);
    update(&rt, input);
    rt.toMoveCount = 0;
  }
  return 0;
}
