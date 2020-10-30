#include <stdio.h>
#include <string.h>
#include "puzzleData.h"

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
  case USE:
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
  return objectGlyph(id);
}

void fillBackground(Runtime * rt) {
  for (int i = 0; i < rt->height * rt->width; i++) {
    rt->objects[rt->objectCount].objId = legendId("Background");
    rt->objects[rt->objectCount].loc = i;
    rt->objectCount++;
  }
}

void loadCell(Runtime * rt, char cell, int loc) {
  int id = legendIdForGlyph(cell);
  int count = legendObjectCount(id);
  for (int i = 0; i < count; i++) {
    rt->objects[rt->objectCount].objId = legendObject(id, i);
    rt->objects[rt->objectCount].loc = loc;
    rt->objectCount++;
  }
}

void loadLevel(Runtime * rt) {
  rt->height = levelHeight(rt->levelIndex);
  rt->width = levelWidth(rt->levelIndex);

  rt->toMoveCount = 0;
  rt->objectCount = 0;

  fillBackground(rt);

  int count = levelCellCount(rt->levelIndex);
  for (int i = 0; i < count; i++) {
    loadCell(rt, levelCell(rt->levelIndex, i), i);
  }
}

void nextLevel(Runtime * rt) {
  rt->levelIndex++;
  loadLevel(rt);
}

Runtime startGame(FILE * file) {
  Runtime rt;
  rt.pd = parsePuzzle(file);
  rt.levelIndex = 0;
  rt.gameWon = 0;
  rt.historyCount = 0;
  loadLevel(&rt);
  return rt;
}

void render(Runtime * rt) {
  // build
  int count = levelCellCount(rt->levelIndex);
  char map[count];
  for (int i = 0; i < count; i++) {
    map[i] = '.';
    map[i] = charForLoc(rt, i);
  }

  // draw
  for (int i = 0; i < count; i++) {
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

int ruleApplies(Runtime * rt, int ruleIndex, ExecutionTime execTime) {
  // TODO: this should check global direction constraints
  if (rule(ruleIndex)->executionTime != execTime) {
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
  for (int i = 0; i < rule(ruleIndex)->resultStateCount; i++) {
    for (int j = 0; j < rule(ruleIndex)->resultStates[i].partCount; j++) {
      if (rule(ruleIndex)->resultStates[i].parts[j].direction != NONE) {
        int source = (loc + j * locDeltaFor(rt, applicationDirection, rule(ruleIndex)->resultStates[i].parts[j].direction));
        addToMove(rt, applicationDirection, objectIndex(rt, rule(ruleIndex)->resultStates[i].parts[j].legendId, source), rule(ruleIndex)->resultStates[i].parts[j].direction);
      }

      for (int k = 0; k < rt->objectCount; k++) {
        if (legendContains(rule(ruleIndex)->matchStates[i].parts[j].legendId, rt->objects[k].objId) && rt->objects[k].loc == loc + (j * locDeltaFor(rt, applicationDirection, rule(ruleIndex)->resultStates[i].parts[j].direction))) {
          rt->objects[k].objId = rule(ruleIndex)->resultStates[i].parts[j].legendId;
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
  for (int matchIndex = 0; matchIndex < rule(ruleIndex)->matchStateCount; matchIndex++) {
    if (cellsMatch(rt, loc, applicationDirection, &rule(ruleIndex)->matchStates[matchIndex]) == 1) {
      doResultState(rt, applicationDirection, ruleIndex, loc);
      appliedSomething = 1;
    }
  }
  return appliedSomething;
}

int applyRules(Runtime * rt, ExecutionTime execTime, Direction act) {
  int count = ruleCount();
  for (int ruleIndex = 0; ruleIndex < count; ruleIndex++) {
    if (ruleApplies(rt, ruleIndex, execTime)) {
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

void printHistory(Runtime * rt) {
  char * directionNames[] = {
                             "RIGHT",
                             "UP",
                             "LEFT",
                             "DOWN",
                             "HORIZONTAL",
                             "VIRTICAL",
                             "REL_UP",
                             "REL_DOWN",
                             "REL_LEFT",
                             "REL_RIGHT",
                             "USE",
                             "NONE"
  };

  for (int i = 0; i < rt->historyCount; i++) {
    printf("%s\n", directionNames[rt->history[i]]);
  }
}

Direction handleInput(Runtime * rt, char * input) {
  // TODO: doesn't need rt
  if (input[0] == 'u' || input[0] == 'U') {
    return UP;
  } else if (input[0] == 'd' || input[0] == 'D') {
    return DOWN;
  } else if (input[0] == 'l' || input[0] == 'L') {
    return LEFT;
  } else if (input[0] == 'r' || input[0] == 'R') {
    return RIGHT;
  } else if (input[0] == 'x' || input[0] == 'X') {
    return USE;
  } else {
    return NONE;
  }
}

void setLevel(Runtime * rt) {
  if (checkWinConditions(rt) == 1) {
    if (rt->levelIndex < levelCount() - 1) {
      nextLevel(rt);
    } else {
      if (rt->pd->debug == 1) {
        printHistory(rt);
      }
      rt->gameWon = 1;
    }
  }
}

void recordInput(Runtime * rt, Direction dir) {
  rt->history[rt->historyCount] = dir;
  rt->historyCount++;
}

void update(Runtime * rt, Direction dir) {
  recordInput(rt, dir);

  addToMove(rt, NONE, objectIndex(rt, legendId("Player"), playerLocation(rt)), dir);

  // apply rules
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
  switch (winCondition(winConditionIndex)->winQualifier) {
  case ALL:
    return verifyAll(rt, winCondition(winConditionIndex)->winIdentifier, winCondition(winConditionIndex)->onIndentifier);
  default:
    printf("err: unsupported win condition '%i'\n", winCondition(winConditionIndex)->winQualifier);
    return 0;
  }
}

int checkWinConditions(Runtime * rt) {
  int satisfied = 0;
  int count = winConditionCount();
  for (int i = 0; i < count; i++) {
    satisfied = checkWinCondition(rt, i);
  }
  return satisfied;
}
