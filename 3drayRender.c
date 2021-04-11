#include "alphabet.c"
#include "puzzleData.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TRANSPARENT                                                            \
  CLITERAL(Color) { 0, 0, 0, 0 } // Blank (Transparent)

const char *colorNames[] = {
    "black",     "white",    "lightgray", "lightgrey",  "gray",
    "grey",      "darkgray", "darkgrey",  "red",        "darkred",
    "lightred",  "brown",    "darkbrown", "lightbrown", "orange",
    "yellow",    "green",    "darkgreen", "lightgreen", "blue",
    "lightblue", "darkblue", "purple",    "pink",       "transparent"};

const Color colors[] = {BLACK,   WHITE,    LIGHTGRAY, LIGHTGRAY, GRAY,
                        GRAY,    DARKGRAY, DARKGRAY,  RED,       MAROON,
                        PINK,    BROWN,    DARKBROWN, YELLOW,    ORANGE,
                        YELLOW,  GREEN,    DARKGREEN, LIME,      BLUE,
                        SKYBLUE, DARKBLUE, PURPLE,    PINK,      TRANSPARENT};

const int colorCount = 25;

Vector3 cubePosition = {0.0f, 0.0f, 0.0f};

Color colorFromName(char *name) {
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

Color colorFromSprite(Runtime *rt, int objId, int tileIndex) {
  int tile = objectSpriteTile(objId, tileIndex);
  switch (tile) {
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
  fprintf(stderr, "FAILED TO MATCH A COLOR CODE id %i (%c)\n", objId, tile);
  for (int i = 0; i < 25; i++) {
    fprintf(stderr, "%c", objectSpriteTile(objId, i));
  }
  fprintf(stderr, "\n");
  return PINK;
}

#define WINDOW_SIZE 840

// Define the camera to look into our 3d world
Camera3D camera = {0};

int offsetX = 0;
int offsetY = 0;
int offsetZ = 0;

void initRenderer() {
  InitWindow(WINDOW_SIZE, WINDOW_SIZE, "My Puzzle Game");
  SetTargetFPS(60);
  camera.position = (Vector3){0.0f, 0.0f, 40.0f}; // Camera position
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};    // Camera looking at point
  camera.up = (Vector3){0.0f, -1.0f,
                        0.0f}; // Camera up vector (rotation towards target)
  camera.fovy = 50.0f;         // Camera field-of-view Y
  camera.type = CAMERA_PERSPECTIVE; // Camera mode type
}

void closeRenderer() { CloseWindow(); }

void renderBackground(Runtime *rt) {
  int pixelSize = 1;
  /* if (rt->width > rt->height) { */
  /*   pixelSize = WINDOW_SIZE / (rt->width * 5); */
  /* } else { */
  /*   pixelSize = WINDOW_SIZE / (rt->height * 5); */
  /* } */

  int width = 5;
  int height = 5;

  int count = levelTileCount(rt->levelIndex);
  for (int tile = 0; tile < count; tile++) {
    for (int i = 0; i < 25; i++) {
      Color tileColor = colorFromSprite(rt, rt->backgroundId, i);

      if (tileColor.a != 0) {
        Vector3 cubePosition;
        cubePosition.x =
            -(tile % rt->width * pixelSize * width) + ((i % 5) * pixelSize);
        cubePosition.y =
            (tile / rt->width * pixelSize * height) + ((i / 5) * pixelSize);
        cubePosition.z = 0.0f;

        int w = pixelSize;
        int h = pixelSize;
        int l = pixelSize;

        DrawCube(cubePosition, w, h, l, tileColor);
      }
    }
  }
}

void drawObj(Runtime *rt, int objIndex) {
  if (rt->objects[objIndex].deleted == 1) {
    return;
  }

  int pixelSize;
  /* if (rt->width > rt->height) { */
  /*   pixelSize = WINDOW_SIZE / (rt->width * 5); */
  /* } else { */
  /*   pixelSize = WINDOW_SIZE / (rt->height * 5); */
  /* } */
  pixelSize = 1;
  // (1,1) 1 * 1 * 5 + 2
  int width = 5;
  int height = 5;
  int objectId = rt->objects[objIndex].objId;

  // TODO: for now app sprites are 25 long, but we can make this more generic
  for (int i = 0; i < 25; i++) {
    Color tileColor = colorFromSprite(rt, objectId, i);
    if (tileColor.a != 0) {
      Vector3 cubePosition;
      cubePosition.x = -(rt->objects[objIndex].x * pixelSize * width) +
                       ((i % 5) * pixelSize);
      cubePosition.y = (rt->objects[objIndex].y * pixelSize * height) +
                       ((i / 5) * pixelSize);
      cubePosition.z = objectLayer(objectId) * 2.5f;
      int w = pixelSize;
      int h = pixelSize;
      float l = pixelSize * 2.5f;

      DrawCube(cubePosition, w, h, l, tileColor);
    }
  }
}

void renderLevel(Runtime *rt) {
  int objLayer;
  renderBackground(rt);
  int count = rt->pd->layerCount;
  for (int layer = 0; layer < count; layer++) {
    for (int i = 0; i < rt->objectCount; i++) {
      objLayer = objectLayer(rt->objects[i].objId);
      if (layer == objLayer) {
        drawObj(rt, i);
      }
    }
  }
}

void renderMessage(Runtime *rt) {
  char *message = levelMessage(rt->levelIndex);
  int textLength = MeasureText(message, 20);

  DrawText(message, WINDOW_SIZE / 2 - textLength / 2, WINDOW_SIZE / 2, 20,
           BLACK);
}

void render(Runtime *rt) {
  BeginDrawing();
  ClearBackground(BLACK);

  camera.target.x = -(((rt->width * 5) / 2) - 5);
  camera.target.y = ((rt->height * 5) / 2);

  camera.position.x = -(((rt->width * 5) / 2) - 5);
  camera.position.y = ((rt->height * 5) / 2) + 55;

  switch (rt->levelType) {
  case SQUARES:
    BeginMode3D(camera);
    renderLevel(rt);
    EndMode3D();
    break;
  case MESSAGE_TEXT:
    renderMessage(rt);
    break;
  }
  EndDrawing();
}

void debugRender(Runtime *rt) { /* render(rt); */
}

char input() {
  char returnKey = -1;
  UpdateCamera(&camera);
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
