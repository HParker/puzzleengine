#ifndef PUZZLE_DATA
#define PUZZLE_DATA

// Object
typedef struct Object {
  char * name;
  char legendKey;
  int height; // TODO these can be global
  int width;
  char sprite[25];
  int colorCount;
  char * colors[9];
} Object;

typedef enum LegendRelation
  {
   LEGEND_RELATION_UNKNOWN = 0,
   LEGEND_RELATION_AND = 1,
   LEGEND_RELATION_OR = 2
  } LegendRelation;

typedef struct LegendValue {
  int isLegend;
  int id;
} LegendValue;

typedef struct Legend {
  char * key;
  LegendRelation objectRelation;
  int objectCount;
  LegendValue objectValues[100];
} Legend;

typedef struct Layer {
  int width;
  int objectIds[100];
} Layer;

typedef enum WinQualifier // TODO: rename to logic words?
  {
   ALL  = 0,
   ANY  = 1,
   NO   = 2,
   SOME = 3,
   ON   = 4,
   UNSPECIFIED = 105
  } WinQualifier;

typedef struct WinCondition {
  WinQualifier winQualifier;
  int winIdentifier;
  int hasOnQualifier;
  int onIndentifier;
} WinCondition;

typedef enum Direction
  {
   RIGHT      = 0,
   UP         = 1,
   LEFT       = 2,
   DOWN       = 3,
   HORIZONTAL = 4,
   VIRTICAL   = 5,
   REL_UP     = 6,
   REL_DOWN   = 7,
   REL_LEFT   = 8,
   REL_RIGHT  = 9,
   USE        = 10,
   NONE       = 11
  } Direction;

typedef enum ExecutionTime
  {
   NORMAL = 0,
   LATE = 1
  } ExecutionTime;

typedef struct RuleStatePart {
  Direction direction;
  int legendId;
  /* char * identifier; // TODO: this can probably be an object or legend id */
} RuleStatePart;

typedef struct RuleState {
  int partCount;
  RuleStatePart parts[100];
} RuleState;

typedef struct Rule {
  int matchStateCount;
  int resultStateCount;
  int matchStateDone; // TODO: this is only needed when parsing. maybe manage it there?
  Direction directionConstraint;
  ExecutionTime executionTime;
  RuleState matchStates[100];
  RuleState resultStates[100];
} Rule;

typedef enum LevelType
  {
   SQUARES,
   MESSAGE_TEXT
  } LevelType;

typedef struct Level {
  LevelType levelType;
  int height;
  int width;
  int cellIndex;
  char cells[1000];
  char * message;
} Level;

typedef struct PuzzleData {
  // basics
  char * title;
  char * author;
  char * homepage;
  // disable flags
  int noAction;
  int noRepeatAction;
  int noUndo;
  int noRestart;

  // enable flags
  int requirePlayerMovement;
  int throttleMovement;
  int runRulesOnLevelStart;

  // global styling
  char * colorPalette;
  char * backgroundColor;
  char * textColor;
  int flickScreenX;
  int flickScreenY;
  int zoomScreenX;
  int zoomScreenY;
  int scanLine;

  // interval
  float keyRepeatInterval;
  float againInterval;
  float realTimeInterval;

  // debug
  int verboseLogging;
  int debug;

  int objectCount;
  Object objects[100];

  // sounds
  // blah. I don't wanna do it...

  // legend
  int legendCount;
  Legend legend[100];

  // collision layers
  int layerCount;
  Layer layers[100];

  // rules
  int ruleCount;
  Rule rules[100];

  // win conditions
  int winConditionCount;
  WinCondition winConditions[100];

  // levels
  int levelCount;
  Level levels[100];
} PuzzleData;

typedef struct ToMove {
  int objIndex;
  Direction direction;
} ToMove;

typedef struct Obj {
  int loc;
  int objId;
} Obj;

typedef struct Runtime {
  int levelIndex;
  char background;
  int height;
  int width;
  int objectCount;
  Obj objects[100];
  int toMoveCount;
  ToMove toMove[100];
  // move history?
} Runtime;

extern PuzzleData pd; // TODO: I explicitly return this instead of using a global
extern int legendId(char * name);
extern int yyparse();
extern FILE * yyin;

#endif
