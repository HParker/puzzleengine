#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "puzzleData.h"

char * directionName(Direction dir) {
  char * directionNames[] = {
                             "RIGHT",
                             "UP",
                             "LEFT",
                             "DOWN",
                             "HORIZONTAL",
                             "VERTICAL",
                             "STATIONARY",
                             "RANDOMDIR",
                             "RANDOM",
                             "REL_UP",
                             "REL_DOWN",
                             "REL_LEFT",
                             "REL_RIGHT",
                             "USE",
                             "NONE",
                             "COND_NO",
                             "QUIT",
                             "RESTART",
                             "UNDO",
                             "UNSPECIFIED"
  };
  return directionNames[dir];
}

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
  } else {
    /* printf("err: tried legendAt off board\n"); */
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
  printf("err: missed %i is in %i\n", rt->objects[rt->map[cellIndex]].objId, legendId);
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

int playerLocation(Runtime * rt) {
  int legendId = aliasLegendId("Player");

  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].deleted == 0 && aliasLegendContains(legendId, rt->objects[i].objId)) {
      return (rt->objects[i].x + rt->objects[i].y * rt->width);
    }
  }
  /* printf("Err: failed to find player location\n"); */
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
  case USE:
  case COND_NO:
  case UNSPECIFIED:
  case STATIONARY:
    return NONE;
  case RANDOMDIR:
    // TODO: more robust random
    return RIGHT;
  default:
    printf("err: (absoluteDirection) unsupported direction (ad: %i rd: %i)\n", applicationDirection, ruleDir);
    return NONE;
  }
}

int matchesDirection(Direction ruleDir, Direction applicationDir, Direction dir, int ignoreUnspec) {
  if (dir == COND_NO || (ignoreUnspec && ruleDir == UNSPECIFIED)) {
    return 1; // TODO: remove special case here
  }
  Direction absoluteDir = absoluteDirection(applicationDir, ruleDir);
  if (absoluteDir == dir) {
    return 1;
  } else {
    return 0;
  }
}

void addToMove(Runtime * rt, Direction applicationDirection, int objIndex, Direction direction) {
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
    printf("tomove realloc\n");
    rt->toMove = realloc(rt->toMove, sizeof(ToMove) * rt->toMoveCapacity);

    rt->toMove[rt->toMoveCount].objIndex = objIndex;
    rt->toMove[rt->toMoveCount].direction = direction;
    rt->toMoveCount++;
  }
}

