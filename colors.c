#ifndef COLORS
#define COLORS

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
#endif
