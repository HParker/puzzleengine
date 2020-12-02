// Interface to puzzle data extracted from the puzzlescript file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "puzzleData.h"
#include "parser.h"

void incObject() {
  if (pd.objectCount + 1 == pd.objectCapacity) {
    printf("object REALLOC\n");
    pd.objectCapacity += 50;
    pd.objects = realloc(pd.objects, sizeof(Object) * pd.objectCapacity);
    for (int i = pd.objectCount + 1; i < pd.objectCapacity; i++) {
      pd.objects[i].colorCount = 0;
      pd.objects[i].height = 0;
      pd.objects[i].width = 0;
    }
  }
  pd.objectCount++;
}

void incAliasLegend() {
  if (pd.aliasLegendCount + 1 == pd.aliasLegendCapacity) {
    printf("aliasLegend REALLOC\n");
    pd.aliasLegendCapacity += 50;
    pd.aliasLegend = realloc(pd.aliasLegend, sizeof(Legend) * pd.aliasLegendCapacity);
  }
  pd.aliasLegendCount++;
}

void incGlyphLegend() {
  if (pd.glyphLegendCount + 1 == pd.glyphLegendCapacity) {
    printf("glyphLegend REALLOC\n");
    pd.glyphLegendCapacity += 50;
    pd.glyphLegend = realloc(pd.glyphLegend, sizeof(Legend) * pd.glyphLegendCapacity);

    for (int i = pd.glyphLegendCount + 1; i < pd.glyphLegendCapacity; i++) {
      pd.glyphLegend[i].objectCount = 0;
      pd.glyphLegend[i].objectRelation = LEGEND_RELATION_UNKNOWN;
    }
  }
  pd.glyphLegendCount++;
}

void incLayer() {
  if (pd.layerCount + 1 == pd.layerCapacity) {
    printf("layer REALLOC\n");
    pd.layerCapacity += 50;
    pd.layers = realloc(pd.layers, sizeof(Layer) * pd.layerCapacity);
    for (int i = pd.layerCount + 1; i < pd.layerCapacity; i++) {
      pd.layers[i].width = 0;
    }
  }
  pd.layerCount++;
}

void incRule() {
  if (pd.ruleCount + 1 == pd.ruleCapacity) {
    printf("RULE REALLOC\n");
    pd.ruleCapacity += 50;
    pd.rules = realloc(pd.rules, sizeof(Rule) * pd.ruleCapacity);
  }
  pd.ruleCount++;
}

void incWinCondition() {
  if (pd.winConditionCount + 1 == pd.winConditionCapacity) {
    printf("winCondition REALLOC\n");
    pd.winConditionCapacity += 50;
    pd.winConditions = realloc(pd.winConditions, sizeof(WinCondition) * pd.winConditionCapacity);
  }
  pd.winConditionCount++;
}

void incLevel() {
  if (pd.levelCount + 1 == pd.levelCapacity) {
    printf("level REALLOC\n");
    pd.levelCapacity += 50;
    pd.levels = realloc(pd.levels, sizeof(Level) * pd.levelCapacity);
    for (int i = pd.levelCount + 1; i < pd.levelCapacity; i++) {
      pd.levels[i].cellIndex = 0;
      pd.levels[i].height = 0;
      pd.levels[i].width = 0;
    }
  }
  pd.levelCount++;
}


void initStarterObjects() {
  pd.aliasLegend[pd.aliasLegendCount].key = "...";
  pd.aliasLegend[pd.aliasLegendCount].objectCount = 1;
  pd.aliasLegend[pd.aliasLegendCount].objects[0].id = pd.objectCount;
  pd.objects[pd.objectCount].name = "_Spread_";
  incObject();
  incAliasLegend();

  pd.aliasLegend[pd.aliasLegendCount].key = "_EMPTY_";
  pd.aliasLegend[pd.aliasLegendCount].objectCount = 1;
  pd.aliasLegend[pd.aliasLegendCount].objects[0].id = pd.objectCount;
  pd.objects[pd.objectCount].name = "_Empty_";
  incObject();
  incAliasLegend();
}

