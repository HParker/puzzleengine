#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curses.h>
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
  Direction absoluteDir = absoluteDirection(applicationDirection, ruleDir);
  if (absoluteDir == dir) {
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
  /* if (rt->toMoveCount + 1 == rt->toMoveCapacity) { */
  /*   rt->toMoveCapacity += 50; */
  /*   printf("tomove realloc\n"); */
  /*   rt->toMove = realloc(rt->toMove, sizeof(ToMove) * rt->toMoveCapacity); */
  /* } */

  rt->toMove[rt->toMoveCount].objIndex = objIndex;
  // TODO: having this absolute dir here seems wrong, we should already convert it before trying to do the move.
  Direction absoluteDir = absoluteDirection(applicationDirection, direction);
  rt->toMove[rt->toMoveCount].direction = absoluteDir;
  rt->toMoveCount++;
}

void addObj(Runtime * rt, int objId, int loc) {
  if (rt->objectCount + 1 == rt->objectCapacity) {
    printf("object realloc\n");
    rt->objectCapacity += 50;
    rt->objects = realloc(rt->objects, sizeof(Direction) * rt->objectCapacity);
  }

  rt->objects[rt->objectCount].objId = objId;
  rt->objects[rt->objectCount].loc = loc;
  rt->objectCount++;
}

void fillBackground(Runtime * rt) {
  // TODO: objectId here
  int backgroundId = legendId("Background");
  int count = legendObjectCount(backgroundId); // always 1

  for (int i = 0; i < rt->height * rt->width; i++) {
    for (int j = 0; j < count; j++) {
      addObj(rt, legendObjectId(backgroundId, j), i);
    }
  }
}

void loadCell(Runtime * rt, char cell, int loc) {
  int id = legendIdForGlyph(cell);

  int backgroundId = legendId("Background");
  int count = legendObjectCount(id);
  for (int i = 0; i < count; i++) {
    if (id != backgroundId) {
      int objId = legendObjectId(legendObjectId(id, i), 0);
      addObj(rt, objId, loc);
    }
  }
}

void loadLevel(Runtime * rt) {
  rt->levelType = levelType(rt->levelIndex);
  if (rt->levelType == SQUARES) {
    rt->height = levelHeight(rt->levelIndex);
    rt->width = levelWidth(rt->levelIndex);
    rt->toMoveCount = 0;
    rt->objectCount = 0;

    fillBackground(rt);

    int count = levelCellCount(rt->levelIndex);
    for (int i = 0; i < count; i++) {
      loadCell(rt, levelCell(rt->levelIndex, i), i);
    }
  } else {
    // Message style level
    // Not sure we have to do anything
    // Since we can just render the level from the message stored in the puzzle data;
  }
}

void nextLevel(Runtime * rt) {
  rt->levelIndex++;
  loadLevel(rt);
}

void initGame(Runtime * rt) {
  rt->levelIndex = 0;
  rt->gameWon = 0;
  rt->toMoveCount = 0;
  rt->historyCount = 0;

  rt->objectCount = 0;
  rt->objectCapacity = 10000;
  rt->objects = malloc(sizeof(Obj) * rt->objectCapacity);

  rt->toMoveCount = 0;
  rt->toMoveCapacity = 100;
  rt->toMove = malloc(sizeof(ToMove) * rt->objectCapacity);

  rt->historyCount = 0;
  rt->historyCapacity = 100;
  rt->history = malloc(sizeof(Direction) * rt->historyCapacity);
}

void startGame(Runtime * rt, FILE * file) {
  initGame(rt);
  rt->pd = parsePuzzle(file);

  printf("OBJECTS\n");
  for (int i = 0; i < objectCount(); i++) {
    printf("(%i) '%s'\n", i, objectName(i));
  }

  printf("LEGEND\n");
  for (int i = 0; i < legendCount(); i++) {
    if (legend(i)->hasStringKey == 1) {
      printf("(%i) '%s' = ", i, legend(i)->stringKey);
      for (int j = 0; j < legendObjectCount(i); j++) {
        printf("'%i' ", legendObjectId(i, j));
      }
      printf("\n");
    } else {
      printf("(%i) '%c' = ", i, legend(i)->key);
      for (int j = 0; j < legendObjectCount(i); j++) {
        printf("'%i' ", legendObjectId(i, j));
      }
      printf("\n");
    }
  }

  loadLevel(rt);
}

