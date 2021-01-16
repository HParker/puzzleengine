#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "puzzleData.h"
#include "render.h"

int onBoard(Runtime * rt, int x, int y) {
  if (x < 0 || x >= rt->width) {
    return 0;
  }
  if (y < 0 || y >= rt->height) {
    return 0;
  }
  return 1;
}

int legendAt(Runtime * rt, int legendId, int x, int y) {
  if (onBoard(rt, x, y)) {
    int cellIndex;
    for (int i = 0; i < layerCount(); i++) {
      cellIndex = (i * rt->width * rt->height) + (y * rt->width) + x;

      if (rt->map[cellIndex] != -1 &&
          aliasLegendContains(legendId, rt->objects[rt->map[cellIndex]].objId) &&
          rt->objects[rt->map[cellIndex]].deleted == 0 // TODO: audit places that use this that also check deleted
          ) {
        return 1;
      }
    }
  }
  return 0;
}

int legendObjIndex(Runtime * rt, int legendId, int x, int y) {
  int cellIndex;
  for (int i = 0; i < layerCount(); i++) {
    cellIndex = (i * rt->width * rt->height) + (y * rt->width) + x;

    if (rt->map[cellIndex] != -1 && aliasLegendContains(legendId, rt->objects[rt->map[cellIndex]].objId)) {
      return rt->map[cellIndex];
    }
  }
  /* fprintf(stderr, "err: missed (cell: %i, objid: %i) is from %i\n", rt->map[cellIndex], rt->objects[rt->map[cellIndex]].objId, legendId); */
  return -1;
}


int isMoving(Runtime * rt, int objIndex, Direction direction) {
  for (int i = 0; i < rt->toMoveCount; i++) {
    if (rt->toMove[i].objIndex == objIndex && rt->toMove[i].direction == direction) {
      return 1;
    }
  }
  return 0;
}

int playerLocation(Runtime * rt) {
  int legendId = aliasLegendId("Player");

  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].deleted == 0 && aliasLegendContains(legendId, rt->objects[i].objId)) {
      return (rt->objects[i].x + rt->objects[i].y * rt->width);
    }
  }
  /* fprintf(stderr, "Err: failed to find player location\n"); */
  return -1;
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
  case DOWN:
  case LEFT:
  case RIGHT:
  case NONE:
  case USE:
    return ruleDir;
  case REL_RIGHT:
    return (Direction)((applicationDirection + 0) % 4);
  case REL_UP:
    return (Direction)((applicationDirection + 1) % 4);
  case REL_LEFT:
    return (Direction)((applicationDirection + 2) % 4);
  case REL_DOWN:
    return (Direction)((applicationDirection + 3) % 4);
  case COND_NO:
  case UNSPECIFIED:
  case STATIONARY:
    return NONE;
  case RANDOMDIR:
    return (Direction)rand() % 4;
  case HORIZONTAL:
    if (applicationDirection == UP) {
      return LEFT;
    } else if (applicationDirection == DOWN) {
      return RIGHT;
    } else {
      return applicationDirection;
    }
  case VERTICAL:
    if (applicationDirection == LEFT) {
      return UP;
    } else if (applicationDirection == RIGHT) {
      return DOWN;
    } else {
      return applicationDirection;
    }
  default:
    fprintf(stderr, "err: (absoluteDirection) unsupported direction (ad: %i rd: %i)\n", applicationDirection, ruleDir);
    return NONE;
  }
}

int matchesDirection(Direction ruleDir, Direction applicationDir, Direction dir, int ignoreUnspec) {
  // TODO: stationary is wrong here, it should be handled later to make sure it isn't moving
  if (dir == COND_NO || dir == STATIONARY || (ignoreUnspec && ruleDir == UNSPECIFIED)) {
    return 1;
  }
  Direction absoluteDir = absoluteDirection(applicationDir, ruleDir);
  if (absoluteDir == dir) {
    return 1;
  } else if (absoluteDir == HORIZONTAL && (dir == LEFT || dir == RIGHT)) {
    return 1;
  } else if (absoluteDir == VERTICAL && (dir == UP || dir == DOWN)) {
    return 1;
  } else {
    return 0;

  }
}

void addToMove(Runtime * rt, int objIndex, Direction direction) {
  for (int i = 0; i < rt->toMoveCount; i++) {
    if (rt->toMove[i].objIndex == objIndex) {
      rt->toMove[i].direction = direction;
      return;
    }
  }

  if (rt->toMoveCount < rt->toMoveCapacity) {
    rt->toMove[rt->toMoveCount].objIndex = objIndex;
    rt->toMove[rt->toMoveCount].direction = direction;
    rt->toMoveCount++;
  } else {
    rt->toMoveCapacity += 50;
    rt->toMove = realloc(rt->toMove, sizeof(ToMove) * rt->toMoveCapacity);

    rt->toMove[rt->toMoveCount].objIndex = objIndex;
    rt->toMove[rt->toMoveCount].direction = direction;
    rt->toMoveCount++;
  }
}

void updateObjInMap(Runtime * rt, int objIndex) {
  int layerId, x, y, cellIndex;

  layerId = objectLayer(rt->objects[objIndex].objId);
  x = rt->objects[objIndex].x;
  y = rt->objects[objIndex].y;



  cellIndex = (layerId * rt->width * rt->height) + (y * rt->width) + x;

  /* printf("updating (%i,%i,%i)[%i] = %i\n", layerId, x, y, cellIndex, objIndex); */

  rt->map[cellIndex] = objIndex;
}