void initPuzzleData() {
  pd.noAction = 0;
  pd.noRepeatAction = 0;
  pd.noUndo = 0;
  pd.noRestart = 0;
  pd.requirePlayerMovement = 0;
  pd.throttleMovement = 0;
  pd.runRulesOnLevelStart = 0;

  pd.objectCount = 0;
  pd.objectCapacity = 100;
  pd.objects = malloc(sizeof(Object) * pd.objectCapacity);
  for (int i = 0; i < pd.objectCapacity; i++) {
    pd.objects[i].colorCount = 0;
    pd.objects[i].height = 0;
    pd.objects[i].width = 0;
  }

  pd.aliasLegendCount = 0;
  pd.aliasLegendCapacity = 1000;
  pd.aliasLegend = malloc(sizeof(Legend) * pd.aliasLegendCapacity);
  for (int i = 0; i < pd.aliasLegendCapacity; i++) {
    pd.aliasLegend[i].objectCount = 0;
    pd.aliasLegend[i].objectRelation = LEGEND_RELATION_UNKNOWN;
    for (int j = 0; j < 100; j++) {
      pd.aliasLegend[i].objects[j].id = -1;
    }
  }

  pd.glyphLegendCount = 0;
  pd.glyphLegendCapacity = 100;
  pd.glyphLegend = malloc(sizeof(Legend) * pd.glyphLegendCapacity);
  for (int i = 0; i < pd.glyphLegendCapacity; i++) {
    pd.glyphLegend[i].objectCount = 0;
    pd.glyphLegend[i].objectRelation = LEGEND_RELATION_UNKNOWN;
  }


  pd.layerCount = 0;
  pd.layerCapacity = 100;
  pd.layers = malloc(sizeof(Layer) * pd.layerCapacity);
  for (int i = 0; i < pd.layerCapacity; i++) {
    pd.layers[i].width = 0;
  }

  pd.ruleCount = 0;
  pd.ruleCapacity = 100;
  pd.rules = malloc(sizeof(Rule) * pd.ruleCapacity);
  for (int i = 0; i < pd.ruleCapacity; i++) {
    pd.rules[i].cancel = 0;
    pd.rules[i].matchStateCount = 0;
    pd.rules[i].resultStateCount = 0;
    pd.rules[i].matchStateDone = 0;
    pd.rules[i].directionConstraint = NONE;
    pd.rules[i].executionTime = NORMAL;
    for (int j = 0; j < 100; j++) {
      pd.rules[i].matchStates[j].partCount = 0;
      pd.rules[i].resultStates[j].partCount = 0;
      for (int k = 0; k < 100; k++) {
        pd.rules[i].matchStates[j].parts[k].ruleIdentityCount = 0;
        pd.rules[i].resultStates[j].parts[k].ruleIdentityCount = 0;
        for (int x = 0; x < 100; x++) {
          pd.rules[i].resultStates[j].parts[k].ruleIdentity[x].direction = NONE;
          pd.rules[i].resultStates[j].parts[k].ruleIdentity[x].legendId = -1;
        }
      }
    }
  }

  pd.winConditionCount = 0;
  pd.winConditionCapacity = 100;
  pd.winConditions = malloc(sizeof(WinCondition) * pd.winConditionCapacity);

  pd.levelCount = 0;
  pd.levelCapacity = 1000;
  pd.levels = malloc(sizeof(Level) * pd.levelCapacity);
  for (int i = 0; i < pd.levelCapacity; i++) {
    pd.levels[i].cellIndex = 0;
    pd.levels[i].height = 0;
    pd.levels[i].width = 0;
  }

  initStarterObjects();
}

PuzzleData * parsePuzzle(FILE * file) {
  initPuzzleData();
  yyin = file;
  yyparse();

  return &pd;
}

void freePuzzle() {
  free(pd.objects);
  free(pd.aliasLegend);
  free(pd.glyphLegend);
  free(pd.layers);
  free(pd.rules);
  free(pd.winConditions);
}

int debug() {
  return pd.debug;
}

int requirePlayerMovement() {
  return pd.requirePlayerMovement;
}

char * objectName(int id) {
  return pd.objects[id].name;
}

int objectSpriteCell(int id, int i) {
  return pd.objects[id].sprite[i];
}

char * objectColor(int id, int index) {
  if (index < pd.objects[id].colorCount) {
    return pd.objects[id].colors[index];
  } else {
    printf("Color out of bounds\n");
    return "!";
  }

}

