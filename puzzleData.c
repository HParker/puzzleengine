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

void incLegend() {
  if (pd.legendCount + 1 == pd.legendCapacity) {
    printf("legend REALLOC\n");
    pd.legendCapacity += 50;
    pd.legend = realloc(pd.legend, sizeof(Legend) * pd.legendCapacity);
  }
  pd.legendCount++;
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
  pd.legend[pd.legendCount].hasStringKey = 1;
  pd.legend[pd.legendCount].stringKey = "...";
  pd.legend[pd.legendCount].objectCount = 1;
  pd.legend[pd.legendCount].objectValues[0].id = pd.objectCount;
  pd.objects[pd.objectCount].name = "Spread";
  incObject();
  incLegend();
}

void initPuzzleData() {
  pd.objectCount = 0;
  pd.objectCapacity = 1000;
  pd.objects = malloc(sizeof(Object) * pd.objectCapacity);

  pd.legendCount = 0;
  pd.legendCapacity = 1000;
  pd.legend = malloc(sizeof(Legend) * pd.legendCapacity);

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
  for (int i = 0; i < pd.legendCount; i++) {
    for (int j = 0; j < pd.legend[i].objectCount; j++) {
      if (pd.legend[i].hasStringKey == 0 && legendContains(i, objId) == 1 && pd.legend[i].objectCount == 1) {
        return pd.legend[i].key;
      }
    }
  }
  printf("err: no key found '%s' (%i)\n", objectName(objId), objId);
  return '!';
}

int legendIdForGlyph(char glyph) {
  for (int i = 0; i < pd.legendCount; i++) {
    if (pd.legend[i].hasStringKey == 0 && pd.legend[i].key == glyph) {
      return i;
    }
  }
  printf("err: no id found '%c'\n", glyph);
  return -1;
}

int legendObjectCount(int id) {
  return pd.legend[id].objectCount;
}

int legendObjectId(int legendId, int objectIndex) {
  return pd.legend[legendId].objectValues[objectIndex].id;
}

Legend * legend(int legendId) {
  return &pd.legend[legendId];
}

int legendContains(int legendId, int objId) {
  int count = legendObjectCount(legendId);
  for (int i = 0; i < count; i++) {
    if (objId == legendObjectId(legendId, i)) {
      return 1;
    }
  }
  return 0;
}

int idForGlyph(char glyph) {
  for (int i = 0; i < pd.legendCount; i++) {
    if (pd.legend[i].hasStringKey == 0 && pd.legend[i].key == glyph) {
      if (pd.legend[i].objectCount > 1) {
        printf("err: multi object key '%c'\n", glyph);
      } else {
        return pd.legend[i].objectValues[0].id;
      }
    }
  }
  printf("err: no id found '%c'\n", glyph);
  return -1;
}

int objectLayer(int objId) {
  for (int i = 0; i < pd.layerCount; i++) {
    for (int j = 0; j < pd.layers[i].width; j++) {
      int legendId = pd.layers[i].objectIds[j];
      if (legendContains(legendId, objId) == 1) {
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

int legendCount() {
  return pd.legendCount;
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
