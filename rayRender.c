#include <stdlib.h>
#include <string.h>
#include<stdio.h>
#include <unistd.h>
#include <raylib.h>
#include "puzzleData.h"
#include "alphabet.c"

#define TRANSPARENT CLITERAL(Color){ 0, 0, 0, 0 }           // Blank (Transparent)

char * RED_COLORS[10] = {"red", "", "", "", "", "", "", "", "", ""};
char * WHITE_COLORS[10] = {"green", "", "", "", "", "", "", "", "", ""};
char * PURPLE_COLORS[10] = {"blue", "", "", "", "", "", "", "", "", ""};

int CURSOR_SPRITE[25] = {
                       '0','.','.','.','0',
                       '.','.','.','.','.',
                       '.','.','.','.','.',
                       '.','.','.','.','.',
                       '0','.','.','.','0'
};

int SQUARE[25] = {
                       '0','0','.','0','0',
                       '0','.','.','.','0',
                       '.','.','.','.','.',
                       '0','.','.','.','0',
                       '0','0','.','0','0'
};

int CHEVRON_UP[25] = {
                       '.','.','0','.','.',
                       '.','0','0','0','.',
                       '0','.','0','.','0',
                       '.','.','0','.','.',
                       '.','.','.','.','.'
};


int CHEVRON_DOWN[25] = {
                         '.','.','.','.','.',
                         '.','.','0','.','.',
                         '0','.','0','.','0',
                         '.','0','0','0','.',
                         '.','.','0','.','.'
};


int CHEVRON_LEFT[25] = {
                       '.','.','0','.','.',
                       '.','0','.','.','.',
                       '0','0','0','0','.',
                       '.','0','.','.','.',
                       '.','.','0','.','.'
};


