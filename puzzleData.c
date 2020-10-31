// Interface to puzzle data extracted from the puzzlescript file
#include <stdio.h>
#include <string.h>
#include "puzzleData.h"
#include "parser.h"

void parsePuzzle(PuzzleData * puzzDat, FILE * file) {
  yyin = file;
  yyparse();
  puzzDat = &pd;
}

char * objectName(int id) {
  return pd.objects[id].name;
}

char objectGlyph(int objId) {
  for (int i = 0; i < pd.legendCount; i++) {
    for (int j = 0; j < pd.legend[i].objectCount; j++) {
      if (pd.legend[i].hasStringKey == 0 && pd.legend[i].objectValues[j].id == objId && pd.legend[i].objectCount == 1) {
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

LegendValue legendObject(int legendId, int objectIndex) {
  return pd.legend[legendId].objectValues[objectIndex];
}

Legend legend(int legendId) {
  return pd.legend[legendId];
}

int legendContains(int legendId, int objId) {
  int recursingValue;
  LegendValue lv;
  int count = legendObjectCount(legendId);
  for (int i = 0; i < count; i++) {
    lv = legendObject(legendId, i);
    if (lv.id == objId) {
      return 1;
    }
    if (lv.isLegend == 1) {
      // TODO: this assumes an `OR` condition used in the legend
      recursingValue = legendContains(lv.id, objId);
      if (recursingValue == 1) {
        return 1;
      }
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
      if (pd.layers[i].objectIds[j] == objId) {
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

int layerIncludes(int layerId, int objId) {
  for (int i = 0; i < pd.layers[layerId].width; i++) {
    if (pd.layers[layerId].objectIds[i] == objId) {
      return 1;
    }
  }
  return 0;
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
