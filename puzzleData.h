#ifndef PUZZLE_DATA
#define PUZZLE_DATA

#include <time.h>

// Object
typedef struct Object {
  char * name;
  int height;
  int width;
  char sprite[25];
  int colorCount;
  char * colors[10];
} Object;

typedef enum LegendRelation {
   LEGEND_RELATION_UNKNOWN = 0,
   LEGEND_RELATION_AND = 1,
   LEGEND_RELATION_OR = 2
} LegendRelation;

typedef struct GlyphLegend {
  char key;
  LegendRelation objectRelation;
  int objectCount;
  int objectCapacity;
  int * objects;
} GlyphLegend;

typedef struct AliasLegend {
  char * key;
  LegendRelation objectRelation;
  int objectCount;
  int objectCapacity;
  int * objects;
  char * mask;
} AliasLegend;

typedef struct Layer {
  int count;
  int objectCapacity;
  int * objectIds;
} Layer;

typedef enum WinQualifier // TODO: rename to logic words?
  {
   ALL  = 0,
   ANY  = 1,
   NO   = 2,
   SOME = 3,
   ON   = 4
  } WinQualifier;

typedef struct WinCondition {
  WinQualifier winQualifier;
  int winIdentifier;
  int hasOnQualifier;
  int onIndentifier;
} WinCondition;

typedef enum Command
  {
   SFX0 = 0,
   SFX1 = 1,
   SFX2 = 2,
   SFX3 = 3,
   SFX4 = 4,
   SFX5 = 5,
   SFX6 = 6,
   SFX7 = 7,
   SFX8 = 8,
   SFX9 = 9,
   AGAIN = 10,
   CANCEL = 11,
   CHECKPOINT = 12,
   CMD_RESTART = 13,
   WIN = 14,
  } Command;

typedef enum Direction {
   RIGHT         = 0,
   UP            = 1,
   LEFT          = 2,
   DOWN          = 3,
   HORIZONTAL    = 4,
   VERTICAL      = 5,
   STATIONARY    = 6,
   RANDOMDIR     = 7,
   RANDOM        = 8,
   REL_UP        = 9,
   REL_DOWN      = 10,
   REL_LEFT      = 11,
   REL_RIGHT     = 12,
   USE           = 13,
   NONE          = 14,
   COND_NO       = 15,
   QUIT          = 16,
   RESTART       = 17,
   UNDO          = 18,
   UNSPECIFIED   = 19,
   MOVING        = 20,
   DEBUGGER      = 21,
   PARALLEL      = 22,
   PERPENDICULAR = 23,
   NEXT_LEVEL    = 24
  } Direction;

typedef enum ExecutionTime {
  NORMAL = 0,
  LATE = 1
} ExecutionTime;

typedef struct RuleIdentity {
  Direction direction;
  int legendId;
} RuleIdentity;

// parts are split by `|`
typedef struct RulePart {
  int identityCount;
  int identityCapacity;
  int isSpread;
  RuleIdentity * identity;
} RulePart;

typedef struct Pattern {
  int partCount;
  int partCapacity;
  RulePart * parts;
} Pattern;

typedef struct Rule {
  int id;
  int lineNo;
  int hasSpread;
  int hasRelativeDirection;
  int hasCompoundDirection;
  int hasMultipleParts;

  int commandCount;
  Command commands[10];

  Direction directionConstraint;
  ExecutionTime executionTime;

  int matchPatternCount;
  int matchPatternCapacity;
  Pattern * matchPatterns;

  int resultPatternCount;
  int resultPatternCapacity;
  Pattern * resultPatterns;
} Rule;

typedef struct MatchData {
  int newObject;
  int objIndex;
  // legend id in rules could be things like `movable = Block or Player`
  // which isn't the legend id of the thing to actually place when applying the result state
  int ruleLegendId;
  int actualDirection;
  // goal
  int goalId;
  int objectId;
  int goalX;
  int goalY;
  int goalDirection;
} MatchData;

typedef struct Match {
  int ruleIndex;
  int partCount;

  int cancel;
  int cursorX;
  int cursorY;
  int targetX;
  int targetY;
  Direction appliedDirection;
  MatchData parts[1000];
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
  int tileIndex;
  int tileCapacity;
  char * tiles;
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
  int colorPalette;
  char * backgroundColor;
  char * textColor;
  int doesFlickScreen;
  int flickScreenX;
  int flickScreenY;
  int doesZoomScreen;
  int zoomScreenX;
  int zoomScreenY;
  int scanLine;

  // interval
  float keyRepeatInterval;
  // TODO: these set flags don't do what we want... again always triggers regardless of if it is set
  //       realtime interval does need this flag though.
  int setAgainInterval;
  float againInterval;
  int setRealtimeInterval;
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
} ToMove;