char objectGlyph(int objId) {
  for (int i = 0; i < pd.glyphLegendCount; i++) {
    if (glyphLegendObjectCount(i) == 1 && glyphLegendContains(i, objId) == 1) {
      return pd.glyphLegend[i].key;
    }
  }
  printf("err: no key found '%s' (%i)\n", objectName(objId), objId);
  return '!';
}

int legendIdForGlyph(char glyph) {
  for (int i = 0; i < pd.glyphLegendCount; i++) {
    if (toupper(pd.glyphLegend[i].key) == toupper(glyph)) {
      return i;
    }
  }
  printf("err: no id found '%c'\n", glyph);
  return -1;
}

int layerCount() {
  return pd.layerCount;
}

int aliasLegendObjectCount(int id) {
  return pd.aliasLegend[id].objectCount;
}

int glyphLegendObjectCount(int id) {
  return pd.glyphLegend[id].objectCount;
}

int aliasLegendObjectId(int legendId, int objectIndex) {
  return pd.aliasLegend[legendId].objects[objectIndex].id;
}

int glyphLegendObjectId(int legendId, int objectIndex) {
  return pd.glyphLegend[legendId].objects[objectIndex].id;
}

int aliasLegendContains(int legendId, int objId) {
  int count = aliasLegendObjectCount(legendId);
  for (int i = 0; i < count; i++) {
    if (objId == aliasLegendObjectId(legendId, i)) {
      return 1;
    }
  }
  return 0;
}

int glyphLegendContains(int legendId, int objId) {
  int count = glyphLegendObjectCount(legendId);
  for (int i = 0; i < count; i++) {
    if (objId == glyphLegendObjectId(legendId, i)) {
      return 1;
    }
  }
  return 0;
}

int idForGlyph(char glyph) {
  for (int i = 0; i < pd.glyphLegendCount; i++) {
    if (toupper(pd.glyphLegend[i].key) == toupper(glyph)) {
      if (pd.glyphLegend[i].objectCount > 1) {
        printf("err: multi object key '%c'\n", glyph);
      } else {
        return pd.glyphLegend[i].objects[0].id;
      }
    }
  }
  printf("err: no id found '%c'\n", glyph);
  return -1;
}

int objectLayer(int objId) {
  if (aliasLegendContains(aliasLegendId("_Empty_"), objId) || objId == -1) {
    // TODO: we should catch this earlier
    return -1;
  }

  for (int i = 0; i < pd.layerCount; i++) {
    for (int j = 0; j < pd.layers[i].width; j++) {
      if (objId == pd.layers[i].objectIds[j]) {
        return i;
      }
    }
  }
  printf("err: layer not found for objid: '%s' (%i) \n", objectName(objId), objId);
  return -1;
}

int levelHeight(int levelIndex) {
  if (levelIndex < pd.levelCount) {
    return pd.levels[levelIndex].height;
  } else {
    printf("err: asked for level out of bounds\n");
    return -1;
  }
}

int levelWidth(int levelIndex) {
  if (levelIndex < pd.levelCount) {
    return pd.levels[levelIndex].width;
  } else {
    printf("err: asked for level out of bounds\n");
    return -1;
  }
}

int levelCellCount(int levelIndex) {
  return pd.levels[levelIndex].cellIndex;
}

int levelCell(int levelIndex, int cellIndex) {
  return pd.levels[levelIndex].cells[cellIndex];
}

LevelType levelType(int levelIndex) {
  return pd.levels[levelIndex].levelType;
}

char * levelMessage(int levelIndex) {
  return pd.levels[levelIndex].message;
}

int layerIncludes(int layerId, int objId) {
  for (int i = 0; i < pd.layers[layerId].width; i++) {
    if (pd.layers[layerId].objectIds[i] == objId) {
      return 1;
    }
  }
  return 0;
}

int objectCount() {
  return pd.objectCount;
}

int aliasLegendCount() {
  return pd.aliasLegendCount;
}

int glyphLegendCount() {
  return pd.glyphLegendCount;
}

char * aliasLegendKey(int id) {
  return pd.aliasLegend[id].key;
}

char glyphLegendKey(int id) {
  return pd.glyphLegend[id].key;
}


int ruleCount() {
  return pd.ruleCount;
}

Rule * rule(int index) {
  return &pd.rules[index];
}


int levelCount() {
  return pd.levelCount;
}

int winConditionCount() {
  return pd.winConditionCount;
}

WinCondition * winCondition(int winConditionIndex) {
  return &pd.winConditions[winConditionIndex];
}
