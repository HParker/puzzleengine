#include "puzzleData.h"
#include <curses.h>
#include <stdio.h>
#include <string.h>

char charForLoc(Runtime *rt, int loc) {
  int maxHeight = -1;
  int id = -1;
  int currentHeight;

  for (int i = 0; i < rt->objectCount; i++) {
    currentHeight = objectLayer(rt->objects[i].objId);

    // TODO: this is silly
    int objectLoc = (rt->objects[i].y * rt->width) + rt->objects[i].x;

    if (rt->objects[i].deleted == 0 && objectLoc == loc &&
        currentHeight > maxHeight) {
      maxHeight = currentHeight;
      id = rt->objects[i].objId;
    }
  }
  if (id == -1) {
    return objectGlyph(rt->backgroundId);
  }
  return objectGlyph(id);
}

void renderLevel(Runtime *rt) {
  clear();
  int row = 0;
  int col = 0;
  int count = levelTileCount(rt->levelIndex);
  char map[count];
  for (int i = 0; i < count; i++) {
    map[i] = charForLoc(rt, i);
  }

  // draw
  for (int i = 0; i < count; i++) {
    mvaddch(row, col, map[i]);
    col++;
    if ((i + 1) % (rt->width) == 0) {
      col = 0;
      row++;
    }
  }
  refresh();
}

void renderMessage(Runtime *rt) {
  clear();
  char *message = levelMessage(rt->levelIndex);
  int messageLength = strlen(message);
  for (int i = 0; i < messageLength + 4; i++) {
    mvaddch(0, i, '*');
  }

  mvaddch(1, 0, '*');
  mvaddch(1, messageLength + 1, '*');

  mvaddstr(1, 2, message);

  for (int i = 0; i < messageLength + 4; i++) {
    mvaddch(2, i, '*');
  }
}

void render(Runtime *rt) {
  switch (rt->levelType) {
  case SQUARES:
    renderLevel(rt);
    break;
  case MESSAGE_TEXT:
    renderMessage(rt);
    break;
  }
}

void debugRender(Runtime *rt) { render(rt); }

void initRenderer() {
  initscr();
  cbreak();
  noecho();
  clear();
}

void closeRenderer() { endwin(); }

char input() {
  switch (getch()) {
  case 'q':
    return QUIT;
  case 'r':
    return RESTART;
  case 'w':
    return UP;
  case 'a':
    return LEFT;
  case 's':
    return DOWN;
  case 'd':
    return RIGHT;
  case 'z':
    return UNDO;
  case 'x':
    return USE;
  }
  return UNSPECIFIED;
}
