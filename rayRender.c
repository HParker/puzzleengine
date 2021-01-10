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

int windowSize() {
  return WINDOW_SIZE;
}

int pixelSize(Runtime * rt) {
  if (rt->width > rt->height) {
    return (windowSize() / (rt->width * SPRITE_WIDTH));
  } else {
    return (windowSize() / (rt->height * SPRITE_WIDTH));
  }
}

int leftMargin(Runtime * rt) {
  return ((windowSize() - (rt->width * pixelSize(rt) * SPRITE_WIDTH)) / 2);
}

int topMargin(Runtime * rt) {
  return ((windowSize() - (rt->height * pixelSize(rt) * SPRITE_WIDTH)) / 2);
}



void drawSprite(Runtime * rt, int sprite[25], char * colors[10], int x, int y) {
  // TODO: for now app sprites are 25 long, but we can make this more generic
  for (int i = 0; i < 25; i++) {
    Color cellColor = colorFromList(colors, sprite[i]);
    if (cellColor.a != 0) {
      int realX = leftMargin(rt) + (x * pixelSize(rt) * SPRITE_WIDTH) + ((i % SPRITE_WIDTH) * pixelSize(rt));
      int realY = topMargin(rt) + (y * pixelSize(rt) * SPRITE_WIDTH) + ((i / SPRITE_WIDTH) * pixelSize(rt));
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

  DrawText(message, windowSize() / 2 - textLength / 2, windowSize() / 2, 14, WHITE);
}

void drawMatch(Runtime * rt, Match * match) {
  for (int i = 0; i < match->partCount; i++) {

    int x = leftMargin(rt) + (match->parts[i].goalX * pixelSize(rt) * SPRITE_WIDTH) + ((i % SPRITE_WIDTH) * pixelSize(rt));
    int y = topMargin(rt) + (match->parts[i].goalY * pixelSize(rt) * SPRITE_WIDTH) + ((i / SPRITE_WIDTH) * pixelSize(rt));

    /* int x = (match->parts[i].goalX * pixelSize(rt) * SPRITE_WIDTH); */
    /* int y = (match->parts[i].goalY * pixelSize(rt) * SPRITE_WIDTH); */
    int w = pixelSize(rt) * SPRITE_WIDTH;
    int h = pixelSize(rt) * SPRITE_WIDTH;
    Rectangle rect = { x, y, w, h };
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

void drawCursors(Runtime * rt, Match * match) {
  Rectangle rect;
  // cursor
  rect.x = leftMargin(rt) + (match->cursorX * pixelSize(rt) * SPRITE_WIDTH);
  rect.y = topMargin(rt) + (match->cursorY * pixelSize(rt) * SPRITE_WIDTH);
  rect.width = pixelSize(rt) * SPRITE_WIDTH;
  rect.height = pixelSize(rt) * SPRITE_WIDTH;
  DrawRectangleLinesEx(rect, pixelSize(rt)/5, GREEN);

  rect.x = leftMargin(rt) + (match->targetX * pixelSize(rt) * SPRITE_WIDTH);
  rect.y = topMargin(rt) + (match->targetY * pixelSize(rt) * SPRITE_WIDTH);
  rect.width = pixelSize(rt) * SPRITE_WIDTH;
  rect.height = pixelSize(rt) * SPRITE_WIDTH;
  DrawRectangleLinesEx(rect, pixelSize(rt)/5, RED);
}

void renderRule(Match * match) {
  Color transparentBlack = { 0, 0, 0, 140 };
  DrawRectangle(0, 0, WINDOW_SIZE, 50, transparentBlack);
  char * rstr = ruleString(match->ruleIndex);
  DrawText(rstr, 10, 10, 14, WHITE);
  free(rstr);
}

void debugRender(Runtime * rt, Match * match) {
  int awaitInput = 0;
  int frameCounter = 0;
  int frameDelay = 1;
  int pressed = 0;
  if (match->partCount > 0 || 1) {
    awaitInput = 1;
  }

  if (rt->levelType == SQUARES && verboseLogging()) {
    if (awaitInput) {
      while (pressed == 0) {
        if (IsKeyPressed(KEY_PERIOD)) {
          pressed = 1;
        }
        BeginDrawing();

        ClearBackground(BLACK);

        renderLevel(rt);
        drawCursors(rt, match);
        drawMatch(rt, match);
        renderRule(match);

        EndDrawing();
      }
    } else {
      while (frameCounter < frameDelay) {
        if (IsKeyPressed(KEY_PERIOD)) {
          pressed = 1;
        }
        BeginDrawing();

        ClearBackground(BLACK);

        renderLevel(rt);
        drawCursors(rt, match);
        drawMatch(rt, match);
        renderRule(match);

        EndDrawing();
        frameCounter++;
      }
    }
  }
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