typedef struct Obj {
  int x;
  int y;
  int deleted;
  int objId;
  Direction moving;
} Obj;

typedef struct State {
  int levelIndex;
  int objectCount;
  int objectCapacity;
  Obj * objects;
  int * map;
  char * oMap;
} State;

typedef struct Runtime {
  PuzzleData * pd;
  time_t randomSeed;
  int gameWon;
  int doAgain;
  int didUndo;
  int deadCount;
  int levelIndex;
  int backgroundId;
  int playerId;
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

  int hasMap;
  int * map;

  int hasOMap;
  char * oMap;

  int statesCount;
  int statesCapacity;
  State * states;
  int prevHistoryCount;
} Runtime;

// PuzzleData
extern PuzzleData * parsePuzzle(FILE * file);
extern void freePuzzle();
extern char objectGlyph(int objId);
extern Object * object(int id);
extern char * objectName(int id);
extern int objectSpriteTile(int id, int index);
extern char * objectColor(int id, int colorIndex);

// legend
extern int legendIdForGlyph(char glyph);
extern int legendObjectCount(int id);
extern int aliasLegendObjectCount(int id);

extern int legendObject(int legendId, int objectIndex);
extern int aliasLegendObject(int legendId, int objectIndex);
extern int glyphLegendObject(int legendId, int objectIndex);
extern int glyphLgendObjectId(int legendId, int objectIndex);
extern int aliasLegendObjectIsLegend(int legendId, int objectIndex);
extern int glyphLegendObjectIsLegend(int legendId, int objectIndex);
extern int legendContains(int legendId, int objId);
extern int glyphLegendContains(int legendId, int objId);
extern int aliasLegendContains(int legendId, int objId);
extern int aliasLegendId(char * name);

extern char * aliasLegendKey(int id);
extern char glyphLegendKey(int id);

// layers
extern int objectLayer(int objId);
extern int layerWidth(int layerId);
extern int layerObjectId(int layerId, int objectIndex);

// level
extern int levelTileCount(int levelIndex);
extern int levelTile(int levelIndex, int tileIndex);
extern LevelType levelType(int levelIndex);
extern char * levelMessage(int levelIndex);

// layers
int layerIncludes(int layerId, int objId);
int objectId(char * name);

// runner
// Levels
extern void startGame(Runtime * rt, FILE * file);
extern void endGame(Runtime * rt);
extern void nextLevel(Runtime * rt);
extern void undo(Runtime * rt);

// Winning
extern int checkWinCondition(Runtime * rt, int winConditionIndex);
extern int checkWinConditions(Runtime * rt);

// Update
extern Direction handleInput(Runtime * rt, Direction input);
extern void update(Runtime * rt, Direction dir);
extern void tick(Runtime * rt);

// Debug
extern void printHistory(Runtime * rt);
extern void printRules();
extern char * ruleString(int ruleId);
extern char * dirName(Direction dir);

// Win Condition
extern int winConditionCount();
extern WinCondition * winCondition(int winConditionIndex);

// Parser methods
extern void verboseLoggingOn();
extern int colorPaletteId(char * name);
extern char * backgroundColor();
extern char * textColor();
extern int legendId(char * name);
extern void incObject();
extern void incAliasLegend();
extern void incGlyphLegend();
extern void incAliasLegendObject(int legendId);
extern void addObjectsToAliasLegend(char * name);
extern void incGlyphLegendObject(int legendId);
extern void incLayer();
extern void addObjectsToLayer(char * name);

extern void incRule();
extern void incRuleMatchState(Rule * rule);
extern void incRuleResultState(Rule * rule);
extern void incRulePart(Pattern * ruleState);
extern void incRuleIdent(RulePart * part);

extern int ruleCommandCount(int ruleId);
extern int ruleCommandContains(Rule * rule, Command cmd);

extern void incWinCondition();
extern void incLevel();
extern void addTile(int levelId, char tileValue);
extern FILE * yyin;
extern int yyparse();

#endif
