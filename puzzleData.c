// Interface to puzzle data extracted from the puzzlescript file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "puzzleData.h"
#include "parser.h"

void incObject() {
  if (pd.objectCount + 1 == pd.objectCapacity) {
    printf("object REALLOC\n");
    pd.objectCapacity += 50;
    pd.objects = realloc(pd.objects, sizeof(Object) * pd.objectCapacity);
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
  }
  pd.glyphLegendCount++;
}

void incLayer() {
  if (pd.layerCount + 1 == pd.layerCapacity) {
    printf("layer REALLOC\n");
    pd.layerCapacity += 50;
    pd.layers = realloc(pd.layers, sizeof(Layer) * pd.layerCapacity);
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
  pd.objectCount = 0;
  pd.objectCapacity = 1000;
  pd.objects = malloc(sizeof(Object) * pd.objectCapacity);

  pd.aliasLegendCount = 0;
  pd.aliasLegendCapacity = 1000;
  pd.aliasLegend = malloc(sizeof(Legend) * pd.aliasLegendCapacity);

  pd.glyphLegendCount = 0;
  pd.glyphLegendCapacity = 1000;
  pd.glyphLegend = malloc(sizeof(Legend) * pd.glyphLegendCapacity);

  pd.layerCount = 0;
  pd.layerCapacity = 100;
  pd.layers = malloc(sizeof(Layer) * pd.layerCapacity);

  pd.ruleCount = 0;
  pd.ruleCapacity = 100;
  pd.rules = malloc(sizeof(Rule) * pd.ruleCapacity);

  pd.winConditionCount = 0;
  pd.winConditionCapacity = 100;
  pd.winConditions = malloc(sizeof(WinCondition) * pd.winConditionCapacity);

  pd.levelCount = 0;
  pd.levelCapacity = 100;
  pd.levels = malloc(sizeof(Level) * pd.levelCapacity);

  initStarterObjects();
}

PuzzleData * parsePuzzle(FILE * file) {
  initPuzzleData();
  yyin = file;
  yyparse();

  return &pd;
}

int debug() {
  return pd.debug;
}

char * objectName(int id) {
  return pd.objects[id].name;
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
    if (pd.glyphLegend[i].key == glyph) {
      return i;
    }
  }
  printf("err: no id found '%c'\n", glyph);
  return -1;
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
    if (pd.glyphLegend[i].key == glyph) {
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
  return pd.levels[levelIndex].height;
}

int levelWidth(int levelIndex) {
  return pd.levels[levelIndex].width;
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
