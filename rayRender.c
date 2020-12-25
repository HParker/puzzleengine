#include <stdlib.h>
#include <string.h>
#include<stdio.h>
#include <unistd.h>
#include <raylib.h>
#include "puzzleData.h"
#include "alphabet.c"

#define TRANSPARENT CLITERAL(Color){ 0, 0, 0, 0 }           // Blank (Transparent)

const char * colorNames[] =
  {
   "black",
   "white",
   "lightgray",
   "lightgrey",
   "gray",
   "grey",
   "darkgray",
   "darkgrey",
   "red",
   "darkred",
   "lightred",
   "brown",
   "darkbrown",
   "lightbrown",
   "orange",
   "yellow",
   "green",
   "darkgreen",
   "lightgreen",
   "blue",
   "lightblue",
   "darkblue",
   "purple",
   "pink",
   "transparent"
  };

const Color colors[] =
  {
   BLACK,
   WHITE,
   LIGHTGRAY,
   LIGHTGRAY,
   GRAY,
   GRAY,
   DARKGRAY,
   DARKGRAY,
   RED,
   MAROON,
   PINK,
   BROWN,
   DARKBROWN,
   YELLOW,
   ORANGE,
   YELLOW,
   GREEN,
   DARKGREEN,
   LIME,
   BLUE,
   SKYBLUE,
   DARKBLUE,
   PURPLE,
   PINK,
   TRANSPARENT
  };

const int colorCount = 25;

Color colorFromName(char * name) {
  if (name[0] == '#') {
    char *str = name;
    str++;
    Color color;
    int r, g, b;
    sscanf(str, "%02x%02x%02x", &r, &g, &b);
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = 255;
    return color;
  }

  for (int i = 0; i < colorCount; i++) {
    if (strcasecmp(colorNames[i], name) == 0) {
      return colors[i];
    }
  }
  fprintf(stderr, "NO COLOR FOUND FOR NAME '%s'\n", name);
  return PINK;
}

Color colorFromSprite(Runtime * rt, int objId, int cellIndex) {
  int cell = objectSpriteCell(objId, cellIndex);
  switch (cell) {
  case '0':
    return colorFromName(objectColor(objId, 0));
    break;

  case '1':
    return colorFromName(objectColor(objId, 1));

  case '2':
    return colorFromName(objectColor(objId, 2));

  case '3':
    return colorFromName(objectColor(objId, 3));

  case '4':
    return colorFromName(objectColor(objId, 4));

  case '5':
    return colorFromName(objectColor(objId, 5));

  case '6':
    return colorFromName(objectColor(objId, 6));

  case '7':
    return colorFromName(objectColor(objId, 7));

  case '8':
    return colorFromName(objectColor(objId, 8));

  case '9':
    return colorFromName(objectColor(objId, 9));

  case '.':
    return TRANSPARENT;

  }
  fprintf(stderr, "FAILED TO MATCH A COLOR CODE id %i (%c)\n", objId, cell);
  for (int i = 0; i < 25; i++) {
    fprintf(stderr, "%c", objectSpriteCell(objId, i));
  }
  fprintf(stderr, "\n");
  return PINK;
}

#define WINDOW_SIZE 840

void initRenderer() {
  InitWindow(WINDOW_SIZE, WINDOW_SIZE, "My Puzzle");
  SetTargetFPS(60);
}

void closeRenderer() {
  CloseWindow();
}

void renderBackground(Runtime * rt) {
  int pixelSize;
  if (rt->width > rt->height) {
    pixelSize = WINDOW_SIZE / (rt->width * 5);
  } else {
    pixelSize = WINDOW_SIZE / (rt->height * 5);
  }

  int width = 5;
  int height = 5;
  int backgroundId = aliasLegendObjectId(aliasLegendId("Background"), 0);

  int count = levelCellCount(rt->levelIndex);
  for (int cell = 0; cell < count; cell++) {
    for (int i = 0; i < 25; i++) {
      Color cellColor = colorFromSprite(rt, backgroundId, i);

      if (cellColor.a != 0) {
        int x = (cell % rt->width * pixelSize * width) + ((i % 5) * pixelSize);
        int y = (cell / rt->width * pixelSize * height) + ((i / 5) * pixelSize);
        int w = pixelSize;
        int h = pixelSize;

        DrawRectangle(x, y, w, h, cellColor);
      }
    }
  }
}

void drawObj(Runtime * rt, int objIndex) {
  if (rt->objects[objIndex].deleted == 1) {
    return;
  }

  int pixelSize;
  if (rt->width > rt->height) {
    pixelSize = WINDOW_SIZE / (rt->width * 5);
  } else {
    pixelSize = WINDOW_SIZE / (rt->height * 5);
  }

  int width = 5;
  int height = 5;
  int objectId = rt->objects[objIndex].objId;

  // TODO: for now app sprites are 25 long, but we can make this more generic
  for (int i = 0; i < 25; i++) {
    if (objectId != 2) {
      Color cellColor = colorFromSprite(rt, objectId, i);
      if (cellColor.a != 0) {
        int x = (rt->objects[objIndex].x * pixelSize * width) + ((i % 5) * pixelSize);
        int y = (rt->objects[objIndex].y * pixelSize * height) + ((i / 5) * pixelSize);
        int w = pixelSize;
        int h = pixelSize;
        DrawRectangle(x, y , w, h, cellColor);
      }
    }
  }
}

void renderLevel(Runtime * rt) {
  int objLayer;
  renderBackground(rt);
  int count = layerCount();
  for (int layer = 0; layer < count; layer++) {
    for (int i = 0; i < rt->objectCount; i++) {
      objLayer = objectLayer(rt->objects[i].objId);
      if (layer == objLayer) {
        drawObj(rt, i);
      }
    }
  }
}

void renderMessage(Runtime * rt) {
  char * message = levelMessage(rt->levelIndex);
  int textLength = MeasureText(message, 20);

  DrawText(message, WINDOW_SIZE / 2 - textLength / 2, WINDOW_SIZE / 2, 20, WHITE);
}

void render(Runtime * rt) {
  BeginDrawing();
  ClearBackground(BLACK);
  switch (rt->levelType) {
  case SQUARES:
    renderLevel(rt);
    break;
  case MESSAGE_TEXT:
    renderMessage(rt);
    break;
  }
  EndDrawing();
}

char input() {
  char returnKey = -1;
  if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
    returnKey = 'd';
  }
  if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
    returnKey = 'a';
  }
  if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
    returnKey = 'w';
  }
  if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
    returnKey = 's';
  }
  if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE)) {
    returnKey = 'q';
  }
  if (IsKeyPressed(KEY_R)) {
    returnKey = 'r';
  }
  if (IsKeyPressed(KEY_Z)) {
    returnKey = 'z';
  }
  if (IsKeyPressed(KEY_X)) {
    returnKey = 'x';
  }
  return returnKey;
}
