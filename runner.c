#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "puzzleData.h"
#include "render.h"

#define EMPTY_ID 1

int onBoard(Runtime * rt, int x, int y) {
  if (x < 0 || x >= rt->width || y < 0 || y >= rt->height) {
    return 0;
  }
  return 1;
}

int legendObjId(Runtime * rt, int legendId, int x, int y) {
  int tileIndex;

  for (int i = 0; i < rt->pd->layerCount; i++) {
    tileIndex = (i * rt->width * rt->height) + (y * rt->width) + x;
    if (rt->map[tileIndex] != -1 &&
        aliasLegendContains(legendId, rt->objects[rt->map[tileIndex]].objId)) {
      return rt->map[tileIndex];
    }
  }
  return -1;
}

int playerLocation(Runtime * rt) {
  int x, y, byteIndex, byteOffset;
  int width = rt->width;
  int bytePerRecord = rt->pd->objectCount/8+1;
  int legendId = rt->playerId;

  for (x = 0; x < rt->width; x++) {
    for (y = 0; y < rt->height; y++) {
      byteIndex = ((y * width * bytePerRecord * 8) + (x * bytePerRecord * 8))/8;
      for (byteOffset = 0; byteOffset < bytePerRecord; byteOffset++) {
        if ((rt->pd->aliasLegend[legendId].mask[byteOffset] & rt->oMap[byteIndex + byteOffset]) != 0) {
          return (y * width + x);
        }
      }
    }
  }
  return -1;
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
  if (dir == COND_NO || (ignoreUnspec && ruleDir == UNSPECIFIED)) {
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
  /* printf("Adding %i %s\n", objIndex, dirName(direction)); */
  for (int i = 0; i < rt->toMoveCount; i++) {
    if (rt->toMove[i].objIndex == objIndex) {
      rt->objects[objIndex].moving = direction;
      return;
    }
  }

  if (rt->toMoveCount < rt->toMoveCapacity) {
  } else {
    rt->toMoveCapacity += 50;
    rt->toMove = realloc(rt->toMove, sizeof(ToMove) * rt->toMoveCapacity);
  }

  rt->toMove[rt->toMoveCount].objIndex = objIndex;
  rt->objects[objIndex].moving = direction;
  rt->toMoveCount++;
}

// MAPS
void buildOMap(Runtime * rt) {
  int width = rt->width;
  int bytePerRecord = rt->pd->objectCount/8+1;

  if (rt->hasOMap) {
    free(rt->oMap);
  }
  rt->hasOMap = 1;
  rt->oMap = calloc(rt->height * rt->width * bytePerRecord, 1);

  for (int x = 0; x < rt->width; x++) {
    for (int y = 0; y < rt->height; y++) {
      for (int layerId = 0; layerId < rt->pd->layerCount; layerId++) {
        int tileIndex = (layerId * rt->width * rt->height) + (y * rt->width) + x;
        if (rt->map[tileIndex] != -1 &&
            rt->objects[rt->map[tileIndex]].deleted == 0) {
          int element = (y * width * bytePerRecord * 8) + (x * bytePerRecord * 8) + rt->objects[rt->map[tileIndex]].objId;
          unsigned int byte_index = element/8;
          unsigned int bit_index = element % 8;
          unsigned int bit_mask = (1 << bit_index);

          rt->oMap[byte_index] |= bit_mask;
        }
      }
    }
  }
}

void buildMap(Runtime * rt) {
  if (rt->hasMap) {
    free(rt->map);
  }
  rt->hasMap = 1;
  rt->map = malloc((sizeof(int) * rt->height * rt->width * rt->pd->layerCount));

  for (int i = 0; i < (rt->pd->layerCount * rt->height * rt->width); i++) {
    rt->map[i] = -1;
  }
}

void updateObjInMap(Runtime * rt, int objIndex) {
  int layerId, x, y, tileIndex;

  layerId = objectLayer(rt->objects[objIndex].objId);
  x = rt->objects[objIndex].x;
  y = rt->objects[objIndex].y;

  tileIndex = (layerId * rt->width * rt->height) + (y * rt->width) + x;

  rt->map[tileIndex] = objIndex;

  // OMAP
  int bytePerRecord = rt->pd->objectCount/8+1;
  int element = (y * rt->width * bytePerRecord * 8) + (x * bytePerRecord * 8) + rt->objects[objIndex].objId;
  rt->oMap[element/8] |= (1 << element % 8);
}

void removeObjFromOMap(Runtime * rt, int objIndex) {
  int x, y;

  int width = rt->width;
  int bytePerRecord = rt->pd->objectCount/8+1;
  x = rt->objects[objIndex].x;
  y = rt->objects[objIndex].y;

  int element = (y * width * bytePerRecord * 8) + (x * bytePerRecord * 8) + rt->objects[objIndex].objId;

  unsigned int byte_index = element/8;
  unsigned int bit_index = element % 8;
  unsigned int bit_mask = (1 << bit_index);
  rt->oMap[byte_index] &= ~(bit_mask);
}

void removeObjFromMap(Runtime * rt, int objIndex) {
  int layerId, x, y, tileIndex;

  layerId = objectLayer(rt->objects[objIndex].objId);
  x = rt->objects[objIndex].x;
  y = rt->objects[objIndex].y;
  tileIndex = (layerId * rt->width * rt->height) + (y * rt->width) + x;

  rt->map[tileIndex] = -1;
  removeObjFromOMap(rt, objIndex);
}
// END MAPS

void addObj(Runtime * rt, int objId, int x, int y) {
  if (rt->objectCount + 1 >= rt->objectCapacity) {
    rt->objectCapacity = rt->objectCapacity * 2;
    rt->objects = realloc(rt->objects, sizeof(Obj) * rt->objectCapacity);
  }

  rt->objects[rt->objectCount].objId = objId;
  rt->objects[rt->objectCount].x = x;
  rt->objects[rt->objectCount].y = y;
  rt->objects[rt->objectCount].deleted = 0;
  rt->objects[rt->objectCount].moving = NONE;
  updateObjInMap(rt, rt->objectCount);
  rt->objectCount++;
}


void cleanup(Runtime * rt) {
  int oldObjCount = rt->objectCount;
  Obj * oldObjs = rt->objects;
  buildMap(rt);

  rt->objectCount = 0;
  rt->objects = malloc(sizeof(Obj) * rt->objectCapacity);
  for (int i = 0; i < oldObjCount; i++) {
    if (oldObjs[i].deleted == 0) {
      addObj(rt, oldObjs[i].objId, oldObjs[i].x, oldObjs[i].y);
    }
  }
  free(oldObjs);
}

void loadTile(Runtime * rt, char tile, int x, int y) {
  int id = legendIdForGlyph(tile);
  addObj(rt, rt->pd->aliasLegend[rt->backgroundId].objects[0], x, y);
  for (int i = 0; i < rt->pd->glyphLegend[id].objectCount; i++) {
    int objId = rt->pd->glyphLegend[id].objects[i];
    if (objId != -1) {
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
  rt->deadCount = 0;

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

  rt->hasMap = 0;
  rt->hasOMap = 0;
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

  if (rt->hasOMap) {
    free(rt->oMap);
  }
  rt->hasOMap = 0;

  freePuzzle();
}


void undo(Runtime * rt) {
  if (rt->didUndo) {
    // Don't undo more then once per update
    return;
  }

  if (rt->states[rt->statesCount].objectCount > 0) {
    free(rt->states[rt->statesCount].objects);
    /* free(rt->states[rt->statesCount].map); */
    /* free(rt->states[rt->statesCount].oMap); */

    rt->states[rt->statesCount].objectCount = 0;
    rt->states[rt->statesCount].objectCapacity = 0;
  }

  rt->objectCount = rt->states[rt->statesCount - 1].objectCount;
  rt->objectCapacity = rt->states[rt->statesCount - 1].objectCapacity;
  rt->objects = realloc(rt->objects, sizeof(Obj) * rt->states[rt->statesCount - 1].objectCapacity);

  rt->levelIndex = rt->states[rt->statesCount - 1].levelIndex;
  rt->levelType = levelType(rt->levelIndex);
  rt->height = rt->pd->levels[rt->levelIndex].height;
  rt->width = rt->pd->levels[rt->levelIndex].width;

  memcpy(rt->objects, rt->states[rt->statesCount - 1].objects, sizeof(Obj) * rt->objectCapacity);

  if (rt->levelType == SQUARES) {
    rt->map = rt->states[rt->statesCount - 1].map;
    rt->oMap = rt->states[rt->statesCount - 1].oMap;
  }

  rt->statesCount--;
  rt->historyCount--;
  rt->didUndo = 1;
}

int isMovable(Runtime * rt, int x, int y, int layerIndex) {
  if (onBoard(rt, x, y)) {
    if (layerIndex == -1) {
      printf("err: isMovable without a layer\n");
      return 0;
    }
    int tileIndex = (layerIndex * rt->width * rt->height) + (y * rt->width) + x;
    /* if (tileIndex > (rt->pd->layerCount * rt->width * rt->height)) */
    if (rt->map[tileIndex] == -1) {
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
}

int deltaX(Direction dir) {
  switch (dir) {
  case UP:
  case DOWN:
  case USE:
  case NONE:
    return 0;
  case LEFT:
    return -1;
  case RIGHT:
    return 1;
  default:
    fprintf(stderr, "Err: deltaX bad direction %i\n", dir);
    return 0;
  }
}

int deltaY(Direction dir) {
  switch (dir) {
  case LEFT:
  case RIGHT:
  case USE:
  case NONE:
    return 0;
  case UP:
    return -1;
  case DOWN:
    return 1;
  default:
    fprintf(stderr, "Err: deltaY bad direction %i\n", dir);
    return 0;
  }
}

// This is require for things like,
// [ Selector Block ] [ EmptyTile ] -> [ Selector ] [ FilledTile Block]
// assume Block is `Block = RedBlock or BlueBlock`
// In rules like this `Block` has to refer to the same specific block that was matched
// Even if the rule that part that matched it is in a different position.
int specificLegendId(Runtime * rt, int legendId, Match * match) {
  // TODO: we should know this when we match, we shouldn't need this
  int objId;
  for (int i = 0; i < match->partCount; i++) {
    if (match->parts[i].newObject == 0) {
      objId = rt->objects[match->parts[i].objIndex].objId;
      if (aliasLegendContains(legendId, objId)) {
        return objId;
      }
    }
  }
  return rt->pd->aliasLegend[legendId].objects[0];
}

void debugRuleApplication(Runtime * rt, Match * match) {
  printf("Rule: %s\n", ruleString(match->ruleIndex));

  /* for (int i = 0; i < match->partCount; i++) { */
  /*   printf("MatchPart %i/%i\n  new: %i, objIndex: %i, goalId: %i (%s), dir: %i, id: (%i,%i)\n", */
  /*          i, */
  /*          match->partCount, */
  /*          match->parts[i].newObject, */
  /*          match->parts[i].objIndex, */
  /*          match->parts[i].goalId, */
  /*          objectName(match->parts[i].goalId), */
  /*          match->parts[i].actualDirection, */
  /*          match->parts[i].goalX, */
  /*          match->parts[i].goalY */
  /*          ); */
  /* } */
}

void applyMatch(Runtime * rt, Match * match) {
  if (rt->pd->verboseLogging) {
    debugRuleApplication(rt, match);
  }

  if (match->cancel) {
    rt->toMoveCount = 0;
    undo(rt);
    return;
  }

  for (int i = 0; i < match->partCount; i++) {
    if (match->parts[i].newObject && match->parts[i].goalId != EMPTY_ID) {
      /* printf("%i/%i - adding object %s (%i,%i)\n", i, match->partCount, objectName(specificLegendId(rt, match->parts[i].goalId, match)), match->parts[i].goalX, match->parts[i].goalY); */

      addObj(rt, specificLegendId(rt, match->parts[i].goalId, match), match->parts[i].goalX, match->parts[i].goalY);

      if (match->parts[i].goalDirection != -1 && (match->parts[i].goalDirection != UNSPECIFIED && match->parts[i].goalDirection != NONE)) {
        /* printf("%i/%i - adding movement to new object above ^ \n", i, match->partCount); */
        addToMove(rt, rt->objectCount - 1,  match->parts[i].goalDirection);
      }
    } else {
      if (match->parts[i].goalId == EMPTY_ID) {
        /* printf("%i/%i - deleting object %s (%i,%i)\n", i, match->partCount, objectName(rt->objects[match->parts[i].objIndex].objId), match->parts[i].goalX, match->parts[i].goalY); */
        rt->objects[match->parts[i].objIndex].deleted = 1;
        removeObjFromMap(rt, match->parts[i].objIndex);
        rt->deadCount++;
      } else if (match->parts[i].goalId == -1 && match->parts[i].goalDirection != UNSPECIFIED) {
        /* printf("%i/%i - adding movement to existing object %s (%i,%i)\n", i, match->partCount, objectName(rt->objects[match->parts[i].objIndex].objId), match->parts[i].goalX, match->parts[i].goalY); */
        addToMove(rt, match->parts[i].objIndex,  match->parts[i].goalDirection);
      }
    }
  }
  if (ruleCommandContains(&rt->pd->rules[match->ruleIndex], AGAIN) && match->partCount > 0) {
    /* printf("Marking again!\n"); */
    rt->doAgain = 1;
  }
  match->partCount = 0;
}

int distance(int i, int j) {
  return abs(i - j);
}

int matchAtDistance(Direction dir, int x, int y, int targetX, int targetY) {
  switch (dir) {
  case DOWN:
    if (x == targetX && targetY > y) {
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

int directionMatches(Runtime * rt, Rule * rule, int patternId, int partId, int identId, Direction appDir, int x, int y) {
  int foundMoving = 0;
  Direction ruleDir = rule->matchPatterns[patternId].parts[partId].identity[identId].direction;
  int legendId = rule->matchPatterns[patternId].parts[partId].identity[identId].legendId;

  for (int i = 0; i < rt->toMoveCount; i++) {
    if (rt->objects[rt->toMove[i].objIndex].x == x &&
        rt->objects[rt->toMove[i].objIndex].y == y &&
        aliasLegendContains(legendId, rt->objects[rt->toMove[i].objIndex].objId)) {
      foundMoving = 1;
      if (matchesDirection(ruleDir, appDir, rt->objects[rt->toMove[i].objIndex].moving, 1)) {
        return 1;
      }
    }
  }
  if (foundMoving == 0 && (ruleDir == STATIONARY || ruleDir == UNSPECIFIED)) {
    return 1;
  }
  return 0;
}

int resultDirectionMatches(Runtime * rt, Rule * rule, int patternId, int partId, Direction appDir, int x, int y, int ignoreUnspecified) {
  int matched = 0;
  int foundMoving = 0;
  for (int identId = 0; identId < rule->resultPatterns[patternId].parts[partId].identityCount; identId++) {
    matched = 0;
    Direction ruleDir = rule->resultPatterns[patternId].parts[partId].identity[identId].direction;
    int legendId = rule->resultPatterns[patternId].parts[partId].identity[identId].legendId;

    if (legendId == EMPTY_ID || ruleDir == COND_NO) {
      return 1;
    } else {
      for (int i = 0; i < rt->toMoveCount; i++) {
        if (rt->objects[rt->toMove[i].objIndex].x == x &&
            rt->objects[rt->toMove[i].objIndex].y == y &&
            aliasLegendContains(legendId, rt->objects[rt->toMove[i].objIndex].objId)) {
          foundMoving = 1;
          if (matchesDirection(ruleDir, appDir, rt->objects[rt->toMove[i].objIndex].moving, ignoreUnspecified)) {
            matched = 1;
          }
        }
      }
      // TODO: we should track if we found it, but it was moving the wrong way above
      if (foundMoving == 0 && (ruleDir == STATIONARY || (ignoreUnspecified && ruleDir == UNSPECIFIED))) {
        matched = 1;
      }
    }
    if (matched == 0) {
      return 0;
    }
  }
  return 1;
}

int alreadyResultIdentity(Runtime * rt, Rule * rule, int patternId, int partId, int identId, Direction appDir, int x, int y) {
    // bit/byte math
  int width = rt->width;
  int bytePerRecord = rt->pd->objectCount/8+1;

  Direction ruleDir = rule->resultPatterns[patternId].parts[partId].identity[identId].direction;
  int legendId = rule->resultPatterns[patternId].parts[partId].identity[identId].legendId;

  // TODO: this shouldn't work
  //       we can't just grab the ident at the same index and get the right thing
  //       semantically it tends to work because humans are likely to list the idents in the same order,
  //       but we shouldn't rely on that behavior
  int ignoreUnspecified;
  if (ruleDir == UNSPECIFIED &&
      rule->matchPatterns[patternId].parts[partId].identity[identId].direction != UNSPECIFIED) {
    ignoreUnspecified = 0;
  } else {
    ignoreUnspecified = 1;
  }

  int matched = 0;
  if (legendId == EMPTY_ID) {
    return 1;
  } else {
    // legend mask & tile
    int byte_index = ((y * width * bytePerRecord * 8) + (x * bytePerRecord * 8))/8;

    if (ruleDir != COND_NO) {
      for (int byteOffset = 0; byteOffset < bytePerRecord; byteOffset++) {
        if ((rt->pd->aliasLegend[legendId].mask[byteOffset] & rt->oMap[byte_index + byteOffset]) != 0 &&
            (rule->executionTime == LATE || resultDirectionMatches(rt, rule, patternId, partId, appDir, x, y, ignoreUnspecified))) {
          matched = 1;
        }
      }
    } else {
      matched = 1;
      for (int byteOffset = 0; byteOffset < bytePerRecord; byteOffset++) {
        if ((rt->pd->aliasLegend[legendId].mask[byteOffset] & rt->oMap[byte_index + byteOffset]) != 0) {
          matched = 0;
        }
      }
    }
    return matched;
  }
}

Direction matchLegendDirection(Rule * rule, int patternId, int partId, int legendId) {
  if (rule->matchPatterns[patternId].parts[partId].identityCount <= 0) {
    return UNSPECIFIED;
  }
  for (int i = 0; i < rule->matchPatterns[patternId].parts[partId].identityCount; i++) {
    if (rule->matchPatterns[patternId].parts[partId].identity[i].legendId == legendId) {
      return rule->matchPatterns[patternId].parts[partId].identity[i].direction;
    }
  }
  return UNSPECIFIED;
}

int resultHasLegendId(Rule * rule, int patternId, int partId, int legendId, int objId) {
  if (rule->resultPatterns[patternId].parts[partId].identityCount == 0) {
    return 0;
  }
  int resultLegendId;
  for (int i = 0; i < rule->resultPatterns[patternId].parts[partId].identityCount; i++) {
    resultLegendId = rule->resultPatterns[patternId].parts[partId].identity[i].legendId;
    if (resultLegendId == legendId || aliasLegendContains(resultLegendId, objId)) {
      return 1;
    }
  }
  return 0;
}

int alreadyResult(Runtime * rt, Rule * rule, int patternId, int partId, Direction appDir, int x, int y) {
  if (rule->resultPatterns[patternId].parts[partId].identityCount <= 0) {
    return 0;
  }
  for (int i = 0; i < rule->resultPatterns[patternId].parts[partId].identityCount; i++) {
    if (alreadyResultIdentity(rt, rule, patternId, partId, i, appDir, x, y) == 0) {
      return 0;
    }
  }
  return 1;
}

void replaceTile(Runtime * rt, Rule * rule, int patternId, int partId, Direction appDir, Match * match) {
  if (rule->resultPatternCount == 0) {
    if (ruleCommandContains(rule, CANCEL)) {
      match->cancel = 1;
      return;
    }
    return;
  }

  // remove things in the match side
  for (int identId = 0; identId < rule->matchPatterns[patternId].parts[partId].identityCount; identId++) {
    Direction ruleDir = rule->matchPatterns[patternId].parts[partId].identity[identId].direction;
    int legendId = rule->matchPatterns[patternId].parts[partId].identity[identId].legendId;
    // TODO: we must have looked this up already, can we reuse?
    int objIndex = legendObjId(rt, legendId, match->targetX, match->targetY);

    if (objIndex != -1) {
      int resultIncludesSelf = resultHasLegendId(rule, patternId, partId, legendId, rt->objects[objIndex].objId);
      if (ruleDir != COND_NO &&
          legendId != EMPTY_ID &&
          resultIncludesSelf == 0) {
        match->parts[match->partCount].newObject = 0;
        match->parts[match->partCount].goalId = EMPTY_ID;
        match->parts[match->partCount].objIndex = objIndex;
        // TODO: get this from the board instead
        match->parts[match->partCount].objectId = rt->objects[objIndex].objId;
        match->parts[match->partCount].goalX = rt->objects[objIndex].x;
        match->parts[match->partCount].goalY = rt->objects[objIndex].y;
        match->parts[match->partCount].goalDirection = UNSPECIFIED;
        match->partCount++;
      }
    }
  }

  if (rule->resultPatternCount == 0 ||
      patternId >= rule->resultPatternCount ||
      (patternId < rule->resultPatternCount && partId >= rule->resultPatterns[patternId].partCount)) {

    return;
  } else {
    if (alreadyResult(rt, rule, patternId, partId, appDir, match->targetX, match->targetY)) {
      return;
    }
  }

  // add the result side at this square
  int resultIdentCount = rule->resultPatterns[patternId].parts[partId].identityCount;
  for (int identId = 0; identId < resultIdentCount; identId++) {
    Direction ruleDir = rule->resultPatterns[patternId].parts[partId].identity[identId].direction;
    int legendId = rule->resultPatterns[patternId].parts[partId].identity[identId].legendId;
    Direction matchDir = matchLegendDirection(rule, patternId, partId, legendId);
    int objIndex = legendObjId(rt, legendId, match->targetX, match->targetY);
    if (legendId != EMPTY_ID) {
      if (ruleDir != COND_NO) {
        if (objIndex == -1) {
          match->parts[match->partCount].newObject = 1;
          match->parts[match->partCount].goalId = legendId;
          match->parts[match->partCount].objIndex = -1;
          match->parts[match->partCount].objectId = -1;
          // TODO: we need to get this from the board
          /* match->parts[match->partCount].objectId = specificLegendId(rt, legendId, match); */
          match->parts[match->partCount].goalX = match->targetX;
          match->parts[match->partCount].goalY = match->targetY;

          match->parts[match->partCount].goalDirection = absoluteDirection(appDir, ruleDir);
          match->partCount++;
        } else {
          if ((((matchDir == UP ||
               matchDir == DOWN ||
               matchDir == LEFT ||
                matchDir == RIGHT) &&
               ruleDir == UNSPECIFIED) ||
              (ruleDir == UP ||
               ruleDir == DOWN ||
               ruleDir == LEFT ||
               ruleDir == RIGHT)) &&
              ruleDir != matchDir
              ) {
            match->parts[match->partCount].newObject = 0;
            match->parts[match->partCount].goalId = -1; //aliasLegendObjectId(legendId, 0);
            match->parts[match->partCount].goalX = match->targetX;
            match->parts[match->partCount].goalY = match->targetY;

            match->parts[match->partCount].objIndex = objIndex;
            match->parts[match->partCount].objectId = rt->objects[objIndex].objId;
            match->parts[match->partCount].goalDirection = absoluteDirection(appDir, ruleDir);
            match->partCount++;
          }
        }
      } else if (objIndex != -1) { // NOT EMPTY AND IS A COND_NO AND THE LEGEND_ID IS THERE
        match->parts[match->partCount].newObject = 0;
        match->parts[match->partCount].goalId = EMPTY_ID;
        match->parts[match->partCount].goalX = match->targetX;
        match->parts[match->partCount].goalY = match->targetY;

        match->parts[match->partCount].objIndex = objIndex;
        match->parts[match->partCount].objectId = rt->objects[objIndex].objId;
        match->parts[match->partCount].goalDirection = absoluteDirection(appDir, ruleDir);
        match->partCount++;
      }
    }
  }
}

int partIdentity(Runtime * rt, Rule * rule, int patternId, int partId, int identId, Direction appDir, int byteIndex, Match * match) {
  // bit/byte math
  int x = match->targetX;
  int y = match->targetY;
  int bytePerRecord = rt->pd->objectCount/8+1;
  Direction ruleDir = rule->matchPatterns[patternId].parts[partId].identity[identId].direction;
  int legendId = rule->matchPatterns[patternId].parts[partId].identity[identId].legendId;
  int matched = 0;
  if (legendId == EMPTY_ID) {
    return 1;
  } else {
    if (ruleDir != COND_NO) {
      for (int byteOffset = 0; byteOffset < bytePerRecord; byteOffset++) {
        if ((rt->pd->aliasLegend[legendId].mask[byteOffset] & rt->oMap[byteIndex + byteOffset]) != 0 &&
            (rule->executionTime == LATE || directionMatches(rt, rule, patternId, partId, identId, appDir, x, y))) {
          return 1;
        }
      }
    } else {
      matched = 1;
      for (int byteOffset = 0; byteOffset < bytePerRecord; byteOffset++) {
        if ((rt->pd->aliasLegend[legendId].mask[byteOffset] & rt->oMap[byteIndex + byteOffset]) != 0) {
          return 0;
        }
      }
    }
    return matched;
  }
}

int partIdentitys(Runtime * rt, Rule * rule, int patternId, int partId, Direction appDir, Match * match) {
  int width = rt->width;
  int bytePerRecord = rt->pd->objectCount/8+1;
  int x = match->targetX;
  int y = match->targetY;
  // TODO: just double checking

  if (onBoard(rt, x, y)) {
    int byteIndex = ((y * width * bytePerRecord * 8) + (x * bytePerRecord * 8))/8;
    for (int i = 0; i < rule->matchPatterns[patternId].parts[partId].identityCount; i++) {
      if  (partIdentity(rt, rule, patternId, partId, i, appDir, byteIndex, match) == 0) {
        return 0;
      }
    }
    return 1;
  }
  return 0;
}

int identitysAtDistance(Runtime * rt, Rule * rule, int patternId, int partId, Direction appDir, int distance, Match * match) {
  int dist = distance;

  while (onBoard(rt, match->targetX, match->targetY)) {
    if (partIdentitys(rt, rule, patternId, partId, appDir, match)) {
      return dist;
    }
    dist++;
    match->targetX = match->cursorX + (dist * deltaX(appDir));
    match->targetY = match->cursorY + (dist * deltaY(appDir));
  }
  return -1;
}

int completeMatch(Runtime * rt, Rule * rule, int patternId, Direction appDir, Match * match) {
  int prevPartCount = match->partCount;
  int anyDistance = 0;
  int distance = 0;
  int dist = 0;

  int success = 1;
  for (int partId = 0; partId < rule->matchPatterns[patternId].partCount; partId++) {
    success = 0;
    match->targetX = match->cursorX + (distance * deltaX(appDir));
    match->targetY = match->cursorY + (distance * deltaY(appDir));

    if (rule->matchPatterns[patternId].parts[partId].isSpread) {
      distance--;
      anyDistance = 1;
      success = 1;
    } else {
      if (anyDistance == 1) {
        dist = identitysAtDistance(rt, rule, patternId, partId, appDir, distance, match);
        if (dist != -1) {
          distance = dist;
          success = 1;
          anyDistance = 0;
        } else {
          success = 0;
          anyDistance = 0;
        }
      } else {
        if (partIdentitys(rt, rule, patternId, partId, appDir, match)) {
          success = 1;
        }
      }

      if (success) {
        int beforePartCount = match->partCount;
        replaceTile(rt, rule, patternId, partId, appDir, match);
        int afterPartCount = match->partCount;

        // TODO: this sucks
        if (rule->matchPatterns[patternId].parts[partId].identityCount > 0 && rule->resultPatternCount > 0 && rule->resultPatterns[patternId].partCount > 0 && rule->resultPatterns[patternId].parts[partId].identityCount > 0 &&
            rule->matchPatterns[patternId].parts[partId].identityCount < rule->resultPatterns[patternId].parts[partId].identityCount &&
            beforePartCount >= afterPartCount) {
          success = 0;
          match->partCount = prevPartCount;
        }
      }
    }

    if (success != 1) {
      // failed to find an object that matches the next part, fail
      /* printf("Failed to complete match setting %i to %i\n", match->partCount, prevPartCount); */
      match->partCount = prevPartCount;
      match->cancel = 0;
      return 0;
    }
    distance++;
  }
  return 1;
}

int hasSpace(Runtime * rt, Rule * rule, Pattern * pattern, Match * match) {
  int spaceRequired = 0;
  for (int p = 0; p < pattern->partCount; p++) {
    if (pattern->parts[p].isSpread == 0) {
      spaceRequired++;
    }
  }

  switch (rule->directionConstraint) {
  case DOWN:
    return (match->cursorY + spaceRequired <= rt->height);
  case RIGHT:
    return (match->cursorX + spaceRequired <= rt->width);
  default:
    printf("DON't see me ever\n");
    return 1;
  }
}

int applyPattern(Runtime * rt, Rule * rule, int patternId, Match * match) {
  int applied;
  int matched = 0;
  int prevPartCount = match->partCount;
  for (int x = 0; x < rt->width; x++) {
    for (int y = 0; y < rt->height; y++) {
      match->targetX = match->cursorX = x;
      match->targetY = match->cursorY = y;
      if (hasSpace(rt, rule, &rule->matchPatterns[patternId], match)) {
        applied = completeMatch(rt, rule, patternId, rule->directionConstraint, match);
        if (applied) {
          matched = 1;
          if (match->partCount > prevPartCount || match->cancel) {
            // under some conditions we should be able to NOT return here
            // if that is the case we can do all update in one walk of the board
            /* if (rule->matchPatterns[patternId].parts[0].identityCount < rule->resultPatterns[patternId].parts[0].identityCount) { */
            /*   return 1; */
            /* } */
            return 1;
          } else {
            match->partCount = 0;
            match->cancel = 0;
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
  match->appliedDirection = rule->directionConstraint;
  for (int i = 0; i < rule->matchPatternCount; i++) {
    if (applyPattern(rt, rule, i, match) == 0) {
      return 0;
    }
  }
  /* if (rt->pd->verboseLogging) { */
  /*   debugRender(rt, match); */
  /* } */
  return 1;
}

void applyRules(Runtime * rt, ExecutionTime execTime) {
  int applied = 1;
  int maxAttempts = 1000;
  int attempts = 0;

  Match match;
  match.partCount = 0;
  match.cancel = 0;

  for (int ruleIndex = 0; ruleIndex < rt->pd->ruleCount; ruleIndex++) {
    if (rt->pd->rules[ruleIndex].executionTime != execTime) {
      continue;
    }

    match.ruleIndex = ruleIndex;

    applied = 1;
    attempts = 0;
    while (match.cancel == 0 && applied && attempts < maxAttempts) {
      applied = 0;
      match.partCount = 0;
      match.cancel = 0;
      match.ruleIndex = ruleIndex;

      applied = applyRule(rt, &rt->pd->rules[ruleIndex], &match);

      if (applied && (match.partCount > 0 || match.cancel)) {
        applyMatch(rt, &match);
      } else {
        applied = 0;
      }
      attempts++;
    }
    if (attempts >= maxAttempts) {
      fprintf(stderr, "warn: max attempts reached\n");
    }
  }
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
      if (rt->toMove[i].objIndex != -1 && rt->objects[rt->toMove[i].objIndex].moving != UNSPECIFIED) {
        int x = rt->objects[rt->toMove[i].objIndex].x;
        int y = rt->objects[rt->toMove[i].objIndex].y;
        int layerIndex = objectLayer(rt->objects[rt->toMove[i].objIndex].objId);
        int movingToX = x + deltaX(rt->objects[rt->toMove[i].objIndex].moving);
        int movingToY = y + deltaY(rt->objects[rt->toMove[i].objIndex].moving);

        if (rt->objects[rt->toMove[i].objIndex].deleted == 0 && moveApplied[i] == 0 && isMovable(rt, movingToX, movingToY, layerIndex) == 1) {

          removeObjFromMap(rt, rt->toMove[i].objIndex);

          rt->objects[rt->toMove[i].objIndex].x = movingToX;
          rt->objects[rt->toMove[i].objIndex].y = movingToY;
          rt->objects[rt->toMove[i].objIndex].moving = NONE;

          updateObjInMap(rt, rt->toMove[i].objIndex);

          moveApplied[i] = 1;
          somethingApplied = 1;
        }
      }
    }
  }
  for (int i = 0; i < rt->toMoveCount; i++) {
    rt->objects[rt->toMove[i].objIndex].moving = NONE;
  }
  rt->toMoveCount = 0;
}

void printHistory(Runtime * rt) {
  for (int i = 0; i < rt->historyCount; i++) {
    fprintf(stderr, "%s\n", dirName(rt->history[i]));
  }
}

Direction handleInput(Runtime * rt, Direction input) {
  switch (input) {
  case DEBUGGER:
    verboseLoggingOn();
    return NONE;
  case USE:
    if (rt->pd->noAction) {
      return NONE;
    } else {
      return USE;
    }
  case RESTART:
    if (rt->pd->noRestart) {
      return NONE;
    } else {
      return RESTART;
    }
  case UNDO:
    if (rt->pd->noUndo) {
      return NONE;
    } else {
      return UNDO;
    }
  default:
    return input;
  }
}

void markPlayerAsMoving(Runtime * rt, Direction dir) {
  int x, y, byteIndex, byteOffset;
  int width = rt->width;
  int bytePerRecord = rt->pd->objectCount/8+1;
  int legendId = rt->playerId;

  for (x = 0; x < rt->width; x++) {
    for (y = 0; y < rt->height; y++) {
      byteIndex = ((y * width * bytePerRecord * 8) + (x * bytePerRecord * 8))/8;
      for (byteOffset = 0; byteOffset < bytePerRecord; byteOffset++) {
        if ((rt->pd->aliasLegend[legendId].mask[byteOffset] & rt->oMap[byteIndex + byteOffset]) != 0) {
          addToMove(rt, legendObjId(rt, legendId, x, y), dir);
        }
      }
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
    /* free(rt->states[rt->statesCount].map); */
    /* free(rt->states[rt->statesCount].oMap); */

    rt->states[rt->statesCount].objectCount = 0;
    rt->states[rt->statesCount].objectCapacity = 0;
  }

  rt->states[rt->statesCount].levelIndex = rt->levelIndex;
  rt->states[rt->statesCount].objectCount = rt->objectCount;
  rt->states[rt->statesCount].objectCapacity = rt->objectCapacity;

  rt->states[rt->statesCount].objects = malloc(sizeof(Obj) * rt->objectCapacity);
  memcpy(rt->states[rt->statesCount].objects, rt->objects, sizeof(Obj) * rt->objectCapacity);

  if (rt->levelType == SQUARES) {
    rt->states[rt->statesCount].map = malloc((sizeof(int) * rt->height * rt->width * rt->pd->layerCount));
    memcpy(rt->states[rt->statesCount].map, rt->map, (sizeof(int) * rt->height * rt->width * rt->pd->layerCount));

    int bytePerRecord = rt->pd->objectCount/8+1;
    rt->states[rt->statesCount].oMap = malloc((rt->height * rt->width * bytePerRecord));
    memcpy(rt->states[rt->statesCount].oMap, rt->oMap, (rt->height * rt->width * bytePerRecord));
  }


  rt->statesCount++;
}

void loadLevel(Runtime * rt) {
  rt->levelType = levelType(rt->levelIndex);
  if (rt->levelType == SQUARES) {
    rt->height = rt->pd->levels[rt->levelIndex].height;
    rt->width = rt->pd->levels[rt->levelIndex].width;
    rt->toMoveCount = 0;
    rt->objectCount = 0;

    buildMap(rt);

    int bytePerRecord = rt->pd->objectCount/8+1;
    if (rt->hasOMap) {
      free(rt->oMap);
    }
    rt->hasOMap = 1;
    rt->oMap = rt->oMap = calloc(rt->height * rt->width * bytePerRecord, 1);

    int count = levelTileCount(rt->levelIndex);
    for (int i = 0; i < count; i++) {
      int x = i % rt->width;
      int y = i / rt->width;
      loadTile(rt, levelTile(rt->levelIndex, i), x, y);
    }

    if (rt->pd->runRulesOnLevelStart) {
      rt->doAgain = 1;
      while (rt->doAgain == 1) {
        rt->doAgain = 0;
        applyRules(rt, NORMAL);
        moveObjects(rt);
        applyRules(rt, LATE);
      }
    }
  }
  for (int i = 0; i < rt->statesCount; i++) {
    free(rt->states[i].objects);
    rt->states[i].objectCount = 0;
    rt->states[i].objectCapacity = 0;
  }
  rt->statesCount = 0;
}

void nextLevel(Runtime * rt) {
  if (rt->levelIndex < rt->pd->levelCount - 1) {
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
  rt->backgroundId = aliasLegendId("Background");
  rt->playerId = aliasLegendId("Player");
  loadLevel(rt);

  rt->doAgain = 0;

  if (rt->pd->verboseLogging) {
    printRules();
  }
}

void preUpdate(Runtime * rt) {
  rt->doAgain = 0;
  rt->didUndo = 0;
  rt->toMoveCount = 0;
  if (rt->deadCount > 300) {
    cleanup(rt);
    rt->deadCount = 0;
  }
}

void tick(Runtime * rt) {
  preUpdate(rt);
  applyRules(rt, NORMAL);
  moveObjects(rt);
  applyRules(rt, LATE);
  updateLevel(rt);
}

void update(Runtime * rt, Direction dir) {
  if (dir == NONE) {
    return;
  } else if (dir == RESTART) {
    rt->historyCount = rt->prevHistoryCount;
    loadLevel(rt);
    return;
  }

  preUpdate(rt);
  addHistory(rt, dir);
  addState(rt);

  if (rt->levelType == SQUARES) {
    int startingPlayerLocation = playerLocation(rt);
    markPlayerAsMoving(rt, dir);

    applyRules(rt, NORMAL);
    moveObjects(rt);
    applyRules(rt, LATE);

    if (rt->pd->requirePlayerMovement &&
        (startingPlayerLocation == playerLocation(rt))) {
      undo(rt);
      return;
    }

    updateLevel(rt);
  } else {
    if (dir == USE) {
      nextLevel(rt);
    }
  }
}

int verifyOne(Runtime * rt, int legendId, int containerId, int hasOnQualifier) {
  int x, y, byteIndex, byteOffset, matched;
  int width = rt->width;
  int bytePerRecord = rt->pd->objectCount/8+1;

  for (x = 0; x < rt->width; x++) {
    for (y = 0; y < rt->height; y++) {
      matched = 0;
      byteIndex = ((y * width * bytePerRecord * 8) + (x * bytePerRecord * 8))/8;
      for (byteOffset = 0; byteOffset < bytePerRecord; byteOffset++) {
        if ((rt->pd->aliasLegend[legendId].mask[byteOffset] & rt->oMap[byteIndex + byteOffset]) != 0) {
          matched = 1;
          if (hasOnQualifier == 0) {
            return 1;
          }

        }
      }
      if (matched) {
        for (byteOffset = 0; byteOffset < bytePerRecord; byteOffset++) {
          if ((rt->pd->aliasLegend[containerId].mask[byteOffset] & rt->oMap[byteIndex + byteOffset]) != 0) {
            return 1;
          }
        }
      }

    }
  }
  return 0;
}

int verifyNone(Runtime * rt, int thingId, int containerId, int hasOnQualifier) {
  return !(verifyOne(rt, thingId, containerId, hasOnQualifier));
}

int verifyAll(Runtime * rt, int legendId, int containerId, int hasOnQualifier) {
  int x, y, byteIndex, byteOffset, thingMatched, containerMatched;
  int width = rt->width;
  int bytePerRecord = rt->pd->objectCount/8+1;

  for (x = 0; x < rt->width; x++) {
    for (y = 0; y < rt->height; y++) {
      thingMatched = 0;
      containerMatched = 0;
      byteIndex = ((y * width * bytePerRecord * 8) + (x * bytePerRecord * 8))/8;
      for (byteOffset = 0; byteOffset < bytePerRecord; byteOffset++) {
        if ((rt->pd->aliasLegend[legendId].mask[byteOffset] & rt->oMap[byteIndex + byteOffset]) != 0) {
          thingMatched = 1;
        }
      }
      if (thingMatched) {
        for (byteOffset = 0; byteOffset < bytePerRecord; byteOffset++) {
          if ((rt->pd->aliasLegend[containerId].mask[byteOffset] & rt->oMap[byteIndex + byteOffset]) != 0) {
            containerMatched = 1;
          }
        }
      }
      if (thingMatched ^ containerMatched) {
        return 0;
      }
    }
  }
  return 1;
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
