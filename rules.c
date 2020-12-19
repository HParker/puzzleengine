#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "puzzleData.h"
#include "debug.c"

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
    return USE;
  case COND_NO:
  case UNSPECIFIED:
  case STATIONARY:
    return NONE;
  case RANDOMDIR:
    return (Direction)rand() % 4;

  default:
    fprintf(stderr, "err: (absoluteDirection) unsupported direction (ad: %i rd: %i)\n", applicationDirection, ruleDir);
    return NONE;
  }
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
  }
  return 0;
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

int ruleApplies(Runtime * rt, int ruleIndex, ExecutionTime execTime) {
  // TODO: this should check global direction constraints
  if (rule(ruleIndex)->executionTime != execTime) {
    return 0;
  }
  return 1;
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

void addObj(Runtime * rt, int objId, int x, int y) {
  if (rt->objectCount + 1 >= rt->objectCapacity) {
    rt->objectCapacity += 50;
    rt->objects = realloc(rt->objects, sizeof(Obj) * rt->objectCapacity);
  }
  rt->objects[rt->objectCount].objId = objId;
  rt->objects[rt->objectCount].x = x;
  rt->objects[rt->objectCount].y = y;
  rt->objects[rt->objectCount].deleted = 0;
  rt->objectCount++;
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
          fprintf(stderr, "err: no layer id for index: %i, objid: %i, deleted: %i\n", i, rt->objects[i].objId, rt->objects[i].deleted);
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


int legendObjIndex(Runtime * rt, int legendId, int x, int y) {
  int cellIndex;
  for (int i = 0; i < layerCount(); i++) {
    cellIndex = (i * rt->width * rt->height) + (y * rt->width) + x;

    if (rt->map[cellIndex] != -1 && aliasLegendContains(legendId, rt->objects[rt->map[cellIndex]].objId)) {
      return rt->map[cellIndex];
    }
  }
  /* fprintf(stderr, "err: missed %i is in %i\n", rt->objects[rt->map[cellIndex]].objId, legendId); */
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


int matchesDirection(Direction ruleDir, Direction applicationDir, Direction dir, int ignoreUnspec) {
  if (dir == COND_NO || (ignoreUnspec && ruleDir == UNSPECIFIED)) {
    return 1;
  }
  Direction absoluteDir = absoluteDirection(applicationDir, ruleDir);
  if (absoluteDir == dir) {
    return 1;
  } else {
    return 0;
  }
}


void applyMatch(Runtime * rt, Match * match) {
  if (verboseLogging()) {
    if (match->cancel == 0) {
      fprintf(stderr, "applying count %i\n", match->partCount);
      for (int i = 0; i < match->partCount; i++) {
        if (match->parts[i].newObject == 1) {
          fprintf(stderr, "Applying rule: %i new: %i id: '%s' (%i) location (%i, %i) goalMovment: %s\n",
                 match->ruleIndex,
                 match->parts[i].newObject,
                 objectName(match->parts[i].goalId),
                 match->parts[i].goalId,
                 match->parts[i].goalX,
                 match->parts[i].goalY,
                 directionName(match->parts[i].goalDirection)
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
                 directionName(match->parts[i].goalDirection));
        }
      }
    } else {
      fprintf(stderr, "Applying rule: %i cancel\n", match->ruleIndex);
    }
  }

  int emptyId = aliasLegendId("_EMPTY_");
  if (match->cancel) {
    undo(rt, 1);
    return;
  }

  for (int i = 0; i < match->partCount; i++) {
    if (match->parts[i].newObject && match->parts[i].goalId != emptyId) {
      addObj(rt, match->parts[i].goalId, match->parts[i].goalX, match->parts[i].goalY);
      if (match->parts[i].goalDirection != -1 || match->parts[i].goalDirection != UNSPECIFIED) {
        addToMove(rt, rt->objectCount - 1,  match->parts[i].goalDirection);
      }
    } else {
      if (match->parts[i].goalId == aliasLegendId("_Empty_")) {
        rt->objects[match->parts[i].objIndex].deleted = 1;
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

Direction matchLegendDirection(int ruleId, int stateId, int partId, int legendId) {
  if (rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentityCount <= 0) {
    return UNSPECIFIED;
  }
  for (int i = 0; i < rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentityCount; i++) {
    if (rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentity[i].legendId == legendId) {
      return rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentity[i].direction;
    }
  }
  return UNSPECIFIED;
}

int resultHasLegendId(int ruleId, int stateId, int partId, int legendId, int objId) {
  if (rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentityCount <= 0) {
    return 0;
  }
  int resultLegendId;
  for (int i = 0; i < rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentityCount; i++) {
    resultLegendId = rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentity[i].legendId;
    if (legendContains(resultLegendId, objId)) {
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

void replaceCell(Runtime * rt, int ruleId, int stateId, int partId, Direction appDir, int x, int y, Match * match) {
  match->ruleIndex = ruleId;
  if (ruleCommandContains(ruleId, CANCEL)) {
    match->cancel = 1;
    return;
  }

  int isResult = alreadyResult(rt, ruleId, stateId, partId, appDir, x, y);
  int emptyId = aliasLegendId("_EMPTY_");

  // remove things in the match side
  int matchIdentCount = rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentityCount;
  for (int identId = 0; identId < matchIdentCount; identId++) {
    Direction ruleDir = rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentity[identId].direction;
    int legendId = rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentity[identId].legendId;
    int resultIncludesSelf = resultHasLegendId(ruleId, stateId, partId, legendId);
    if (ruleDir != COND_NO &&
        legendId != emptyId &&
        resultIncludesSelf == 0
        ) {
      int objIndex = legendObjIndex(rt, legendId, x, y);
      match->parts[match->partCount].newObject = 0;
      match->parts[match->partCount].goalId = emptyId;
      match->parts[match->partCount].objIndex = objIndex;
      match->parts[match->partCount].goalX = rt->objects[objIndex].x;
      match->parts[match->partCount].goalY = rt->objects[objIndex].y;
      match->parts[match->partCount].goalDirection = UNSPECIFIED;
      match->partCount++;
    }
  }

  if (isResult) {
    return;
  }

  // add the result side at this square
  int resultIdentCount = rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentityCount;
  for (int identId = 0; identId < resultIdentCount; identId++) {
    Direction ruleDir = rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentity[identId].direction;
    int legendId = rule(ruleId)->resultStates[stateId].parts[partId].ruleIdentity[identId].legendId;
    Direction matchDir = matchLegendDirection(ruleId, stateId, partId, legendId);
    if (legendId != emptyId) {
      if (ruleDir != COND_NO) {
        if (legendAt(rt, legendId, x, y) == 0) {
          match->parts[match->partCount].newObject = 1;
          // TODO: this should be ok all the time, but feels wrong.
          match->parts[match->partCount].goalId = aliasLegendObjectId(legendId, 0);
          match->parts[match->partCount].goalX = x;
          match->parts[match->partCount].goalY = y;

          match->parts[match->partCount].goalDirection = absoluteDirection(appDir, ruleDir);
          match->partCount++;
        } else {
          int objIndex = legendObjIndex(rt, legendId, x, y);
          if (ruleDir != UNSPECIFIED || matchDir != UNSPECIFIED) {
            match->parts[match->partCount].newObject = 0;
            match->parts[match->partCount].goalId = -1; // Don't change the id
            match->parts[match->partCount].objIndex = objIndex;
            match->parts[match->partCount].goalDirection = absoluteDirection(appDir, ruleDir);
            match->partCount++;
          }
        }
      }
    }
  }
}

int partIdentity(Runtime * rt, int ruleId, int stateId, int partId, int identId, Direction appDir, int x, int y, Match * match) {
  int objIndex = -1;
  int matched = 0;

  Direction ruleDir = rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentity[identId].direction;
  int legendId = rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentity[identId].legendId;

  int emptyId = aliasLegendId("_EMPTY_");
  int legAt = legendAt(rt, legendId, x, y);

  if (legendId == emptyId) {
    matched = 1;
  } else if (ruleDir == COND_NO && legAt == 0) {
    matched = 1;
  } else if (ruleDir != COND_NO && legAt) {
    objIndex = legendObjIndex(rt, legendId, x, y);
    if (objIndex != -1 && matchesDirection(ruleDir, appDir, directionMoving(rt, objIndex), 1) == 1) {
      matched = 1;
    }
  }

  return matched;
}

int partIdentitys(Runtime * rt, int ruleId, int stateId, int partId, Direction appDir, int x, int y, Match * match) {
  int success = 0;

  for (int i = 0; i < rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentityCount; i++) {
    success = partIdentity(rt, ruleId, stateId, partId, i, appDir, x, y, match);
    if (success == 0) {
      /* fprintf(stderr, "X -- FAILED rule: %i state: %i part: %i ident: %i, appDir: %s (%i, %i)\n", ruleId, stateId, partId, i, directionName(appDir), x, y); */
      return 0;
    } else {
      /* fprintf(stderr, "V --- Matched rule: %i state: %i part: %i ident: %i, appDir: %s (%i, %i)\n", ruleId, stateId, partId, i, directionName(appDir), x, y); */
    }
  }
  /* fprintf(stderr, "> --- Identity Success\n"); */
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
      if (partIdentitys(rt, ruleId, stateId, partId, appDir, currentX, currentY, match)) {
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

/* int completeMatchPart(Runtime * rt, int ruleId, int stateId, int partId, Direction appDir, int x, int y, Match * match) { */

/* } */

int completeMatch(Runtime * rt, int ruleId, int stateId, Direction appDir, int x, int y, Match * match) {
  int prevPartCount = match->partCount;
  int anyDistance = 0;
  int distance = 0;
  int dist = 0;
  match->appliedDirection = appDir;

  int success = 1;
  int currentX, currentY;
  for (int partId = 0; partId < rule(ruleId)->matchStates[stateId].partCount; partId++) {
    success = 0;
    currentX = x + (distance * deltaX(appDir));
    currentY = y + (distance * deltaY(appDir));

    if (onBoard(rt, currentX, currentY) == 0) {
      return 0;
    }

    // Little bit of a hack, but ... has to be alone in a cell, so we can try checking this way.
    int firstLegendId = rule(ruleId)->matchStates[stateId].parts[partId].ruleIdentity[0].legendId;
    if (strcmp(aliasLegendKey(firstLegendId), "...") == 0) {
      distance--;
      anyDistance = 1;
      success = 1;
    } else {
      if (anyDistance == 1) {
        dist = identitysAtDistance(rt, ruleId, stateId, partId, appDir, distance, x, y, match);
        if (dist != -1) {
          distance = dist;
          success = 1;
          anyDistance = 0;
        } else {
          success = 0;
          anyDistance = 0;
        }
      } else {
        if (partIdentitys(rt, ruleId, stateId, partId, appDir, currentX, currentY, match)) {
          success = 1;
          anyDistance = 0; // TODO: not needed
        }
      }
      currentX = x + (distance * deltaX(appDir));
      currentY = y + (distance * deltaY(appDir));
      replaceCell(rt, ruleId, stateId, partId, appDir, currentX, currentY, match);
    }

    if (success != 1) {
      // failed to find an object that matches the next part, fail
      match->partCount = prevPartCount;
      match->cancel = 0;
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
        /* fprintf(stderr, "applied (%i) && (match.partCount (%i) > 0 || match.cancel (%i))\n", applied, match.partCount, match.cancel); */
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
      fprintf(stderr, "warn: max attempts reached\n");
    }
  }
  return cancel;
}
