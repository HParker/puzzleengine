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

typedef enum WinQualifier
  {
   ALL,
   ANY,
   NO,
   SOME,
   ON,   UNSPECIFIED
  } WinQualifier;

typedef struct WinCondition {
  char * winQualifier;
  char * winIdentifier;
  int hasOnQualifier;
  char * onIndentifier;
} WinCondition;

typedef enum Direction
  {
   UP,
   DOWN,
   LEFT,
   RIGHT,
   HORIZONTAL,
   VIRTICAL,
   // TODO: remove RELs they shouldn't make it into actual rules
   REL_UP,
   REL_DOWN,
   REL_LEFT,
   REL_RIGHT,
   NONE
  } Direction;

typedef enum ExecutionTime
  {
   NORMAL,
   LATE
  } ExecutionTime;

typedef struct RuleStatePart {
  int hasDirection;
  char * direction;
  char * identifier;
} RuleStatePart;

typedef struct RuleState {
  int partCount;
  RuleStatePart parts[100];
} RuleState;

typedef struct Rule {
  int matchStateCount;
  int resultStateCount;
  int matchStateDone;
  int hasDirectionConstraint;
  char * directionConstraint;
  ExecutionTime executionTime;
  RuleState matchStates[100];
  RuleState resultStates[100];
} Rule;

typedef struct Level {
  int height;
  int width;
  int cellIndex;
  char cells[100];
  int hasMessage;
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