int CHEVRON_RIGHT[25] = {
                       '.','.','0','.','.',
                       '.','.','.','0','.',
                       '.','0','0','0','0',
                       '.','.','.','0','.',
                       '.','.','0','.','.'
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

#define WINDOW_SIZE 940
#define SPRITE_WIDTH 5

void initRenderer() {
  InitWindow(WINDOW_SIZE, WINDOW_SIZE, "My Puzzle");
  SetTargetFPS(27);
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

Color bkColor() {
  return colorFromName(backgroundColor());
}

Color txtColor() {
  return colorFromName(textColor());
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

void debugDrawSprite(Runtime * rt, int sprite[25], char * colors[10], int x, int y) {
  // TODO: for now app sprites are 25 long, but we can make this more generic
  for (int i = 0; i < 25; i++) {
    Color cellColor = colorFromList(colors, sprite[i]);
    if (cellColor.a != 0) {
      int realX = leftMargin(rt) + (x * pixelSize(rt) * SPRITE_WIDTH) + ((i % SPRITE_WIDTH) * pixelSize(rt));
      int realY = topMargin(rt)  + (y * pixelSize(rt) * SPRITE_WIDTH) + ((i / SPRITE_WIDTH) * pixelSize(rt));

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
  int count = levelCellCount(rt->levelIndex);
  for (int cell = 0; cell < count; cell++) {
    int x = cell % rt->width;
    int y = cell / rt->width;
    drawObject(rt, rt->backgroundId, x, y);
  }
}

void renderLevel(Runtime * rt) {
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
  if (rt->pd->verboseLogging) {
    Rectangle rec = { 0, 0, WINDOW_SIZE, WINDOW_SIZE };
    DrawRectangleLinesEx(rec, 10, RED);
  }
}

void renderMessage(Runtime * rt) {
  char * message = levelMessage(rt->levelIndex);
  int textLength = MeasureText(message, 20);

  DrawText(message, windowSize() / 2 - textLength / 2, windowSize() / 2, 25, txtColor());
}

void drawMovement(Runtime * rt, int x, int y, Direction dir, char * colors[10]) {
  switch (dir) {
  case LEFT:
    debugDrawSprite(rt, CHEVRON_LEFT, colors, x, y);
    break;
  case RIGHT:
    debugDrawSprite(rt, CHEVRON_RIGHT, colors, x, y);
    break;
  case UP:
    debugDrawSprite(rt, CHEVRON_UP, colors, x, y);
    break;
  case DOWN:
    debugDrawSprite(rt, CHEVRON_DOWN, colors, x, y);
    break;
  default:
    debugDrawSprite(rt, SQUARE, colors, x, y);
    break;
  }
}

void drawMatch(Runtime * rt, Match * match) {
  for (int i = 0; i < match->partCount; i++) {

    int x = leftMargin(rt) + (match->parts[i].goalX * pixelSize(rt) * SPRITE_WIDTH) + ((i % SPRITE_WIDTH) * pixelSize(rt));
    int y = topMargin(rt) + (match->parts[i].goalY * pixelSize(rt) * SPRITE_WIDTH) + ((i / SPRITE_WIDTH) * pixelSize(rt));

    drawMovement(rt, match->parts[i].goalX, match->parts[i].goalY, match->parts[i].goalDirection, RED_COLORS);
  }
}

void drawCursors(Runtime * rt, Match * match) {
  // cursor
  /* if (match->cursorX != -1) { */
  /*   debugDrawSprite(rt, CURSOR_SPRITE, WHITE_COLORS, match->cursorX, match->cursorY); */
  /* } */

  /* if (match->targetX != -1) { */
  /*   debugDrawSprite(rt, CURSOR_SPRITE, WHITE_COLORS, match->targetX, match->targetY); */
  /* } */
}

void drawToMove(Runtime * rt) {
  for (int i = 0; i < rt->toMoveCount; i++) {
    if (rt->objects[rt->toMove[i].objIndex].deleted == 0) {
      int x = rt->objects[rt->toMove[i].objIndex].x;
      int y = rt->objects[rt->toMove[i].objIndex].y;
      drawMovement(rt, x, y, rt->objects[rt->toMove[i].objIndex].moving, PURPLE_COLORS);
    }
  }
}



void renderRule(Match * match) {
  Color transparentBlack = { 0, 0, 0, 140 };
  DrawRectangle(0, 0, WINDOW_SIZE, 50, transparentBlack);
  char * rstr = ruleString(match->ruleIndex);
  DrawText(rstr, 10, 10, 20, WHITE);
  free(rstr);
}

void debugRender(Runtime * rt, Match * match) {
  int awaitInput = 1;
  int frameCounter = 0;
  int frameDelay = 1;
  int pressed = 0;
  if (match->partCount > 0) {
    awaitInput = 1;
  }

  if (rt->levelType == SQUARES && rt->pd->verboseLogging) {
    if (awaitInput) {
      while (pressed == 0) {
        if (IsKeyPressed(KEY_PERIOD)) {
          pressed = 1;
        }
        BeginDrawing();

        ClearBackground(bkColor());

        renderLevel(rt);
        drawMatch(rt, match);
        drawToMove(rt);
        renderRule(match);
        /* drawCursors(rt, match); */
        EndDrawing();
      }
    } else {
      while (frameCounter < frameDelay) {
        if (IsKeyPressed(KEY_PERIOD)) {
          pressed = 1;
        }
        BeginDrawing();

        ClearBackground(bkColor());

        renderLevel(rt);
        drawMatch(rt, match);
        renderRule(match);
        drawCursors(rt, match);
        EndDrawing();
        frameCounter++;
      }
    }
  }
}


void render(Runtime * rt) {
  BeginDrawing();

  ClearBackground(bkColor());
  DrawFPS(0,0);
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

Direction input() {
  if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
    return RIGHT;
  }
  if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
    return LEFT;
  }
  if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
    return UP;
  }
  if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
    return DOWN;
  }
  if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE)) {
    return QUIT;
  }
  if (IsKeyPressed(KEY_R)) {
    return RESTART;
  }
  if (IsKeyPressed(KEY_Z)) {
    return UNDO;
  }
  if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_SPACE)) {
    return USE;
  }

  if (IsKeyPressed(KEY_TAB)) {
    return DEBUGGER;
  }
  return UNSPECIFIED;
}
