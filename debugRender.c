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
    printf("%c", map[i]);
    col++;
    if ((i + 1) % (rt->width) == 0) {
      col = 0;
      row++;
      printf("\n");
    }
  }
}

void renderMessage(Runtime * rt) {
  char * message = levelMessage(rt->levelIndex);
  int messageLength = strlen(message);

  printf("%s\n", message);
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

void initRenderer() {
    printf("OBJECTS\n");
  for (int i = 0; i < objectCount(); i++) {
    printf("(%i) '%s'\n", i, objectName(i));
  }

  printf("ALIAS LEGEND\n");
  for (int i = 0; i < aliasLegendCount(); i++) {
    printf("(%i) '%s' = ", i, aliasLegendKey(i));
    for (int j = 0; j < aliasLegendObjectCount(i); j++) {
      printf("'%i' ", aliasLegendObjectId(i, j));
    }
    printf("\n");
  }

  printf("GLYPH LEGEND\n");
  for (int i = 0; i < glyphLegendCount(); i++) {
    printf("(%i) '%c' = ", i, glyphLegendKey(i));
    int count = glyphLegendObjectCount(i);
    printf(" (count %i) ", count);
    for (int j = 0; j < count; j++) {
      printf("'%i' ", glyphLegendObjectId(i, j));
    }
    printf("\n");
  }
}

void closeRenderer() {
}

char input() {
  char rawInput[100];
  fgets(rawInput, 100, stdin);
  return rawInput[0];
}
