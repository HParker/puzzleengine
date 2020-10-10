// Object
typedef struct Object {
  char * name;
  char legendKey;
  int height; // TODO these can be global
  int width;
  int shape[25];
  int colorCount;
  char * colors[9];
} Object;

typedef struct Layer {
  int width;
  char * objectNames[100];
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
  char * winIdentifier;
  int hasOnQualifier;
  char * onIndentifier;
} WinCondition;

typedef enum Direction
  {
   UP         = 0,
   DOWN       = 1,
   LEFT       = 2,
   RIGHT      = 3,
   HORIZONTAL = 4,
   VIRTICAL   = 5,
   REL_UP     = 6,
   REL_DOWN   = 7,
   REL_LEFT   = 8,
   REL_RIGHT  = 9,
   NONE       = 10
  } Direction;

typedef enum ExecutionTime
  {
   NORMAL = 0,
   LATE = 1
  } ExecutionTime;

typedef struct RuleStatePart {
  Direction direction;
  char * identifier;
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
   MESSAGE
  } LevelType;

typedef struct Level {
  LevelType levelType;
  int height;
  int width;
  int cellIndex;
  char cells[100];
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

  // Objects (with Legend tag)
  int objectCount;
  Object objects[100];

  // sounds

  // blah. I don't wanna do it...

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