void removeObjFromMap(Runtime * rt, int objIndex) {
  int layerId, x, y, cellIndex;

  layerId = objectLayer(rt->objects[objIndex].objId);
  x = rt->objects[objIndex].x;
  y = rt->objects[objIndex].y;
  cellIndex = (layerId * rt->width * rt->height) + (y * rt->width) + x;

  rt->map[cellIndex] = -1;
}


void clearMap(Runtime * rt) {
  for (int i = 0; i < (layerCount() * rt->height * rt->width); i++) {
    rt->map[i] = -1;
  }
}

void updateMap(Runtime * rt) {
  /* printf("Doing full map update\n"); */
  if (rt->levelType == SQUARES) {
    clearMap(rt);

    for (int i = 0; i < rt->objectCount; i++) {
      if (rt->objects[i].deleted == 0) {
        updateObjInMap(rt, i);
      }
    }
  }
}

void cleanup(Runtime * rt) {
  int newObjCursor = 0;
  Obj * newObjs = malloc(sizeof(Obj) * rt->objectCapacity);

  /* for (int i = 0; i < rt->toMoveCount; i++) { */
  /*   if (rt->objects[rt->toMove[i].objIndex].deleted == 1) { */
  /*     rt->toMove[i].objIndex = -1; */
  /*   } */
  /* } */

  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].deleted == 0) {
      newObjs[newObjCursor].deleted = 0;
      newObjs[newObjCursor].objId = rt->objects[i].objId;
      newObjs[newObjCursor].x = rt->objects[i].x;
      newObjs[newObjCursor].y = rt->objects[i].y;
      newObjCursor++;
    }
  }
  free(rt->objects);
  rt->objects = newObjs;
  rt->objectCount = newObjCursor;
  updateMap(rt);
}

void addObj(Runtime * rt, int objId, int x, int y) {
  if (rt->objectCount + 1 >= rt->objectCapacity) {
    rt->objectCapacity = rt->objectCapacity * 2;
    rt->objects = realloc(rt->objects, sizeof(Obj) * rt->objectCapacity);
    rt->requestCleanup = 1;
  }

  rt->objects[rt->objectCount].objId = objId;
  rt->objects[rt->objectCount].x = x;
  rt->objects[rt->objectCount].y = y;
  rt->objects[rt->objectCount].deleted = 0;
  updateObjInMap(rt, rt->objectCount);
  rt->objectCount++;

}


void loadCell(Runtime * rt, char cell, int x, int y) {
  int id = legendIdForGlyph(cell);
  int count = glyphLegendObjectCount(id);
  int backgroundId = aliasLegendObjectId(aliasLegendId("Background"), 0);
  for (int i = 0; i < count; i++) {
    int objId = glyphLegendObjectId(id, i);
    if (backgroundId != objId && objId != -1) {
      addObj(rt, objId, x, y);
    }
  }
}

void updateLevel(Runtime * rt) {
  if (checkWinConditions(rt) == 1) {
    nextLevel(rt);
  }
}

void initGame(Runtime * rt) {
  srand((unsigned)time(&rt->randomSeed));
  rt->levelIndex = 0;
  rt->gameWon = 0;
  rt->prevHistoryCount = 0;

  rt->objectCount = 0;
  rt->objectCapacity = 250;
  rt->objects = malloc(sizeof(Obj) * rt->objectCapacity);

  rt->toMoveCount = 0;
  rt->toMoveCapacity = 1000;
  rt->toMove = malloc(sizeof(ToMove) * rt->toMoveCapacity);

  rt->historyCount = 0;
  rt->historyCapacity = 1000;
  rt->history = malloc(sizeof(Direction) * rt->historyCapacity);

  rt->statesCount = 0;
  rt->statesCapacity = 1000;
  rt->states = malloc(sizeof(State) * rt->statesCapacity);
  for (int i = 0; i < rt->statesCapacity; i++) {
    rt->states[i].objectCapacity = 0;
    rt->states[i].objectCount = 0;
  }

  rt->removedId = -1;
  rt->hasMap = 0;
}

void endGame(Runtime * rt) {
  free(rt->objects);
  free(rt->toMove);
  free(rt->history);

  for (int i = 0; i < rt->statesCount; i++) {
    free(rt->states[i].objects);
    rt->states[i].objectCount = 0;
    rt->states[i].objectCapacity = 0;
  }
  free(rt->states);

  if (rt->hasMap) {
    free(rt->map);
  }
  rt->hasMap = 0;

  freePuzzle();
}


void undo(Runtime * rt, int partial) {
  if (rt->states[rt->statesCount].objectCount > 0) {
    free(rt->states[rt->statesCount].objects);
    rt->states[rt->statesCount].objectCount = 0;
    rt->states[rt->statesCount].objectCapacity = 0;
  }

  rt->objectCount = rt->states[rt->statesCount - 1].objectCount;
  rt->objectCapacity = rt->states[rt->statesCount - 1].objectCapacity;
  rt->objects = realloc(rt->objects, sizeof(Obj) * rt->states[rt->statesCount - 1].objectCapacity);

  rt->levelIndex = rt->states[rt->statesCount - 1].levelIndex;
  rt->levelType = levelType(rt->levelIndex);
  rt->height = levelHeight(rt->levelIndex);
  rt->width = levelWidth(rt->levelIndex);

  memcpy(rt->objects, rt->states[rt->statesCount - 1].objects, sizeof(Obj) * rt->objectCapacity);

  updateMap(rt);

  rt->statesCount--;
  rt->historyCount--;
}

