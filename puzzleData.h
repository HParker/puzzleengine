#ifndef PUZZLE_DATA
#define PUZZLE_DATA

// Object
typedef struct Object {
  char * name;
  char legendKey;
  int height;
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
  int id; // TODO: rename to object id. this is never a legend value anymore
} LegendValue;

typedef struct Legend {
  int hasStringKey;
  char key;
  char * stringKey;
  LegendRelation objectRelation;
  int objectCount;
  LegendValue objectValues[100];
} Legend;

typedef struct GlyphLegend {
  char key;
  LegendRelation objectRelation;
  int objectCount;
  LegendValue objects[100];
} GlyphLegend;

typedef struct AliasLegend {
  char * key;
  LegendRelation objectRelation;
  int objectCount;
  LegendValue objects[100];
} AliasLegend;

typedef struct Layer {
  int width;
  int objectIds[1000];
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
   NONE       = 11,
   COND_NO    = 12
  } Direction;

typedef enum ExecutionTime
  {
   NORMAL = 0,
   LATE = 1
  } ExecutionTime;


typedef struct RuleIdentity {
  // TODO: direction can work here especially for things like [ Player no Ground ]
  //       to contain modifiers as well, but I am not ready to make that change in the parser
  Direction direction;
  int legendId;
} RuleIdentity;

// parts are split by `|`
typedef struct RuleStatePart {
  int ruleIdentityCount;
  RuleIdentity ruleIdentity[100];
  /* Direction direction; */
  /* int legendId; */
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

typedef struct MatchData {
  int objIndex;
  int ruleLegendId;
  int actualX;
  int actualY;
  int actualDirection;
  // goal
  int goalId;
  int goalX;
  int goalY;
  int goalDirection;
} MatchData;

typedef struct Match {
  // legend id in rules could be things like `movable = Block or Player`
  // which isn't the legend id of the thing to actually place when applying the result state
  int ruleIndex;
  int partCount;
  Direction appliedDirection;
  MatchData parts[100];
} Match;

typedef enum LevelType
  {
   SQUARES = 0,
   MESSAGE_TEXT = 1
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
  int objectCapacity;
  Object * objects;

  // sounds
  // blah. I don't wanna do it...

  int aliasLegendCount;
  int aliasLegendCapacity;
  AliasLegend * aliasLegend;

  int glyphLegendCount;
  int glyphLegendCapacity;
  GlyphLegend * glyphLegend;


  // collision layers
  int layerCount;
  int layerCapacity;
  Layer * layers;

  // rules
  int ruleCount;
  int ruleCapacity;
  Rule * rules;

  // win conditions
  int winConditionCount;
  int winConditionCapacity;
  WinCondition * winConditions;

  // levels
  int levelCount;
  int levelCapacity;
  Level * levels;
} PuzzleData;

typedef struct ToMove {
  int objIndex;
  Direction direction;
} ToMove;

typedef struct Obj {
  int x;
  int y;
  int deleted;
  int objId;
} Obj;

typedef struct Runtime {
  PuzzleData * pd;
  int gameWon;
  int levelIndex;
  char background;
  int height;
  int width;
  LevelType levelType;
  int objectCount;
  int objectCapacity;
  Obj * objects;
  int toMoveCount;
  int toMoveCapacity;
  ToMove * toMove;
  int historyCount;
  int historyCapacity;
  Direction * history;
  // move history?
} Runtime;

// PuzzleData
extern PuzzleData * parsePuzzle(FILE * file);
extern char objectGlyph(int objId);
extern char * objectName(int id);
extern int debug();
extern int objectCount();

// legend
extern int legendCount();
extern int legendIdForGlyph(char glyph);
extern int legendObjectCount(int id);
int aliasLegendObjectCount(int id);
int glyphLegendObjectCount(int id);
int aliasLegendObjectId(int legendId, int objectIndex);
int glyphLegendObjectId(int legendId, int objectIndex);

extern LegendValue legendObject(int legendId, int objectIndex);
extern LegendValue aliasLegendObject(int legendId, int objectIndex);
extern LegendValue glyphLegendObject(int legendId, int objectIndex);
extern int aliasLegendObjectId(int legendId, int objectIndex);
extern int glyphLgendObjectId(int legendId, int objectIndex);
extern int aliasLegendObjectIsLegend(int legendId, int objectIndex);
extern int glyphLegendObjectIsLegend(int legendId, int objectIndex);
extern int legendContains(int legendId, int objId);
extern int glyphLegendContains(int legendId, int objId);
extern int aliasLegendContains(int legendId, int objId);
extern int aliasLegendId(char * name);

extern int aliasLegendCount();
extern int glyphLegendCount();
extern char * aliasLegendKey(int id);
extern char glyphLegendKey(int id);

// layers
extern int objectLayer(int objId);

// level
extern int levelHeight(int levelIndex);
extern int levelWidth(int levelIndex);
extern int levelCellCount(int levelIndex);
extern int levelCell(int levelIndex, int cellIndex);
extern LevelType levelType(int levelIndex);
extern char * levelMessage(int levelIndex);
// layers
int layerIncludes(int layerId, int objId);

int objectId(char * name);
//rules
extern int ruleCount();
extern Rule * rule(int index);

// runner
// Levels
extern void startGame(Runtime * rt, FILE * file);
extern void nextLevel(Runtime * rt);
extern int levelCount();

// Winning
extern int checkWinCondition(Runtime * rt, int winConditionIndex);
extern int checkWinConditions(Runtime * rt);

// Drawing
extern void render(Runtime * rt);

// Update
extern Direction handleInput(Runtime * rt, int input);
extern void setLevel(Runtime * rt);
extern void update(Runtime * rt, Direction dir);

// Debug
void printHistory(Runtime * rt);

// Win Condition
extern int winConditionCount();
extern WinCondition * winCondition(int winConditionIndex);

// Parser methods
extern int legendId(char * name);
extern void incObject();
extern void incAliasLegend();
extern void incGlyphLegend();
extern void incLayer();
extern void incRule();
extern void incWinCondition();
extern void incLevel();
extern FILE * yyin;
extern int yyparse();

#endif
