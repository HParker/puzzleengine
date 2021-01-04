#include <stdlib.h>
#include <string.h>
#include<stdio.h>
#include <unistd.h>
#include <raylib.h>
#include "puzzleData.h"
#include "alphabet.c"

#define TRANSPARENT CLITERAL(Color){ 0, 0, 0, 0 }           // Blank (Transparent)

char * CHEVRON_COLORS[10] = {"red", "", "", "", "", "", "", "", "", ""};

int CHEVRON_UP[25] = {
                       '.','.','.','.','.',
                       '.','.','0','.','.',
                       '.','0','.','0','.',
                       '.','.','.','.','.',
                       '.','.','.','.','.'
};


int CHEVRON_DOWN[25] = {
                         '.','.','.','.','.',
                         '.','.','.','.','.',
                         '.','0','.','0','.',
                         '.','.','0','.','.',
                         '.','.','.','.','.'
};


int CHEVRON_LEFT[25] = {
                       '.','.','.','.','.',
                       '.','.','0','.','.',
                       '.','0','.','.','.',
                       '.','.','0','.','.',
                       '.','.','.','.','.'
};


int CHEVRON_RIGHT[25] = {
                       '.','.','.','.','.',
                       '.','.','0','.','.',
                       '.','.','.','0','.',
                       '.','.','0','.','.',
                       '.','.','.','.','.'
};

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
#define SPRITE_WIDTH 5

void initRenderer() {
  InitWindow(WINDOW_SIZE, WINDOW_SIZE, "My Puzzle");
  SetTargetFPS(60);
}

void closeRenderer() {
  CloseWindow();
}

Color colorFromList(char * colors[10], char cell) {
  switch (cell) {
  case '0':
    return colorFromName(colors[0]);

  case '1':
    return colorFromName(colors[1]);

  case '2':
    return colorFromName(colors[2]);

  case '3':
    return colorFromName(colors[3]);

  case '4':
    return colorFromName(colors[4]);

  case '5':
    return colorFromName(colors[5]);

  case '6':
    return colorFromName(colors[6]);

  case '7':
    return colorFromName(colors[7]);

  case '8':
    return colorFromName(colors[8]);

  case '9':
    return colorFromName(colors[9]);

  case '.':
    return TRANSPARENT;

  }
  fprintf(stderr, "FAILED TO MATCH A COLOR CODE id (%c)\n", cell);
  return PINK;
}

int pixelSize(Runtime * rt) {
  if (rt->width > rt->height) {
    return (WINDOW_SIZE / (rt->width * SPRITE_WIDTH));
  } else {
    return (WINDOW_SIZE / (rt->height * SPRITE_WIDTH));
  }

}

int leftMargin(Runtime * rt) {
  return 0;
  /* return ((WINDOW_SIZE - (rt->width * pixelSize(rt) * SPRITE_WIDTH)) / 2); */
}

int topMargin(Runtime * rt) {
  return 0;
  /* return ((WINDOW_SIZE - (rt->height * pixelSize(rt) * SPRITE_WIDTH)) / 2); */
}



void drawSprite(Runtime * rt, int sprite[25], char * colors[10], int x, int y) {
  // TODO: for now app sprites are 25 long, but we can make this more generic
  for (int i = 0; i < 25; i++) {
    Color cellColor = colorFromList(colors, sprite[i]);
    if (cellColor.a != 0) {
      int realX = /* leftMargin(rt) + */ (x * pixelSize(rt) * SPRITE_WIDTH) + ((i % SPRITE_WIDTH) * pixelSize(rt));
      int realY = /* topMargin(rt) + */ (y * pixelSize(rt) * SPRITE_WIDTH) + ((i / SPRITE_WIDTH) * pixelSize(rt));
      DrawRectangle(realX, realY, pixelSize(rt), pixelSize(rt), cellColor);
    }
  }
}

void drawObject(Runtime * rt, int objId, int x, int y) {
  drawSprite(rt,
             object(objId)->sprite,
             object(objId)->colors,
             x,
             y);
}

void drawObj(Runtime * rt, int objIndex) {
  if (rt->objects[objIndex].deleted == 1) {
    return;
  }
  drawObject(rt, rt->objects[objIndex].objId, rt->objects[objIndex].x, rt->objects[objIndex].y);
}

void renderBackground(Runtime * rt) {
  int backgroundId = aliasLegendObjectId(aliasLegendId("Background"), 0);

  int count = levelCellCount(rt->levelIndex);
  for (int cell = 0; cell < count; cell++) {
    int x = cell % rt->width;
    int y = cell / rt->width;
    drawObject(rt, backgroundId, x, y);
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

void drawMatch(Runtime * rt, Match * match) {
  printf("Drawing match %i\n", match->partCount);
  for (int i = 0; i < match->partCount; i++) {

    int x = (match->parts[i].goalX * pixelSize(rt) * SPRITE_WIDTH);
    int y = (match->parts[i].goalY * pixelSize(rt) * SPRITE_WIDTH);
    int w = pixelSize(rt) * SPRITE_WIDTH;
    int h = pixelSize(rt) * SPRITE_WIDTH;
    Rectangle rect = { x, y, w, h };
    printf("Drawing match (%i, %i)\n", x, y);
    DrawRectangleLinesEx(rect, pixelSize(rt)/5, RED);

    switch (match->parts[i].goalDirection) {
    case LEFT:
      drawSprite(rt, CHEVRON_LEFT, CHEVRON_COLORS, match->parts[i].goalX, match->parts[i].goalY);
      break;
    case RIGHT:
      drawSprite(rt, CHEVRON_RIGHT, CHEVRON_COLORS, match->parts[i].goalX, match->parts[i].goalY);
      break;
    case UP:
      drawSprite(rt, CHEVRON_UP, CHEVRON_COLORS, match->parts[i].goalX, match->parts[i].goalY);
      break;
    case DOWN:
      drawSprite(rt, CHEVRON_DOWN, CHEVRON_COLORS, match->parts[i].goalX, match->parts[i].goalY);
      break;
    }
  }
}

void debugRender(Runtime * rt, Match * match) {
  if (match->partCount > 0) {
    int pressed = 0;
    while (pressed == 0) {
      if (IsKeyPressed(KEY_PERIOD)) {
        pressed = 1;
      }
      BeginDrawing();
      ClearBackground(PINK);
      switch (rt->levelType) {
      case SQUARES:
        renderLevel(rt);
        break;
      case MESSAGE_TEXT:
        renderMessage(rt);
        break;
      }

      // TODO: give this a new prelude tag
      if (verboseLogging()) {
        drawMatch(rt, match);
      }

      EndDrawing();
    }
  }
}


void render(Runtime * rt) {
  BeginDrawing();
  ClearBackground(MAROON);

  switch (rt->levelType) {
  case SQUARES:
    renderLevel(rt);
    break;
  case MESSAGE_TEXT:
    renderMessage(rt);
    break;
  }
  DrawRectangle(0, 0, 10, 10, BLACK);
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
