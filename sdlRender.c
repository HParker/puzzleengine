#include <stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include "puzzleData.h"

#define WINDOW_SIZE 840

typedef struct Color {
  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 a;
} Color;

#define CLITERAL(type) (type)
#define BLACK       CLITERAL(Color){ 0x00, 0x00, 0x00, 0xFF }
#define WHITE       CLITERAL(Color){ 255, 255, 255, 255 }   // White

#define LIGHTGRAY   CLITERAL(Color){ 200, 200, 200, 255 }   // Light Gray
#define GRAY        CLITERAL(Color){ 130, 130, 130, 255 }   // Gray
#define DARKGRAY    CLITERAL(Color){ 80, 80, 80, 255 }      // Dark Gray

#define RED         CLITERAL(Color){ 230, 41, 55, 255 }     // Red
#define DARKRED     CLITERAL(Color){ 190, 33, 55, 255 }     // Dark Red
#define LIGHTRED    CLITERAL(Color){ 240, 44, 55, 255 }     // Light Red

#define BROWN       CLITERAL(Color){ 127, 106, 79, 255 }    // Brown
#define LIGHTBROWN  CLITERAL(Color){ 211, 176, 131, 255 }   // Beige
#define DARKBROWN   CLITERAL(Color){ 76, 63, 47, 255 }      // Dark Brown

#define ORANGE      CLITERAL(Color){ 255, 161, 0, 255 }     // Orange
#define YELLOW      CLITERAL(Color){ 253, 249, 0, 255 }     // Yellow

#define GOLD        CLITERAL(Color){ 255, 203, 0, 255 }     // Gold

#define GREEN       CLITERAL(Color){ 0, 228, 48, 255 }      // Green
#define LIGHTGREEN  CLITERAL(Color){ 0, 158, 47, 255 }      // Lime
#define DARKGREEN   CLITERAL(Color){ 0, 117, 44, 255 }      // Dark Green

#define BLUE        CLITERAL(Color){ 0, 121, 241, 255 }     // Blue
#define LIGHTBLUE   CLITERAL(Color){ 102, 191, 255, 255 }   // Sky Blue
#define DARKBLUE    CLITERAL(Color){ 0, 82, 172, 255 }      // Dark Blue

#define PURPLE      CLITERAL(Color){ 200, 122, 255, 255 }   // Purple
#define LIGHTPURPLE CLITERAL(Color){ 135, 60, 190, 255 }    // Violet
#define DARKPURPLE  CLITERAL(Color){ 112, 31, 126, 255 }    // Dark Purple
#define PINK        CLITERAL(Color){ 255, 109, 194, 255 }   // Pink

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
   DARKRED,
   LIGHTRED,
   BROWN,
   DARKBROWN,
   LIGHTBROWN,
   ORANGE,
   YELLOW,
   GREEN,
   DARKGREEN,
   LIGHTGREEN,
   BLUE,
   LIGHTBLUE,
   DARKBLUE,
   PURPLE,
   PINK,
   TRANSPARENT
  };

int shouldQuit = 0;
int delta = 0;
SDL_Window * window;
SDL_Renderer * renderer;
SDL_Event e;

Color colorFromName(char * name) {
  for (int i = 0; i < 25; i++) {
    if (strcasecmp(colorNames[i], name) == 0) {
      return colors[i];
    }
  }
  printf("NO COLOR FOUND FOR NAME '%s'\n", name);
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
  printf("FAILED TO MATCH A COLOR CODE id %i (%c)\n", objId, cell);
  for (int i = 0; i < 25; i++) {
    printf("%c", objectSpriteCell(objId, i));
  }
  printf("\n");
  return PINK;

}

void drawObj(Runtime * rt, int objIndex) {
  if (rt->objects[objIndex].deleted == 1) {
    return;
  }
  int pixelSize = 10;
  int width = 5;
  int height = 5;
  SDL_Rect sdlRect;
  int objectId = rt->objects[objIndex].objId;


  // TODO: for now app sprites are 25 long, but we can make this more generic
  for (int i = 0; i < 25; i++) {
    Color cellColor = colorFromSprite(rt, objectId, i);
    if (cellColor.a != 0) {
      sdlRect.x = (rt->objects[objIndex].x * pixelSize * width) + ((i % 5) * pixelSize);
      sdlRect.y = (rt->objects[objIndex].y * pixelSize * height) + ((i / 5) * pixelSize);
      sdlRect.w = pixelSize;
      sdlRect.h = pixelSize;


      SDL_SetRenderDrawColor(renderer, cellColor.r, cellColor.g, cellColor.b, cellColor.a);
      SDL_RenderFillRect(renderer, &sdlRect);
    }
  }
}

void initRenderer() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0){
    printf("ERROR Initializing SDL2\n");
    shouldQuit = 1;
  }

  window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    printf("ERROR making window\n");
    shouldQuit = 1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    printf("ERROR making renderer\n");
    shouldQuit = 1;
  }
}
void renderBackground(Runtime * rt) {
  int pixelSize = 10;
  int width = 5;
  int height = 5;
  int backgroundId = aliasLegendObjectId(aliasLegendId("Background"), 0);
  SDL_Rect sdlRect;

  int count = levelCellCount(rt->levelIndex);
  for (int cell = 0; cell < count; cell++) {
    for (int i = 0; i < 25; i++) {
      Color cellColor = colorFromSprite(rt, backgroundId, i);
      if (cellColor.a != 0) {
        sdlRect.x = (cell % rt->width * pixelSize * width) + ((i % 5) * pixelSize);
        sdlRect.y = (cell / rt->width * pixelSize * height) + ((i / 5) * pixelSize);
        sdlRect.w = pixelSize;
        sdlRect.h = pixelSize;

        SDL_SetRenderDrawColor(renderer, cellColor.r, cellColor.g, cellColor.b, cellColor.a);
        SDL_RenderFillRect(renderer, &sdlRect);
      }
    }
  }
}

void renderLevel(Runtime * rt) {
  int objLayer;

  renderBackground(rt);

  for (int layer = 0; layer < 4; layer++) {
    for (int i = 0; i < rt->objectCount; i++) {
      objLayer = objectLayer(rt->objects[i].objId);
      if (layer == objLayer) {
        drawObj(rt, i);
      }
    }
  }

  SDL_RenderPresent(renderer);
}

void renderMessage(Runtime * rt) {
  SDL_SetRenderDrawColor(renderer, GREEN.r, GREEN.g, GREEN.b, GREEN.a);
  SDL_Rect sdlRect = { 10, 10, 10, 10 };
  SDL_RenderFillRect(renderer, &sdlRect);

  SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
  SDL_RenderPresent(renderer);
}

void render(Runtime * rt) {
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
}

char input() {
  // block on input for now. I don't think we have to do this forever.
  char c = '-';
  while (SDL_PollEvent(&e) || c == '-') {
    if (e.type == SDL_QUIT) {
      shouldQuit = 1;
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
      default:
        break;
      }
    }
    SDL_Delay(10);
  }
  return c;
}

void closeRenderer() {
  SDL_DestroyWindow(window);
  SDL_Quit();
}