void addObj(Runtime * rt, int objId, int x, int y) {
  if (rt->objectCount < rt->objectCapacity) {
    rt->objects[rt->objectCount].objId = objId;
    rt->objects[rt->objectCount].x = x;
    rt->objects[rt->objectCount].y = y;
    rt->objects[rt->objectCount].deleted = 0;
    rt->objectCount++;
  } else {
    printf("object realloc capacity %i -> %i\n", rt->objectCapacity, rt->objectCapacity + 50);
    rt->objectCapacity += 50;
    rt->objects = realloc(rt->objects, sizeof(Obj) * rt->objectCapacity);

    rt->objects[rt->objectCount].objId = objId;
    rt->objects[rt->objectCount].x = x;
    rt->objects[rt->objectCount].y = y;
    rt->objects[rt->objectCount].deleted = 0;
    rt->objectCount++;
  }
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

void loadLevel(Runtime * rt) {
  rt->levelType = levelType(rt->levelIndex);
  if (rt->levelType == SQUARES) {
    rt->height = levelHeight(rt->levelIndex);
    rt->width = levelWidth(rt->levelIndex);
    rt->toMoveCount = 0;
    rt->objectCount = 0;

    int count = levelCellCount(rt->levelIndex);
    for (int i = 0; i < count; i++) {
      int x = i % rt->width;
      int y = i / rt->width;
      loadCell(rt, levelCell(rt->levelIndex, i), x, y);
    }
  } else {
    // Message style level
    // Not sure we have to do anything
    // Since we can just render the level from the message stored in the puzzle data;
  }

  // map
  if (rt->levelType == SQUARES) {
    // TODO: I can malloc this once since I can find out the largest size
    free(rt->map);
    rt->map = malloc((sizeof(int) * rt->height * rt->width * layerCount()));
  }
}

void nextLevel(Runtime * rt) {
  if (rt->levelIndex < levelCount() - 1) {
    rt->prevHistoryCount = rt->historyCount;
    rt->levelIndex++;
    loadLevel(rt);
  } else {
    rt->gameWon = 1;
  }
}

void updateLevel(Runtime * rt) {
  if (checkWinConditions(rt) == 1) {
    nextLevel(rt);
  }
}

void initGame(Runtime * rt) {
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

  rt->map = malloc(1);
}

void startGame(Runtime * rt, FILE * file) {
  initGame(rt);
  rt->pd = parsePuzzle(file);
  loadLevel(rt);
}

void endGame(Runtime * rt) {
  free(rt->objects);
  free(rt->toMove);
  free(rt->history);

  for (int i = 0; i < rt->statesCount; i++) {
    free(rt->states[i].objects);
  }
  free(rt->states);

  free(rt->map);
  freePuzzle();
}


void undo(Runtime * rt, int partial) {
  rt->toMoveCount = 0;
  if (partial == 0) {
    /* free(rt->states[rt->statesCount].objects); */
  }

  rt->objectCount = rt->states[rt->statesCount - 1].objectCount;
  rt->objectCapacity = rt->states[rt->statesCount - 1].objectCapacity;
  rt->objects = realloc(rt->objects, sizeof(Obj) * rt->states[rt->statesCount - 1].objectCapacity);

  rt->levelIndex = rt->states[rt->statesCount - 1].levelIndex;
  rt->levelType = levelType(rt->levelIndex);
  rt->height = levelHeight(rt->levelIndex);
  rt->width = levelWidth(rt->levelIndex);

  memcpy(rt->objects, rt->states[rt->statesCount - 1].objects, sizeof(Obj) * rt->objectCapacity);

  rt->statesCount--;
  rt->historyCount--;
}

int isMovable(Runtime * rt, int x, int y, int layerIndex) {
  // inbounds
  if (x >= rt->width || x < 0 || y >= rt->height || y < 0) {
    return 0;
  }

  int hasCollidable = 0;
  for (int i = 0; i < rt->objectCount; i++) {
    if (rt->objects[i].deleted == 0 && rt->objects[i].x == x && rt->objects[i].y == y && layerIncludes(layerIndex, rt->objects[i].objId)) {
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
    printf("Err: deltaX bad direction %i\n", dir);
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
    printf("Err: deltaY bad direction %i\n", dir);
    return 0;
  }
}

void updateMap(Runtime * rt) {
  if (rt->levelType == SQUARES) {
    int layerId, x, y, cellIndex;
    for (int i = 0; i < layerCount() * rt->height * rt->width; i++) {
      rt->map[i] = -1;
    }

    for (int i = 0; i < rt->objectCount; i++) {
      if (rt->objects[i].deleted == 0) {
        layerId = objectLayer(rt->objects[i].objId);
        if (layerId == -1) {
          printf("no layer id for index: %i, objid: %i, deleted: %i\n", i, rt->objects[i].objId, rt->objects[i].deleted);
        }

        if (rt->objects[i].deleted == 0 && layerId != -1) {
          x = rt->objects[i].x;
          y = rt->objects[i].y;
          cellIndex = (layerId * rt->width * rt->height) + (y * rt->width) + x;
          rt->map[cellIndex] = i;
        }
      }
    }
  }
}

void applyMatch(Runtime * rt, Match * match) {
  if (rt->pd->debug == 1) {
    if (match->cancel == 0) {
      printf("applying count %i\n", match->partCount);
      for (int i = 0; i < match->partCount; i++) {
        printf("Applying rule: %i new: %i, objIndex %i, (%i) id: '%s' (%i) -> '%s' (%i) location: (%i,%i) -> (%i,%i) goalMovment: %s\n",
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
               directionName(match->parts[i].goalDirection));
      }
    } else {
      printf("Applying rule: %i cancel\n", match->ruleIndex);
    }
  }

  if (match->cancel == 1) {
    undo(rt, 1);
    return;
  }

  for (int i = 0; i < match->partCount; i++) {
    if (match->parts[i].newObject == 1 && match->parts[i].goalId != -1) {
      addObj(rt, match->parts[i].goalId, match->parts[i].goalX, match->parts[i].goalY);
      // TODO: I don't like mutating this, lets try not to.
      match->parts[i].objIndex = rt->objectCount - 1;
    } else if (match->parts[i].goalId == aliasLegendId("_Empty_")) {
      rt->objects[match->parts[i].objIndex].deleted = 1;
    } else if (aliasLegendObjectCount(match->parts[i].goalId) == 1 && match->parts[i].goalId != aliasLegendId("...") && match->parts[i].goalId != aliasLegendId("_Empty_")) {
      // TODO: does this work if I use a alias with only one value?
      rt->objects[match->parts[i].objIndex].objId = aliasLegendObjectId(match->parts[i].goalId, 0);
    }

    if (match->parts[i].objIndex == -1) {
      printf("got here with objindex -1 why? goalId: %i\n", match->parts[i].goalId);
    } else {
      rt->objects[match->parts[i].objIndex].objId = match->parts[i].goalId;
      rt->objects[match->parts[i].objIndex].x = match->parts[i].goalX;
      rt->objects[match->parts[i].objIndex].y = match->parts[i].goalY;
    }

    if (match->parts[i].goalDirection != -1) {
      addToMove(rt, match->appliedDirection, match->parts[i].objIndex,  match->parts[i].goalDirection);
    }
   }
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
    printf("err: (matchAtDistance) unsupported dir %i", dir);
    return 0;
  }
}

// TODO: match at distance
int objNotAt(Runtime * rt, int legendId, int x, int y) {
  for (int j = 0; j < rt->objectCount; j++) {
    int legendContains = aliasLegendContains(legendId, rt->objects[j].objId);
    if (rt->objects[j].deleted == 0 && legendContains && rt->objects[j].x == x && rt->objects[j].y == y) {
      return 0;
    }
  }
  return 1;
}

int alreadyResultIdentity(Runtime * rt, int ruleId, int stateId, int partId, int identId, Direction appDir, int x, int y) {
  int ignoreUnspecified, objIndex;
  int matched = 0;
  int legendId = rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentity[identId].legendId;
  int ruleDir = rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentity[identId].direction;

  if (ruleDir == UNSPECIFIED &&
      rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentity[identId].direction != UNSPECIFIED) {
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

int resultHasLegendId(int ruleId, int stateId, int partId, int legendId) {
  if (rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentityCount <= 0) {
    return 0;
  }
  for (int i = 0; i < rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentityCount; i++) {
    if (rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentity[i].legendId == legendId) {
      return 1;
    }
  }
  return 0;
}

int alreadyResult(Runtime * rt, int ruleId, int stateId, int partId, Direction appDir, int x, int y) {
  int matched = 0;
  if (rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentityCount <= 0) {
    return 0;
  }
  for (int i = 0; i < rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentityCount; i++) {
    matched = alreadyResultIdentity(rt, ruleId, stateId, partId, i, appDir, x, y);
    if (matched == 0) {
      return 0;
    }
  }
  return 1;
}

int partIdentity(Runtime * rt, int ruleId, int stateId, int partId, int identId, Direction appDir, int x, int y, Match * match) {
  int objIndex = -1;
  int matched = 0;

  Direction ruleDir = rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentity[identId].direction;
  int legendId = rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentity[identId].legendId;

  int emptyId = aliasLegendId("_EMPTY_");
  int legAt = legendAt(rt, legendId, x, y);


  if (ruleDir == COND_NO || legendId == emptyId) {
    if (legAt == 0 || legendId == emptyId) {
      matched = 1;
    }
  } else {
    if (legAt) {
      objIndex = legendObjIndex(rt, legendId, x, y);
      if (objIndex != -1 && matchesDirection(ruleDir, appDir, directionMoving(rt, objIndex), 1) == 1) {
        matched = 1;
      }
    }
  }

  RuleStatePart * resultPart = &rule(ruleId)->resultStates[stateId].parts[partId];
  int prevPartCount = match->partCount; // TODO: not sure this helps, we don't inc unless we succeeded

  if (matched) {
    if (alreadyResult(rt, ruleId, stateId, partId, appDir, x, y) == 0) {
      match->ruleIndex = ruleId;
      if (ruleDir == COND_NO || legendId == emptyId) {
        match->parts[match->partCount].objIndex = -1;
        match->parts[match->partCount].newObject = 1;
        match->parts[match->partCount].goalX = x;
        match->parts[match->partCount].goalY = y;
        match->parts[match->partCount].goalDirection = -1; // TODO: UNSPECIFIED, but must be handled in application first
      } else {
        match->parts[match->partCount].newObject = 0;
        match->parts[match->partCount].objIndex = objIndex;
        match->parts[match->partCount].goalX = rt->objects[objIndex].x;
        match->parts[match->partCount].goalY = rt->objects[objIndex].y;
        match->parts[match->partCount].goalDirection = -1; // TODO: UNSPECIFIED, but must be handled in application first
      }

      // TODO: These are always the same
      match->parts[match->partCount].actualX = x;
      match->parts[match->partCount].actualY = y;

      if (rule(ruleId)->cancel) {
        // undo the whole turn...
        // TODO: not sure we have to set goal* here since we ignore it for the cancel case
        match->parts[match->partCount].goalDirection = -1; // TODO: UNSPECIFIED, but must be handled in application first
        match->parts[match->partCount].goalId = -1;
        match->cancel = 1;
      } else {
        match->parts[match->partCount].goalDirection = absoluteDirection(appDir, resultPart->ruleIdentity[identId].direction);

        if (objIndex != -1 && aliasLegendContains(resultPart->ruleIdentity[identId].legendId, rt->objects[objIndex].objId) == 1) {
          match->parts[match->partCount].goalId = rt->objects[objIndex].objId;
        } else {
          match->parts[match->partCount].goalId = resultPart->ruleIdentity[identId].legendId;
        }
      }
      match->partCount++;
    } else {
      if (resultHasLegendId(ruleId, stateId, partId, legendId) == 0 && ruleDir != COND_NO) {
        printf("objectIndex %i\n", objIndex);
        match->parts[match->partCount].objIndex = objIndex;
        match->parts[match->partCount].goalId = emptyId;
        match->parts[match->partCount].goalX = rt->objects[objIndex].x;
        match->parts[match->partCount].goalY = rt->objects[objIndex].y;
        match->parts[match->partCount].actualX = x;
        match->parts[match->partCount].actualY = y;
        match->parts[match->partCount].goalDirection = -1; // TODO: UNSPECIFIED, but must be handled in application first
        match->partCount++;
      }
    }
  } else {
    match->partCount = prevPartCount;
    match->cancel = 0;
    return 0;
  }
  return 1;
}

int partIdentitys(Runtime * rt, int ruleId, int stateId, int partId, Direction appDir, int x, int y, Match * match) {
  int prevMatchCount = match->partCount;
  int success = 0;

  int count = rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentityCount;
  for (int i = 0; i < count; i++) {
    success = partIdentity(rt, ruleId, stateId, partId, i, appDir, x, y, match);
    if (success == 0) {
      /* printf("X -- FAILED rule: %i state: %i part: %i ident: %i, appDir: %s (%i, %i)\n", ruleId, stateId, partId, i, directionName(appDir), x, y); */
      match->partCount = prevMatchCount;
      return 0;
    } else {
      /* printf("V --- Matched rule: %i state: %i part: %i ident: %i, appDir: %s (%i, %i)\n", ruleId, stateId, partId, i, directionName(appDir), x, y); */
    }
  }
  /* printf("> --- Identity Success\n"); */
  return 1;
}

int identitysAtDistance(Runtime * rt, int ruleId, int stateId, int partId, Direction appDir, int distance, int x, int y, Match * match) {
  int dist = distance;
  int currentX = x + (dist * deltaX(appDir));
  int currentY = y + (dist * deltaY(appDir));

  // TODO: this sucks, we need to see how far to the edge
  for (int i = 0; i < 100; i++) {
    currentX = x + (dist * deltaX(appDir));
    currentY = y + (dist * deltaY(appDir));
    if (onBoard(rt, currentX, currentY)) {
      /* printf("trying dist at (%i, %i)\n", currentX, currentY); */
      if (partIdentitys(rt, ruleId, stateId, partId, appDir, currentX, currentY, match)) {
        return dist;
      }
      dist++;
    }
  }
  return -1;
}

int completeMatch(Runtime * rt, int ruleId, int stateId, Direction appDir, int x, int y, Match * match) {
  int prevPartCount = match->partCount;
  int anyDistance = 0;
  int distance = 0;
  int dist = 0;
  match->appliedDirection = appDir;

  int success = 1;

  int count = rule(ruleId)->matchStates[stateId].partCount;
  for (int i = 0; i < count; i++) {
    success = 0;

    int currentX = x + (distance * deltaX(appDir));
    int currentY = y + (distance * deltaY(appDir));

    // Little bit of a hack, but ... has to be alone in a cell, so we can try checking this way.
    int firstLegendId = rule(ruleId)->matchStates[stateId].parts[i].ruleIdentity[0].legendId;
    if (strcmp(aliasLegendKey(firstLegendId), "...") == 0) {
      distance--;
      anyDistance = 1;
      success = 1;
    } else {
      if (anyDistance == 1) {
        dist = identitysAtDistance(rt, ruleId, stateId, i, appDir, distance, x, y, match);
        if (dist != -1) {
          distance = dist;
          success = 1;
          anyDistance = 0;
        } else {
          success = 0;
          anyDistance = 0;
        }
      } else {
        if (partIdentitys(rt, ruleId, stateId, i, appDir, currentX, currentY, match)) {
          success = 1;
          anyDistance = 0; // TODO: not needed
        }
      }
    }

    if (success != 1) {
      // failed to find an object that matches the next part, fail
      match->partCount = prevPartCount;
      return 0;
    }
    distance++;
  }
  return 1;
}

int continueMatch(Runtime * rt, int ruleId, int stateId, int x, int y, Match * match) {
  Direction dirConstant = rule(ruleId)->directionConstraint;

  if (dirConstant == RIGHT || dirConstant == HORIZONTAL || dirConstant == NONE) {
    if (completeMatch(rt, ruleId, stateId, RIGHT, x, y, match)) {
      return 1;
    }
  }

  if (dirConstant == UP || dirConstant == VERTICAL || dirConstant == NONE) {
    if (completeMatch(rt, ruleId, stateId, UP, x, y, match)) {
      return 1;
    }
  }

  if (dirConstant == LEFT || dirConstant == HORIZONTAL || dirConstant == NONE) {
    if (completeMatch(rt, ruleId, stateId, LEFT, x, y, match)) {
      return 1;
    }
  }

  if (dirConstant == DOWN || dirConstant == VERTICAL || dirConstant == NONE) {
    if (completeMatch(rt, ruleId, stateId, DOWN, x, y, match)) {
      return 1;
    }
  }
  return 0;
}

int cellMatch(Runtime * rt, int ruleId, int stateId, int x, int y, Match * match) {
  int prevMatchCount = match->partCount;
  int legId, legAt;
  Direction dir;

  if (rule(ruleId)->matchStates[stateId].partCount > 0 && rule(ruleId)->matchStates[stateId].parts[0].ruleIdentityCount > 0 ) {
    legId = rule(ruleId)->matchStates[stateId].parts[0].ruleIdentity[0].legendId;
    dir = rule(ruleId)->matchStates[stateId].parts[0].ruleIdentity[0].direction;

    legAt = legendAt(rt, legId, x, y);
    if ((dir == COND_NO && legAt == 0) || (dir != COND_NO && legAt)) {
      if (continueMatch(rt, ruleId, stateId, x, y, match)) {
        return 1;
      } else {
        match->partCount = prevMatchCount;
        return 0;
      }
    }
  }
  return 0;
}

int applyState(Runtime * rt, int ruleId, int stateId, Match * match) {
  int applied, x, y;
  int matchedOne = 0;

  int count = levelCellCount(rt->levelIndex);
  for (int i = 0; i < count; i++) {
    x = i % rt->width;
    y = i / rt->width;

    applied = cellMatch(rt, ruleId, stateId, x, y, match);
    if (applied) {
      // TODO: can I early return here?
      matchedOne = 1;
    }
  }
  return matchedOne;
}

int applyRule(Runtime * rt, int ruleId, Match * match) {
  updateMap(rt);
  int applied = 0;

  int stateCount = rule(ruleId)->matchStateCount;
  for (int i = 0; i < stateCount; i++) {
    applied = applyState(rt, ruleId, i, match);
    if (applied == 0) {
      return 0;
    }
  }
  return 1;
}

int applyRules(Runtime * rt, ExecutionTime execTime) {
  int cancel = 0;
  int applied = 1;
  int maxAttempts = 100;
  int attempts = 0;

  Match match;
  match.partCount = 0;
  match.cancel = 0;

  int count = ruleCount();
  for (int ruleIndex = 0; ruleIndex < count; ruleIndex++) {
    applied = 1;
    attempts = 0;
    while (match.cancel == 0 && applied && attempts < maxAttempts) {
      applied = 0;
      match.partCount = 0;
      if (ruleApplies(rt, ruleIndex, execTime)) {
        applied = applyRule(rt, ruleIndex, &match);
        /* printf("applied (%i) && (match.partCount (%i) > 0 || match.cancel (%i))\n", applied, match.partCount, match.cancel); */
        if (applied && (match.partCount > 0 || match.cancel)) {
          if (match.cancel) {
            cancel = 1;
          }
          applyMatch(rt, &match);
        } else {
          applied = 0;
        }
      }
      attempts++;
    }
    if (attempts >= maxAttempts) {
      printf("warn: max attempts reached\n");
    }
  }
  return cancel;
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
      int x = rt->objects[rt->toMove[i].objIndex].x;
      int y = rt->objects[rt->toMove[i].objIndex].y;

      int layerIndex = objectLayer(rt->objects[rt->toMove[i].objIndex].objId);
      int movingToX = x + deltaX(rt->toMove[i].direction);
      int movingToY = y + deltaY(rt->toMove[i].direction);

      if (rt->objects[rt->toMove[i].objIndex].deleted == 0 && moveApplied[i] == 0 && isMovable(rt, movingToX, movingToY, layerIndex) == 1) {
        rt->objects[rt->toMove[i].objIndex].x += deltaX(rt->toMove[i].direction);
        rt->objects[rt->toMove[i].objIndex].y += deltaY(rt->toMove[i].direction);
        if (aliasLegendContains(aliasLegendId("Player"), rt->objects[rt->toMove[i].objIndex].objId)) {
          playerMoved = 1;
        }
        moveApplied[i] = 1;
        somethingApplied = 1;
      }
    }
  }
  rt->toMoveCount = 0;
  return playerMoved;
}

void printHistory(Runtime * rt) {
  for (int i = 0; i < rt->historyCount; i++) {
    printf("%s\n", directionName(rt->history[i]));
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
  } else if (input == 'x') {
    return USE;
  } else if (input == 'q') {
    return QUIT;
  } else if (input == 'r') {
    return RESTART;
  } else if (input == 'z') {
    return UNDO;
  } else {
    return NONE;
  }
}

void markPlayerAsMoving(Runtime * rt, Direction dir) {
  int legendId = aliasLegendId("Player");

  for (int i = 0; i < rt->objectCount; i++) {
    if (aliasLegendContains(legendId, rt->objects[i].objId)) {
      addToMove(rt, NONE, i, dir);
    }
  }
}

void addHistory(Runtime * rt, Direction dir) {
  if (rt->historyCount == rt->historyCapacity) {
    printf("history realloc\n");
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
  if (rt->statesCount < rt->statesCapacity) {

  } else {
    printf("states realloc\n");
    rt->statesCapacity += 50;
    rt->states = realloc(rt->states, sizeof(State) * rt->statesCapacity);
  }

  rt->states[rt->statesCount].levelIndex = rt->levelIndex;
  rt->states[rt->statesCount].objectCount = rt->objectCount;
  rt->states[rt->statesCount].objectCapacity = rt->objectCapacity;

  rt->states[rt->statesCount].objects = malloc(sizeof(Obj) * rt->objectCapacity);
  memcpy(rt->states[rt->statesCount].objects, rt->objects, sizeof(Obj) * rt->objectCapacity);

  rt->statesCount++;
}

void update(Runtime * rt, Direction dir) {
  int cancel;
  int startingPlayerLocation = playerLocation(rt);
  addHistory(rt, dir);
  addState(rt);

  if (dir == RESTART) {
    rt->historyCount = rt->prevHistoryCount;
    loadLevel(rt);
    return;
  }

  // TODO: remove deleted objects?
  if (rt->levelType == SQUARES) {
    // mark player for moving
    markPlayerAsMoving(rt, dir);

    // apply rules
    if (applyRules(rt, NORMAL)) {
      cancel = 1;
    }
    // apply marked for move
    moveObjects(rt);
    rt->toMoveCount = 0;

    // apply late rules
    if (applyRules(rt, LATE)) {
      cancel = 1;
    }

    if (requirePlayerMovement() &&
        (startingPlayerLocation == playerLocation(rt) && cancel == 0)
        ) {
      printf("undo due to lack of player movement\n");
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
        if (aliasLegendContains(containerId, rt->objects[j].objId) &&
            rt->objects[i].x == rt->objects[j].x &&
            rt->objects[i].y == rt->objects[j].y) {
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
    return verifyAll(rt, winCondition(winConditionIndex)->winIdentifier, winCondition(winConditionIndex)->onIndentifier, winCondition(winConditionIndex)->hasOnQualifier);
  case SOME:
    return verifyOne(rt, winCondition(winConditionIndex)->winIdentifier, winCondition(winConditionIndex)->onIndentifier, winCondition(winConditionIndex)->hasOnQualifier);
  case NO:
    return verifyNone(rt, winCondition(winConditionIndex)->winIdentifier, winCondition(winConditionIndex)->onIndentifier, winCondition(winConditionIndex)->hasOnQualifier);
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
    if (satisfied == 0) {
      return 0;
    }
  }

  return satisfied;
}