int isMovable(Runtime * rt, int x, int y, int layerIndex) {
  if (onBoard(rt, x, y)) {
    if (layerIndex == -1) {
      printf("err: isMovable without a layer\n");
      return 0;
    }
    int cellIndex = (layerIndex * rt->width * rt->height) + (y * rt->width) + x;
    /* if (cellIndex > (layerCount() * rt->width * rt->height)) */
    if (rt->map[cellIndex] == -1) {
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
}

int ruleApplies(Runtime * rt, int ruleIndex, ExecutionTime execTime) {
  // TODO: this should check global direction constraints
  if (rule(ruleIndex)->executionTime != execTime) {
    return 0;
  }
  return 1;
}

int deltaX(Direction dir) {
  switch (dir) {
  case UP:
  case DOWN:
    return 0;
  case LEFT:
    return -1;
  case RIGHT:
    return 1;
  case USE:
    return 0;
  case NONE:
    return 0;
  default:
    fprintf(stderr, "Err: deltaX bad direction %i\n", dir);
    return 0;
  }
}

int deltaY(Direction dir) {
  switch (dir) {
  case UP:
    return -1;
  case DOWN:
    return 1;
  case LEFT:
  case RIGHT:
    return 0;
  case USE:
    return 0;
  case NONE:
    return 0;
  default:
    fprintf(stderr, "Err: deltaY bad direction %i\n", dir);
    return 0;
  }
}

int specificLegendId(Runtime * rt, int legendId, Match * match) {
  int objId;
  for (int i = 0; i < match->partCount; i++) {
    if (match->parts[i].newObject == 0) {
      objId = rt->objects[match->parts[i].objIndex].objId;
      if (aliasLegendContains(legendId, objId)) {
        return objId;
      }
    }
  }
  return aliasLegendObjectId(legendId, 0);
}


void applyMatch(Runtime * rt, Match * match) {
  if (verboseLogging()) {
    if (match->cancel == 0) {
      fprintf(stderr, "applying count %i\n", match->partCount);
      for (int i = 0; i < match->partCount; i++) {
        if (match->parts[i].newObject == 1) {
          fprintf(stderr, "Applying rule (%i): %i new: %i id: '%s' (%i) location (%i, %i) goalMovment: %s\n",
                 rule(match->ruleIndex)->lineNo,
                 match->ruleIndex,
                 match->parts[i].newObject,
                 objectName(match->parts[i].goalId),
                 match->parts[i].goalId,
                 match->parts[i].goalX,
                 match->parts[i].goalY,
                 dirName(match->parts[i].goalDirection)
                 );
        } else {
          fprintf(stderr, "Applying rule: %i new: %i, objIndex: %i, (%i) id: '%s' (%i) -> '%s' (%i) location: (%i, %i) -> (%i, %i) goalMovment: %s\n",
                 match->ruleIndex,
                 match->parts[i].newObject,
                 match->parts[i].objIndex,
                 i,
                 objectName(rt->objects[match->parts[i].objIndex].objId),
                 rt->objects[match->parts[i].objIndex].objId,
                 objectName(match->parts[i].goalId),
                 match->parts[i].goalId,
                 rt->objects[match->parts[i].objIndex].x,
                 rt->objects[match->parts[i].objIndex].y,
                 match->parts[i].goalX,
                 match->parts[i].goalY,
                 dirName(match->parts[i].goalDirection));
        }
      }
    } else {
      fprintf(stderr, "Applying rule: %i cancel\n", match->ruleIndex);
    }
  }

  int emptyId = aliasLegendId("_EMPTY_");

  if (match->cancel) {
    rt->toMoveCount = 0;
    undo(rt, 1);
    return;
  }

  for (int i = 0; i < match->partCount; i++) {
    if (match->parts[i].newObject && match->parts[i].goalId != emptyId) {
      addObj(rt, specificLegendId(rt, match->parts[i].goalId, match), match->parts[i].goalX, match->parts[i].goalY);
      if (match->parts[i].goalDirection != -1 || match->parts[i].goalDirection != UNSPECIFIED) {
        addToMove(rt, rt->objectCount - 1,  match->parts[i].goalDirection);
      }
    } else {
      if (match->parts[i].goalId == aliasLegendId("_Empty_")) {
        removeObjFromMap(rt, match->parts[i].objIndex);
        rt->objects[match->parts[i].objIndex].deleted = 1;
        /* rt->removedId = match->parts[i].objIndex; */
      } else if (match->parts[i].goalId == -1) {
        addToMove(rt, match->parts[i].objIndex,  match->parts[i].goalDirection);
      }
    }
  }
  match->partCount = 0;
}

int distance(int i, int j) {
  return abs(i - j);
}

int matchAtDistance(Direction dir, int x, int y, int targetX, int targetY) {
  switch (dir) {
  case UP:
    if (x == targetX && targetY < y) {
      return 1;
    } else {
      return 0;
    }
  case DOWN:
    if (x == targetX && targetY > y) {
      return 1;
    } else {
      return 0;
    }
  case LEFT:
    if (y == targetY && targetX < x) {
      return 1;
    } else {
      return 0;
    }
  case RIGHT:
    if (y == targetY && targetX > x) {
      return 1;
    } else {
      return 0;
    }
  default:
    fprintf(stderr, "err: (matchAtDistance) unsupported dir %i", dir);
    return 0;
  }
}

int alreadyResultIdentity(Runtime * rt, Rule * rule, int stateId, int partId, int identId, Direction appDir, int x, int y) {
  int ignoreUnspecified, objIndex;
  int matched = 0;
  int legendId = rule->resultStates[stateId].parts[partId].ruleIdentity[identId].legendId;
  int ruleDir = rule->resultStates[stateId].parts[partId].ruleIdentity[identId].direction;

  if (ruleDir == UNSPECIFIED &&
      rule->matchStates[stateId].parts[partId].ruleIdentity[identId].direction != UNSPECIFIED) {
    ignoreUnspecified = 0;
  } else {
    ignoreUnspecified = 1;
  }

  int legAt = legendAt(rt, legendId, x, y);
  if (ruleDir == COND_NO) {
    if (legAt == 0) {
      matched = 1;
    }
  } else {
    if (legAt) {
      objIndex = legendObjIndex(rt, legendId, x, y);
      if (objIndex != -1 && matchesDirection(ruleDir, appDir, directionMoving(rt, objIndex), ignoreUnspecified) == 1) {
        matched = 1;
      }
    }
  }
  return matched;
}

Direction matchLegendDirection(Rule * rule, int stateId, int partId, int legendId) {
  if (rule->matchStates[stateId].parts[partId].ruleIdentityCount <= 0) {
    return UNSPECIFIED;
  }
  for (int i = 0; i < rule->matchStates[stateId].parts[partId].ruleIdentityCount; i++) {
    if (rule->matchStates[stateId].parts[partId].ruleIdentity[i].legendId == legendId) {
      return rule->matchStates[stateId].parts[partId].ruleIdentity[i].direction;
    }
  }
  return UNSPECIFIED;
}

int resultHasLegendId(Rule * rule, int stateId, int partId, int legendId, int objId) {
  if (rule->resultStates[stateId].parts[partId].ruleIdentityCount <= 0) {
    return 0;
  }
  int resultLegendId;
  for (int i = 0; i < rule->resultStates[stateId].parts[partId].ruleIdentityCount; i++) {
    resultLegendId = rule->resultStates[stateId].parts[partId].ruleIdentity[i].legendId;
    if (aliasLegendContains(resultLegendId, objId)) {
      return 1;
    }
  }
  return 0;
}

int alreadyResult(Runtime * rt, Rule * rule, int stateId, int partId, Direction appDir, int x, int y) {
  int matched = 0;
  if (rule->resultStates[stateId].parts[partId].ruleIdentityCount <= 0) {
    return 0;
  }
  for (int i = 0; i < rule->resultStates[stateId].parts[partId].ruleIdentityCount; i++) {
    matched = alreadyResultIdentity(rt, rule, stateId, partId, i, appDir, x, y);
    if (matched == 0) {
      return 0;
    }
  }
  return 1;
}

void replaceCell(Runtime * rt, Rule * rule, int stateId, int partId, Direction appDir, Match * match) {
  match->ruleIndex = rule->lineNo;
  if (rule->resultStateCount == 0) {
    if (ruleCommandContains(rule, CANCEL)) {
      match->cancel = 1;
      return;
    }
    return;
  }

  int emptyId = aliasLegendId("_EMPTY_");

  // remove things in the match side
  int matchIdentCount = rule->matchStates[stateId].parts[partId].ruleIdentityCount;
  for (int identId = 0; identId < matchIdentCount; identId++) {

    Direction ruleDir = rule->matchStates[stateId].parts[partId].ruleIdentity[identId].direction;
    int legendId = rule->matchStates[stateId].parts[partId].ruleIdentity[identId].legendId;
    int objIndex = legendObjIndex(rt, legendId, match->targetX, match->targetY);

    if (objIndex != -1) {
      int resultIncludesSelf = resultHasLegendId(rule, stateId, partId, legendId, rt->objects[objIndex].objId);
      if (ruleDir != COND_NO &&
          legendId != emptyId &&
          resultIncludesSelf == 0
          ) {
        match->parts[match->partCount].newObject = 0;
        match->parts[match->partCount].goalId = emptyId;
        match->parts[match->partCount].objIndex = objIndex;
        match->parts[match->partCount].goalX = rt->objects[objIndex].x;
        match->parts[match->partCount].goalY = rt->objects[objIndex].y;
        match->parts[match->partCount].goalDirection = UNSPECIFIED;
        match->partCount++;
      }
    }
  }

  if (rule->resultStateCount == 0 ||
      stateId >= rule->resultStateCount ||
      (stateId < rule->resultStateCount && partId >= rule->resultStates[stateId].partCount)) {

    return;
  } else {
    int isResult = alreadyResult(rt, rule, stateId, partId, appDir, match->targetX, match->targetY);
    if (isResult) {
      return;
    }
  }

  // add the result side at this square
  int resultIdentCount = rule->resultStates[stateId].parts[partId].ruleIdentityCount;
  for (int identId = 0; identId < resultIdentCount; identId++) {
    Direction ruleDir = rule->resultStates[stateId].parts[partId].ruleIdentity[identId].direction;
    int legendId = rule->resultStates[stateId].parts[partId].ruleIdentity[identId].legendId;
    Direction matchDir = matchLegendDirection(rule, stateId, partId, legendId);
    if (legendId != emptyId) {
      if (ruleDir != COND_NO) {
        if (legendAt(rt, legendId, match->targetX, match->targetY) == 0) {
          match->parts[match->partCount].newObject = 1;
          // TODO: this should be ok all the time, but feels wrong.
          match->parts[match->partCount].goalId = legendId; // specificLegendId(rt, legendId, match);
          match->parts[match->partCount].goalX = match->targetX;
          match->parts[match->partCount].goalY = match->targetY;

          match->parts[match->partCount].goalDirection = absoluteDirection(appDir, ruleDir);
          match->partCount++;
        } else {
          // TODO: I think this case should always be the above case now
          int objIndex = legendObjIndex(rt, legendId, match->targetX, match->targetY);
          if (ruleDir != UNSPECIFIED || matchDir != UNSPECIFIED) {
            match->parts[match->partCount].newObject = 0;
            match->parts[match->partCount].goalId = -1; //aliasLegendObjectId(legendId, 0);
            match->parts[match->partCount].goalX = match->targetX;
            match->parts[match->partCount].goalY = match->targetY;

            match->parts[match->partCount].objIndex = objIndex;
            match->parts[match->partCount].goalDirection = absoluteDirection(appDir, ruleDir);
            match->partCount++;
          }
        }
      }
    }
  }
}

int partIdentity(Runtime * rt, Rule * rule, int stateId, int partId, int identId, Direction appDir, Match * match) {
  int objIndex = -1;
  int matched = 0;

  Direction ruleDir = rule->matchStates[stateId].parts[partId].ruleIdentity[identId].direction;
  int legendId = rule->matchStates[stateId].parts[partId].ruleIdentity[identId].legendId;

  int emptyId = aliasLegendId("_EMPTY_");
  int legAt = legendAt(rt, legendId, match->targetX, match->targetY);
  if (legendId == emptyId) {
    matched = 1;
  } else if (ruleDir == COND_NO && legAt == 0) {
    matched = 1;
  } else if (ruleDir != COND_NO && legAt) {
    objIndex = legendObjIndex(rt, legendId, match->targetX, match->targetY);
    if (objIndex != -1 && matchesDirection(ruleDir, appDir, directionMoving(rt, objIndex), 1) == 1) {
      matched = 1;
    }
  }

  return matched;
}

int partIdentitys(Runtime * rt, Rule * rule, int stateId, int partId, Direction appDir, Match * match) {
  int success = 0;
  int ruleLine = 0;
  for (int i = 0; i < rule->matchStates[stateId].parts[partId].ruleIdentityCount; i++) {
    success = partIdentity(rt, rule, stateId, partId, i, appDir, match);
    ruleLine = rule->lineNo;

    if (success == 0) {
      /* fprintf(stderr, "X -- FAILED rule: %i state: %i part: %i ident: %i, appDir: %s (%i, %i)\n", ruleLine, stateId, partId, i, dirName(appDir), match->targetX, match->targetY); */
      return 0;
    } else {
      /* fprintf(stderr, "V --- Matched rule: %i state: %i part: %i ident: %i, appDir: %s (%i, %i)\n", ruleLine, stateId, partId, i, dirName(appDir), match->targetX, match->targetY); */
    }
  }
  /* fprintf(stderr, "> --- Identity Success\n"); */
  return 1;
}

int identitysAtDistance(Runtime * rt, Rule * rule, int stateId, int partId, Direction appDir, int distance, Match * match) {
  int dist = distance;
  match->targetX = match->cursorX + (dist * deltaX(appDir));
  match->targetY = match->cursorY + (dist * deltaY(appDir));

  // TODO: this sucks, we need to see how far to the edge
  for (int i = 0; i < 100; i++) {
    match->targetX = match->cursorX + (dist * deltaX(appDir));
    match->targetY = match->cursorY + (dist * deltaY(appDir));
    if (onBoard(rt, match->targetX, match->targetY)) {
      if (partIdentitys(rt, rule, stateId, partId, appDir, match)) {
        return dist;
      }
      dist++;
    } else {
      // off the board no reason to continue
      return -1;
    }
  }
  return -1;
}

int completeMatch(Runtime * rt, Rule * rule, int stateId, Direction appDir, Match * match) {
  int prevPartCount = match->partCount;
  int anyDistance = 0;
  int distance = 0;
  int dist = 0;
  match->appliedDirection = appDir;

  int success = 1;
  for (int partId = 0; partId < rule->matchStates[stateId].partCount; partId++) {
    success = 0;
    match->targetX = match->cursorX + (distance * deltaX(appDir));
    match->targetY = match->cursorY + (distance * deltaY(appDir));

    if (onBoard(rt, match->targetX, match->targetY) == 0) {
      return 0;
    }
    // Little bit of a hack, but ... has to be alone in a cell, so we can try checking this way.
    int firstLegendId = rule->matchStates[stateId].parts[partId].ruleIdentity[0].legendId;
    if (strcmp(aliasLegendKey(firstLegendId), "...") == 0) {
      distance--;
      anyDistance = 1;
      success = 1;
    } else {
      if (anyDistance == 1) {
        dist = identitysAtDistance(rt, rule, stateId, partId, appDir, distance, match);
        if (dist != -1) {
          distance = dist;
          success = 1;
          anyDistance = 0;
        } else {
          success = 0;
          anyDistance = 0;
        }
      } else {
        match->targetX = match->cursorX + (distance * deltaX(appDir));
        match->targetY = match->cursorY + (distance * deltaY(appDir));
        if (partIdentitys(rt, rule, stateId, partId, appDir, match)) {
          success = 1;
          anyDistance = 0; // TODO: not needed
        }
      }
      // TODO: are we setting this again for a reason?
      match->targetX = match->cursorX + (distance * deltaX(appDir));
      match->targetY = match->cursorY + (distance * deltaY(appDir));
      // TODO: why would we do this if we aren't successful?
      replaceCell(rt, rule, stateId, partId, appDir, match);
    }

    if (success != 1) {
      // failed to find an object that matches the next part, fail
      match->partCount = prevPartCount;
      match->cancel = 0;
      return 0;
    }
    distance++;
  }

  if (verboseLogging()) {
    debugRender(rt, match);
  }
  return 1;
}

int cellMatch(Runtime * rt, Rule * rule, int stateId, Match * match) {
  int legId;
  Direction dirConst;

  if (rule->matchStates[stateId].partCount > 0 && rule->matchStates[stateId].parts[0].ruleIdentityCount > 0) {
    dirConst = rule->directionConstraint;
    legId = rule->matchStates[stateId].parts[0].ruleIdentity[0].legendId;

    if (partIdentitys(rt, rule, stateId, 0, dirConst, match)) {
      return completeMatch(rt, rule, stateId, dirConst, match);
    }
  }
  return 0;
}

int hasSpace(Runtime * rt, Rule * rule, RuleState * state, Match * match) {
  int spaceRequired = state->partCount;
  if (rule->hasSpread) {
    spaceRequired--;
  }

  switch (rule->directionConstraint) {
  case UP:
    return (match->cursorY - spaceRequired >= -1);
  case DOWN:
    return (match->cursorY + spaceRequired <= rt->height);
  case LEFT:
    return (match->cursorX - spaceRequired >= -1);
  case RIGHT:
    return (match->cursorX + spaceRequired <= rt->width);
  default:
    printf("DON't see me ever\n");
    return 1;
  }
}

int applyState(Runtime * rt, Rule * rule, int stateId, Match * match) {
  int applied;
  int matched = 0;

  int count = levelCellCount(rt->levelIndex);
  for (int y = 0; y < rt->height; y++) {
    for (int x = 0; x < rt->width; x++) {
      match->targetX = match->cursorX = x;
      match->targetY = match->cursorY = y;
      if (hasSpace(rt, rule, &rule->matchStates[stateId], match)) {
        applied = cellMatch(rt, rule, stateId, match);
        if (applied) {
          // TODO: can I early return here?
          matched = 1;
          if (match->partCount > 0 || match->cancel) {
            return 1;
          }
        }
      }
    }
  }

  match->cursorX = -1;
  match->cursorY = -1;
  match->targetX = -1;
  match->targetY = -1;
  return matched;
}

int applyRule(Runtime * rt, Rule * rule, Match * match) {
  int applied = 0;

  int stateCount = rule->matchStateCount;
  for (int i = 0; i < stateCount; i++) {
    applied = applyState(rt, rule, i, match);
    if (applied == 0) {
      return 0;
    }
  }
  return 1;
}

void applyRules(Runtime * rt, ExecutionTime execTime) {
  int applied = 1;
  int maxAttempts = 10000;
  int attempts = 0;

  Match match;
  match.partCount = 0;
  match.cancel = 0;

  int count = ruleCount();
  for (int ruleIndex = 0; ruleIndex < count; ruleIndex++) {
    match.ruleIndex = ruleIndex;
    /* printf("%s\n", ruleString(ruleIndex)); */

    applied = 1;
    attempts = 0;
    while (match.cancel == 0 && applied && attempts < maxAttempts) {
      applied = 0;
      match.partCount = 0;
      if (ruleApplies(rt, ruleIndex, execTime)) {
        applied = applyRule(rt, rule(ruleIndex), &match);
        /* fprintf(stderr, "ruleIdex (%i/%i) applied (%i) && (match.partCount (%i) > 0 || match.cancel (%i))\n", ruleIndex, rt->pd->ruleCount, applied, match.partCount, match.cancel); */
        if (applied && (match.partCount > 0 || match.cancel)) {
          applyMatch(rt, &match);
        } else {
          applied = 0;
        }
      }
      attempts++;
    }
    if (attempts >= maxAttempts) {
      fprintf(stderr, "warn: max attempts reached\n");
    }
  }
}

int moveObjects(Runtime * rt) {
  int playerMoved = 0;
  int moveApplied[rt->toMoveCount];
  for (int i = 0; i < rt->toMoveCount; i++) {
    moveApplied[i] = 0;
  }

  int somethingApplied = 1;
  while (somethingApplied == 1) {
    somethingApplied = 0;
    for (int i = 0; i < rt->toMoveCount; i++) {
      if (rt->toMove[i].objIndex != -1) {
        int x = rt->objects[rt->toMove[i].objIndex].x;
        int y = rt->objects[rt->toMove[i].objIndex].y;

        int layerIndex = objectLayer(rt->objects[rt->toMove[i].objIndex].objId);
        int movingToX = x + deltaX(rt->toMove[i].direction);
        int movingToY = y + deltaY(rt->toMove[i].direction);

        if (rt->objects[rt->toMove[i].objIndex].deleted == 0 && moveApplied[i] == 0 && isMovable(rt, movingToX, movingToY, layerIndex) == 1) {

          removeObjFromMap(rt, rt->toMove[i].objIndex);

          rt->objects[rt->toMove[i].objIndex].x += deltaX(rt->toMove[i].direction);
          rt->objects[rt->toMove[i].objIndex].y += deltaY(rt->toMove[i].direction);

          updateObjInMap(rt, rt->toMove[i].objIndex);

          if (aliasLegendContains(aliasLegendId("Player"), rt->objects[rt->toMove[i].objIndex].objId)) {
            playerMoved = 1;
          }
          moveApplied[i] = 1;
          somethingApplied = 1;
        }
      }
    }
  }
  rt->toMoveCount = 0;
  return playerMoved;
}

void printHistory(Runtime * rt) {
  for (int i = 0; i < rt->historyCount; i++) {
    fprintf(stderr, "%s\n", dirName(rt->history[i]));
  }
}

Direction handleInput(Runtime * rt, int input) {
  // TODO: doesn't need rt
  if (input == 'w') {
    return UP;
  } else if (input == 's') {
    return DOWN;
  } else if (input == 'a') {
    return LEFT;
  } else if (input == 'd') {
    return RIGHT;
  } else if (input == 'x' && noAction() == 0) {
    return USE;
  } else if (input == 'q') {
    return QUIT;
  } else if (input == 'r' && noRestart() == 0) {
    return RESTART;
  } else if (input == 'z' && noUndo() == 0) {
    return UNDO;
  } else if (input == '`') {
    verboseLoggingOn();
    return NONE;
  } else {
    // Is this none or something else?
    return NONE;
  }
}

void markPlayerAsMoving(Runtime * rt, Direction dir) {
  int legendId = aliasLegendId("Player");

  for (int i = 0; i < rt->objectCount; i++) {
    if (aliasLegendContains(legendId, rt->objects[i].objId)) {
      addToMove(rt, i, dir);
    }
  }
}

void addHistory(Runtime * rt, Direction dir) {
  if (rt->historyCount == rt->historyCapacity) {
    rt->historyCapacity += 50;
    rt->history = realloc(rt->history, sizeof(Direction) * rt->historyCapacity);
  }
  rt->history[rt->historyCount] = dir;
  rt->historyCount++;
}

void addState(Runtime * rt) {
  // This seems weird, but I think we only have to copy objects at the time.
  // Since history can continue where it was if you undo and `toMove`
  // should always be empty.
  if (rt->statesCount + 1 >= rt->statesCapacity) {
    rt->statesCapacity += 50;
    rt->states = realloc(rt->states, sizeof(State) * rt->statesCapacity);
    for (int i = rt->statesCount + 1; i < rt->statesCapacity; i++) {
      rt->states[i].objectCapacity = 0;
      rt->states[i].objectCount = 0;
    }
  }

  if (rt->states[rt->statesCount].objectCount > 0) {
    // TODO: I feel like this shouldn't happen
    free(rt->states[rt->statesCount].objects);
    rt->states[rt->statesCount].objectCount = 0;
    rt->states[rt->statesCount].objectCapacity = 0;
  }

  rt->states[rt->statesCount].levelIndex = rt->levelIndex;
  rt->states[rt->statesCount].objectCount = rt->objectCount;
  rt->states[rt->statesCount].objectCapacity = rt->objectCapacity;

  rt->states[rt->statesCount].objects = malloc(sizeof(Obj) * rt->objectCapacity);
  memcpy(rt->states[rt->statesCount].objects, rt->objects, sizeof(Obj) * rt->objectCapacity);

  rt->statesCount++;
}

void loadLevel(Runtime * rt) {
  rt->levelType = levelType(rt->levelIndex);

  if (rt->levelType == SQUARES) {
    rt->height = levelHeight(rt->levelIndex);
    rt->width = levelWidth(rt->levelIndex);
    rt->toMoveCount = 0;
    rt->objectCount = 0;

    if (rt->hasMap) {
      free(rt->map);
    }
    rt->hasMap = 1;
    rt->map = malloc((sizeof(int) * rt->height * rt->width * layerCount()));

    clearMap(rt);

    /* int backgroundId = aliasLegendObjectId(aliasLegendId("Background"), 0); */
    /* for (int x = 0; x < rt->width; x++) { */
    /*   for (int y = 0; y < rt->height; y++) { */
    /*     addObj(rt, backgroundId, x, y); */
    /*   } */
    /* } */

    int count = levelCellCount(rt->levelIndex);
    for (int i = 0; i < count; i++) {
      int x = i % rt->width;
      int y = i / rt->width;
      loadCell(rt, levelCell(rt->levelIndex, i), x, y);
    }
  }

  if (rt->pd->runRulesOnLevelStart) {
    applyRules(rt, NORMAL);
    moveObjects(rt);
    applyRules(rt, LATE);
  }
}

void nextLevel(Runtime * rt) {
  if (rt->levelIndex < levelCount() - 1) {
    rt->prevHistoryCount = rt->historyCount;
    rt->levelIndex++;
    if (rt->hasMap) {
      rt->hasMap = 0;
      free(rt->map);
    }
    loadLevel(rt);
  } else {
    rt->gameWon = 1;
  }
}

void startGame(Runtime * rt, FILE * file) {
  initGame(rt);
  rt->pd = parsePuzzle(file);
  loadLevel(rt);

  if (verboseLogging()) {
    printRules();
  }

}

void tick(Runtime * rt) {
  if (rt->requestCleanup) {
    cleanup(rt);
    rt->requestCleanup = 0;
  }

  printf("objectCount: %i\n", rt->objectCount);
  applyRules(rt, NORMAL);
  moveObjects(rt);
}

void update(Runtime * rt, Direction dir) {
  if (rt->requestCleanup) {
    cleanup(rt);
    rt->requestCleanup = 0;
  }

  int startingPlayerLocation = playerLocation(rt);
  addHistory(rt, dir);
  addState(rt);
  rt->toMoveCount = 0;

  if (dir == RESTART) {
    rt->historyCount = rt->prevHistoryCount;
    loadLevel(rt);
    return;
  }

  // TODO: remove deleted objects?
  if (rt->levelType == SQUARES) {
    // mark player for moving
    markPlayerAsMoving(rt, dir);

    applyRules(rt, NORMAL);
    moveObjects(rt);
    applyRules(rt, LATE);

    if (requirePlayerMovement() &&
        (startingPlayerLocation == playerLocation(rt))) {
      undo(rt, 1);
      return;
    }

    updateLevel(rt);
  } else {
    if (dir == USE) {
      nextLevel(rt);
    }
  }
}

int verifyOne(Runtime * rt, int thingId, int container, int hasOnQualifier) {
  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].deleted == 0 && aliasLegendContains(thingId, rt->objects[i].objId) == 1) {
      if (hasOnQualifier == 0) {
        return 1;
      }
      for (int j = 0; j < rt->objectCount; j++) {
        if (rt->objects[j].objId == container && rt->objects[i].x == rt->objects[j].x && rt->objects[i].y == rt->objects[j].y) {
          return 1;
        }
      }
    }
  }
  return 0;
}

