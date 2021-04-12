#include "alphabet.c"
#include "colors.c"
#include "puzzleData.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WINDOW_SIZE 840

int delta = 0;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event e;

void drawChar(int x, int y, int pixelSize, char *sprite) {
  int width = 5;
  int height = 5;
  SDL_Rect sdlRect;
  Color textColor = WHITE;

  for (int i = 0; i < 25; i++) {
    if (sprite[i] == '0') {
      sdlRect.x = (x * pixelSize * width) + ((i % 5) * pixelSize);
      sdlRect.y = (y * pixelSize * height) + ((i / 5) * pixelSize);
      sdlRect.w = pixelSize;
      sdlRect.h = pixelSize;
      SDL_SetRenderDrawColor(renderer, textColor.r, textColor.g, textColor.b,
                             textColor.a);
      SDL_RenderFillRect(renderer, &sdlRect);
    }
  }
}

void drawObj(Runtime *rt, int objIndex) {
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
  SDL_Rect sdlRect;
  int objectId = rt->objects[objIndex].objId;

  // TODO: for now app sprites are 25 long, but we can make this more generic
  for (int i = 0; i < 25; i++) {
    Color tileColor = colorFromSprite(rt, objectId, i);
    if (tileColor.a != 0) {
      sdlRect.x =
          (rt->objects[objIndex].x * pixelSize * width) + ((i % 5) * pixelSize);
      sdlRect.y = (rt->objects[objIndex].y * pixelSize * height) +
                  ((i / 5) * pixelSize);
      sdlRect.w = pixelSize;
      sdlRect.h = pixelSize;

      SDL_SetRenderDrawColor(renderer, tileColor.r, tileColor.g, tileColor.b,
                             tileColor.a);
      SDL_RenderFillRect(renderer, &sdlRect);
    }
  }
}

void initRenderer(char *name) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "ERROR Initializing SDL2\n");
  }

  window =
      SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_SIZE, WINDOW_SIZE, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    fprintf(stderr, "ERROR making window\n");
  }

  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    fprintf(stderr, "ERROR making renderer\n");
  }
}
void renderBackground(Runtime *rt) {
  int pixelSize;
  if (rt->width > rt->height) {
    pixelSize = WINDOW_SIZE / (rt->width * 5);
  } else {
    pixelSize = WINDOW_SIZE / (rt->height * 5);
  }

  int width = 5;
  int height = 5;
  SDL_Rect sdlRect;

  int count = levelTileCount(rt->levelIndex);
  for (int tile = 0; tile < count; tile++) {
    for (int i = 0; i < 25; i++) {
      Color tileColor = colorFromSprite(rt, rt->backgroundId, i);
      if (tileColor.a != 0) {
        sdlRect.x =
            (tile % rt->width * pixelSize * width) + ((i % 5) * pixelSize);
        sdlRect.y =
            (tile / rt->width * pixelSize * height) + ((i / 5) * pixelSize);
        sdlRect.w = pixelSize;
        sdlRect.h = pixelSize;

        SDL_SetRenderDrawColor(renderer, tileColor.r, tileColor.g, tileColor.b,
                               tileColor.a);
        SDL_RenderFillRect(renderer, &sdlRect);
      }
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
  int messageLength = strlen(message);
  for (int i = 0; i < messageLength; i++) {
    drawChar((i % 25) + 1, ((i / 25) * 2) + 5, 5, charSprite(message[i]));
  }
}

void render(Runtime *rt) {
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer);

  switch (rt->levelType) {
  case SQUARES:
    renderLevel(rt);
    break;
  case MESSAGE_TEXT:
    renderMessage(rt);
    break;
  }

  SDL_RenderPresent(renderer);
}

void debugRender(Runtime *rt) { /* render(rt); */
}

Direction input() {
  // block on input for now. I don't think we have to do this forever.
  if (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      return QUIT;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
      case SDLK_q:
      case SDLK_ESCAPE:
        return QUIT;
      case SDLK_r:
        return RESTART;
      case SDLK_z:
        return UNDO;
      case SDLK_w:
      case SDLK_UP:
        return UP;
      case SDLK_s:
      case SDLK_DOWN:
        return DOWN;
      case SDLK_d:
      case SDLK_RIGHT:
        return RIGHT;
      case SDLK_a:
      case SDLK_LEFT:
        return LEFT;
      case SDLK_x:
        return USE;
      case SDLK_TAB:
        return DEBUGGER;
      default:
        break;
      }
    }
  }
  return UNSPECIFIED;
}

// Blocking
char blockInput() {
  // block on input for now. I don't think we have to do this forever.
  char c = -1;
  while (SDL_PollEvent(&e) || c == -1) {
    if (e.type == SDL_QUIT) {
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
      case SDLK_q:
      case SDLK_ESCAPE:
        return 'q';
      case SDLK_r:
        return 'r';
      case SDLK_z:
        return 'z';
      case SDLK_w:
      case SDLK_UP:
        return 'w';
      case SDLK_s:
      case SDLK_DOWN:
        return 's';
      case SDLK_d:
      case SDLK_RIGHT:
        return 'd';
      case SDLK_a:
      case SDLK_LEFT:
        return 'a';
      case SDLK_x:
        return 'x';
      case SDLK_TAB:
        return '`';
      default:
        break;
      }
    }
  }
  return c;
}

void closeRenderer() {
  SDL_DestroyWindow(window);
  SDL_Quit();
}