int playerLocation(Runtime * rt) {
  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].objId == legendId("Player")) {
      return rt->objects[i].loc;
    }
  }
  printf("Player not found\n");
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
  Direction absoluteDir = absoluteDirection(applicationDirection, dir);
  switch (absoluteDir) {
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

void applyMatch(Runtime * rt, Match * match) {
  for (int i = 0; i < match->partCount; i++) {
    if (rt->pd->debug == 1) {
      mvprintw(15, 0, "Applying match part id: '%s' -> '%s' location: %i -> %i goalMovment: %i\n",
             objectName(rt->objects[match->parts[i].objIndex].objId),
             objectName(match->parts[i].goalId),
             rt->objects[match->parts[i].objIndex].loc,
             match->parts[i].goalLocation,
             match->parts[i].goalDirection);
    }

    if (legend(match->parts[i].goalId)->objectCount > 1) {
      // This must be a match based on a legend that includes this object.
      // Stay the same

    } else {
      rt->objects[match->parts[i].objIndex].objId =  match->parts[i].goalId;
    }

    rt->objects[match->parts[i].objIndex].loc = match->parts[i].goalLocation;
    addToMove(rt, match->appliedDirection, match->parts[i].objIndex,  match->parts[i].goalDirection);
  }
}

int ruleStateMatchDir(Runtime * rt, Match * match, int ruleIndex, int matchStateIndex, int loc, Direction dir) {
  match->appliedDirection = dir;

  int success = 1;
  int count = rule(ruleIndex)->matchStates[matchStateIndex].partCount;
  // Technically we can start at one since we know we are on a good spot to start
  for (int i = 0; i < count; i++) {
    int adjustedLoc = loc + (i * locDeltaFor(rt, dir, dir));
    int legendId = rule(ruleIndex)->matchStates[matchStateIndex].parts[i].legendId;
    Direction ruleDir = rule(ruleIndex)->matchStates[matchStateIndex].parts[i].direction;

    success = 0;
    for (int j = 0; j < rt->objectCount; j++) {
      if (legendContains(legendId, rt->objects[j].objId) &&
          rt->objects[j].loc == adjustedLoc &&
          matchesDirection(ruleDir, dir, directionMoving(rt, j))
          ) {
        match->parts[match->partCount].objIndex = j;
        match->parts[match->partCount].actualLegendId = rt->objects[j].objId;
        match->parts[match->partCount].actualLocation = adjustedLoc;
        match->parts[match->partCount].actualDirection = directionMoving(rt, j);

        match->parts[match->partCount].goalDirection = absoluteDirection(dir, rule(ruleIndex)->resultStates[matchStateIndex].parts[i].direction); // TODO: this might be wrong.

        match->parts[match->partCount].ruleLegendId = legendId;


        match->parts[match->partCount].goalId = rule(ruleIndex)->resultStates[matchStateIndex].parts[i].legendId;
        // TODO: does this ever change?
        match->parts[match->partCount].goalLocation = adjustedLoc;
        success = 1;
      }
    }
    if (success == 1) {
      // found a matching object continue
      match->partCount++;
    } else {
      // failed to find an object that matches the next part, fail
      match->partCount = 0;
      return 0;
    }
  }
  return 1;
}

int ruleStateMatched(Runtime * rt, Match * match, int ruleIndex, int matchStateIndex) {
  int matchedOne = 0;
  if (rule(ruleIndex)->matchStates[matchStateIndex].partCount > 0) {
    for (int i = 0; i < rt->objectCount; i++) {
      int legendIdentity = rule(ruleIndex)->matchStates[matchStateIndex].parts[0].legendId;
      int objId = rt->objects[i].objId;
      if (legendContains(legendIdentity, objId) == 1) {
        // start of rue state matched, we can continue the rest of the rule state
        // try directions
        if (ruleStateMatchDir(rt, match, ruleIndex, matchStateIndex, rt->objects[i].loc, RIGHT) == 1) {
          return 1;
        } else if (ruleStateMatchDir(rt, match, ruleIndex, matchStateIndex, rt->objects[i].loc, UP) == 1) {
          return 1;
        } else if (ruleStateMatchDir(rt, match, ruleIndex, matchStateIndex, rt->objects[i].loc, LEFT) == 1) {
          return 1;
        } else if (ruleStateMatchDir(rt, match, ruleIndex, matchStateIndex, rt->objects[i].loc, DOWN) == 1) {
          return 1;
        }
      } else {
        // Nothing to do unless you matched it.
      }
    }
  } else {
    // no parts? in the match side? is this allowed? what does it do?
    // wildcard?
    return 0;
  }
  return matchedOne;
}

int ruleMatched(Runtime * rt, Match * match, int ruleIndex) {
  int ruleStateResult = 1;
  int count = rule(ruleIndex)->matchStateCount;
  for (int matchStateIndex = 0; matchStateIndex < count; matchStateIndex++) {
    if (ruleStateResult == 0) {
      return 0;
    } else {
      ruleStateResult = ruleStateMatched(rt, match, ruleIndex, matchStateIndex);
    }
  }
  return ruleStateResult;
}

int applyRules(Runtime * rt, ExecutionTime execTime) {
  int applied = 1;
  Match match;
  match.partCount = 0;
  int count = ruleCount();
  for (int ruleIndex = 0; ruleIndex < count; ruleIndex++) {
    applied = 1;
    while (applied == 1) {
      applied = 0;
      match.partCount = 0;
      if (ruleApplies(rt, ruleIndex, execTime)) {
        applied = ruleMatched(rt, &match, ruleIndex);
        if (applied == 1) {
          applyMatch(rt, &match);
        }
      }
    }
  }
  return applied;
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

Direction handleInput(Runtime * rt, int input) {
  // TODO: doesn't need rt
  if (input == 'w' || input == KEY_UP) {
    return UP;
  } else if (input == 's' || input == KEY_DOWN) {
    return DOWN;
  } else if (input == 'a' || input == KEY_LEFT) {
    return LEFT;
  } else if (input == 'd' || input == KEY_RIGHT) {
    return RIGHT;
  } else if (input == 'x' || input == 'X') {
    return USE;
  } else {
    return NONE;
  }
}

void setLevel(Runtime * rt) {
  if (rt->levelType == SQUARES && checkWinConditions(rt) == 1) {
    if (rt->levelIndex < levelCount() - 1) {
      nextLevel(rt);
    } else {

      if (debug() == 1) {
        printf("you won!!!\n");
        printHistory(rt);
      }
      rt->gameWon = 1;
    }
  }
}

void addHistory(Runtime * rt, Direction dir) {
  if (rt->historyCount + 1 == rt->historyCapacity) {
    printf("history realloc\n");
    rt->historyCapacity += 50;
    rt->history = realloc(rt->history, sizeof(Direction) * rt->historyCapacity);
  }
  rt->history[rt->historyCount] = dir;
  rt->historyCount++;
}

void update(Runtime * rt, Direction dir) {
  for (int i = 0; i < rt->objectCount; i++) {
    /* printf("<OBJ index: %i id: %i '%s' loc: %i>\n", i, rt->objects[i].objId, objectName(rt->objects[i].objId), rt->objects[i].loc); */
  }

  addHistory(rt, dir);
  if (rt->levelType == SQUARES) {
    addToMove(rt, NONE, objectIndex(rt, legendId("Player"), playerLocation(rt)), dir);

    // apply rules
    applyRules(rt, NORMAL);


    // apply marked for move
    moveObjects(rt);
    rt->toMoveCount = 0;

    // apply late rules
    applyRules(rt, LATE);
    setLevel(rt);
  } else {
    if (dir == USE) {
      nextLevel(rt);
    }
  }
}

int verifyOne(Runtime * rt, int thing, int container) {
  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].objId == thing) {
      for (int j = 0; j < rt->objectCount; j++) {
        if (rt->objects[j].objId == container && rt->objects[i].loc == rt->objects[j].loc) {
          return 1;
        }
      }
    }
  }
  return 0;
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
  case SOME:
    return verifyOne(rt, winCondition(winConditionIndex)->winIdentifier, winCondition(winConditionIndex)->onIndentifier);
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