int verifyNone(Runtime * rt, int thingId, int containerId, int hasOnQualifier) {
  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].deleted == 0 && aliasLegendContains(thingId, rt->objects[i].objId) == 1) {
      if (hasOnQualifier) {
        for (int j = 0; j < rt->objectCount; j++) {
          if (aliasLegendContains(containerId, rt->objects[j].objId) &&
              rt->objects[i].x == rt->objects[j].x &&
              rt->objects[i].y == rt->objects[j].y) {
            return 0;
          }
        }
      } else {
        return 0;
      }
    }
  }
  return 1;
}

int verifyAll(Runtime * rt, int thingId, int containerId, int hasOnQualifier) {
  int satisfied = 1;
  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].deleted == 0 && aliasLegendContains(thingId, rt->objects[i].objId) == 1) {
      satisfied = 0;
      for (int j = 0; j < rt->objectCount; j++) {
        if (rt->objects[j].deleted == 0 &&
            aliasLegendContains(containerId, rt->objects[j].objId) &&
            rt->objects[i].x == rt->objects[j].x &&
            rt->objects[i].y == rt->objects[j].y) {
          satisfied = 1;
        }
      }
      if (satisfied == 0) {
        return 0;
      }
    }

  }

  return satisfied;
}

int checkWinCondition(Runtime * rt, int winConditionIndex) {
  switch (winCondition(winConditionIndex)->winQualifier) {
  case ALL:
    return verifyAll(rt, winCondition(winConditionIndex)->winIdentifier, winCondition(winConditionIndex)->onIndentifier, winCondition(winConditionIndex)->hasOnQualifier);
  case SOME:
    return verifyOne(rt, winCondition(winConditionIndex)->winIdentifier, winCondition(winConditionIndex)->onIndentifier, winCondition(winConditionIndex)->hasOnQualifier);
  case ANY:
    return verifyOne(rt, winCondition(winConditionIndex)->winIdentifier, winCondition(winConditionIndex)->onIndentifier, winCondition(winConditionIndex)->hasOnQualifier);
  case NO:
    return verifyNone(rt, winCondition(winConditionIndex)->winIdentifier, winCondition(winConditionIndex)->onIndentifier, winCondition(winConditionIndex)->hasOnQualifier);
  default:
    fprintf(stderr, "err: unsupported win condition '%i'\n", winCondition(winConditionIndex)->winQualifier);
    return 0;
  }
}

int checkWinConditions(Runtime * rt) {
  int satisfied = 0;
  int count = winConditionCount();

  for (int i = 0; i < count; i++) {
    satisfied = checkWinCondition(rt, i);
    if (satisfied == 0) {
      return 0;
    }
  }

  return satisfied;
}
