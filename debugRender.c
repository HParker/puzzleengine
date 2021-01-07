#include <stdio.h>
#include <string.h>
#include "puzzleData.h"

char charForLoc(Runtime * rt, int loc) {
  int backgroundId = aliasLegendObjectId(aliasLegendId("Background"), 0);
  int maxHeight = -1;
  int id = -1;
  int currentHeight;
  for (int i = 0; i < rt->objectCount; i++) {
    currentHeight = objectLayer(rt->objects[i].objId);
    int objectLoc = (rt->objects[i].y * rt->width) + rt->objects[i].x;
    if (rt->objects[i].deleted == 0 && objectLoc == loc && currentHeight > maxHeight) {
      maxHeight = currentHeight;
      id = rt->objects[i].objId;
    }
  }
  if (id == -1) {
    return objectGlyph(backgroundId);
  }
  return objectGlyph(id);
}

void renderLevel(Runtime * rt) {
  // build
  int row = 0;
  int col = 0;
  int count = levelCellCount(rt->levelIndex);
  char map[count];
  for (int i = 0; i < count; i++) {
    map[i] = charForLoc(rt, i);
  }

  // draw
  for (int i = 0; i < count; i++) {
    fprintf(stderr, "%c", map[i]);
    col++;
    if ((i + 1) % (rt->width) == 0) {
      col = 0;
      row++;
      fprintf(stderr, "\n");
    }
  }
}

void renderMessage(Runtime * rt) {
  char * message = levelMessage(rt->levelIndex);
  fprintf(stderr, "%s\n", message);
}

void render(Runtime * rt) {
  switch (rt->levelType) {
  case SQUARES:
    renderLevel(rt);
    break;
  case MESSAGE_TEXT:
    renderMessage(rt);
    break;
  }
}

void debugRender(Runtime * rt) {
  render(rt);
}


void initRenderer() {
    fprintf(stderr, "OBJECTS\n");
  for (int i = 0; i < objectCount(); i++) {
    fprintf(stderr, "(%i) '%s'\n", i, objectName(i));
  }

  fprintf(stderr, "ALIAS LEGEND\n");
  for (int i = 0; i < aliasLegendCount(); i++) {
    fprintf(stderr, "(%i) '%s' = ", i, aliasLegendKey(i));
    for (int j = 0; j < aliasLegendObjectCount(i); j++) {
      fprintf(stderr, "'%i' ", aliasLegendObjectId(i, j));
    }
    fprintf(stderr, "\n");
  }

  fprintf(stderr, "GLYPH LEGEND\n");
  for (int i = 0; i < glyphLegendCount(); i++) {
    fprintf(stderr, "(%i) '%c' = ", i, glyphLegendKey(i));
    int count = glyphLegendObjectCount(i);
    fprintf(stderr, " (count %i) ", count);
    for (int j = 0; j < count; j++) {
      fprintf(stderr, "'%i' ", glyphLegendObjectId(i, j));
    }
    fprintf(stderr, "\n");
  }

  fprintf(stderr, "LAYERS LEGEND\n");
  for (int i = 0; i < layerCount(); i++) {
    fprintf(stderr, "(%i) = ", i);
    int count = layerWidth(i);
    fprintf(stderr, "(count %i) ", count);
    for (int j = 0; j < count; j++) {
      fprintf(stderr, "'%i' ", layerObjectId(i, j));
    }
    fprintf(stderr, "\n");
  }
}

void closeRenderer() {
}

char input() {
  char rawInput[100];
  fgets(rawInput, 100, stdin);
  return rawInput[0];
}
