// Interface to puzzle data extracted from the puzzlescript file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "puzzleData.h"

extern PuzzleData pd;

#define PUZZLE_MALLOC_INC 100;
#define EMPTY_ID 1

void verboseLoggingOn() {
  if (pd.verboseLogging) {
    pd.verboseLogging = 0;
  } else {
    pd.verboseLogging = 1;
  }
}


// TODO: util this
char * dirName(Direction dir) {
  char * directionNames[] = {
                             "RIGHT",
                             "UP",
                             "LEFT",
                             "DOWN",
                             "HORIZONTAL",
                             "VERTICAL",
                             "STATIONARY",
                             "RANDOMDIR",
                             "RANDOM",
                             "^",
                             "v",
                             "<",
                             ">",
                             "USE",
                             "NONE",
                             "NO",
                             "QUIT",
                             "RESTART",
                             "UNDO",
                             "UNSPECIFIED",
                             "MOVING",
                             "DEBUGGER",
                             "PARALLEL",
                             "PERPENDICULAR"
  };
  return directionNames[dir];
}

char * commandName(Command cmd) {
  char * commandNames[] = {
                           "SFX0",
                           "SFX1",
                           "SFX2",
                           "SFX3",
                           "SFX4",
                           "SFX5",
                           "SFX6",
                           "SFX7",
                           "SFX8",
                           "SFX9",
                           "AGAIN",
                           "CANCEL",
                           "CHECKPOINT",
                           "CMD_RESTART",
                           "WIN"
  };
  return commandNames[cmd];
}

char * backgroundColor() {
  return pd.backgroundColor;
}

char * textColor() {
  return pd.textColor;
}


int colorPaletteId(char * name) {
  if (strcasecmp(name, "mastersystem") == 0) {
    return 1;
  } else if (strcasecmp(name, "gameboycolour") == 0) {
    return 2;
  } else if (strcasecmp(name, "gameboycolor") == 0) {
    return 2;
  } else if (strcasecmp(name, "amiga") == 0) {
    return 3;
  } else if (strcasecmp(name, "arnecolors") == 0) {
    return 4;
  } else if (strcasecmp(name, "famicom") == 0) {
    return 5;
  } else if (strcasecmp(name, "atari") == 0) {
    return 6;
  } else if (strcasecmp(name, "pastel") == 0) {
    return 7;
  } else if (strcasecmp(name, "ega") == 0) {
    return 8;
  } else if (strcasecmp(name, "amstrad") == 0) {
    return 9;
  } else if (strcasecmp(name, "proteus_mellow") == 0) {
    return 10;
  } else if (strcasecmp(name, "proteus_rich") == 0) {
    return 11;
  } else if (strcasecmp(name, "proteus_night") == 0) {
    return 12;
  } else if (strcasecmp(name, "c64") == 0) {
    return 13;
  } else if (strcasecmp(name, "whitingjp") == 0) {
    return 14;
  }
  return 4;
}

void incObject() {
  if (pd.objectCount + 1 >= pd.objectCapacity) {
    fprintf(stderr, "object REALLOC\n");
    pd.objectCapacity += PUZZLE_MALLOC_INC;
    pd.objects = realloc(pd.objects, sizeof(Object) * pd.objectCapacity);
    for (int i = pd.objectCount + 1; i < pd.objectCapacity; i++) {
      pd.objects[i].colorCount = 0;
      pd.objects[i].height = 0;
      pd.objects[i].width = 0;
    }
  }
  pd.objectCount++;
}

void incAliasLegend() {
  if (pd.aliasLegendCount + 1 >= pd.aliasLegendCapacity) {
    fprintf(stderr, "aliasLegend REALLOC\n");
    pd.aliasLegendCapacity += PUZZLE_MALLOC_INC;
    pd.aliasLegend = realloc(pd.aliasLegend, sizeof(AliasLegend) * pd.aliasLegendCapacity);
    for (int i = pd.aliasLegendCount + 1; i < pd.aliasLegendCapacity; i++) {
      pd.aliasLegend[i].objectCount = 0;
      pd.aliasLegend[i].objectCapacity = 10;
      pd.aliasLegend[i].objects = malloc(sizeof(int) * pd.aliasLegend[i].objectCapacity);
    }
  }
  pd.aliasLegendCount++;
}

void incGlyphLegend() {
  if (pd.glyphLegendCount + 1 >= pd.glyphLegendCapacity) {
    fprintf(stderr, "glyphLegend REALLOC\n");
    pd.glyphLegendCapacity += PUZZLE_MALLOC_INC;
    pd.glyphLegend = realloc(pd.glyphLegend, sizeof(GlyphLegend) * pd.glyphLegendCapacity);
    for (int i = pd.glyphLegendCount + 1; i < pd.glyphLegendCapacity; i++) {
      pd.glyphLegend[i].objectCount = 0;
      pd.glyphLegend[i].objectCapacity = 10;
      pd.glyphLegend[i].objects = malloc(sizeof(int) * pd.glyphLegend[i].objectCapacity);
    }
  }
  pd.glyphLegendCount++;
}

void incAliasLegendObject(int legendId) {
  if (pd.aliasLegend[legendId].objectCount + 1 >= pd.aliasLegend[legendId].objectCapacity) {
    pd.aliasLegend[legendId].objectCapacity += PUZZLE_MALLOC_INC;
    pd.aliasLegend[legendId].objects = realloc(pd.aliasLegend[legendId].objects, sizeof(int) * pd.aliasLegend[legendId].objectCapacity);
  }

  pd.aliasLegend[legendId].objectCount++;
}

void addObjectsToAliasLegend(char * name) {
  int legId = aliasLegendId(name);

  for (int i = 0; i < pd.aliasLegend[legId].objectCount; i++) {
    pd.aliasLegend[pd.aliasLegendCount].objects[pd.aliasLegend[pd.aliasLegendCount].objectCount] = pd.aliasLegend[legId].objects[i];
    incAliasLegendObject(pd.aliasLegendCount);
  }
}

void incGlyphLegendObject(int legendId) {
  if (pd.glyphLegend[legendId].objectCount + 1 >= pd.glyphLegend[legendId].objectCapacity) {
    pd.glyphLegend[legendId].objectCapacity += PUZZLE_MALLOC_INC;
    pd.glyphLegend[legendId].objects = realloc(pd.glyphLegend[legendId].objects, sizeof(int) * pd.glyphLegend[legendId].objectCapacity);
  }

  pd.glyphLegend[legendId].objectCount++;
}


void incLayer() {
  if (pd.layerCount + 1 >= pd.layerCapacity) {
    fprintf(stderr, "layer REALLOC\n");
    pd.layerCapacity += PUZZLE_MALLOC_INC;
    pd.layers = realloc(pd.layers, sizeof(Layer) * pd.layerCapacity);
    for (int i = pd.layerCount + 1; i < pd.layerCapacity; i++) {
      pd.layers[i].count = 0;
      pd.layers[i].objectCapacity = 100;
      pd.layers[i].objectIds = malloc(sizeof(int) * pd.layers[i].objectCapacity);
    }
  }
  pd.layerCount++;
}

void addObjectsToLayer(char * name) {
  int legId = aliasLegendId(name);
  if (pd.layers[pd.layerCount].count + pd.aliasLegend[legId].objectCount + 1 >= pd.layers[pd.layerCount].objectCapacity) {
    pd.layers[pd.layerCount].objectCapacity += pd.aliasLegend[legId].objectCount + PUZZLE_MALLOC_INC;
    pd.layers[pd.layerCount].objectIds = realloc(pd.layers[pd.layerCount].objectIds, sizeof(int) * pd.layers[pd.layerCount].objectCapacity);
  }

  for (int i = 0; i < pd.aliasLegend[legId].objectCount; i++) {
    pd.layers[pd.layerCount].objectIds[pd.layers[pd.layerCount].count] = pd.aliasLegend[legId].objects[i];
    pd.layers[pd.layerCount].count++;
  }
}

char * ruleString(int ruleId) {
  char * ruleStr = malloc(sizeof(char) * 4096);

  char lineNumber[80];
  sprintf(lineNumber, "%d. ", pd.rules[ruleId].lineNo);
  strcpy(ruleStr, lineNumber);

  strcat(ruleStr, dirName(pd.rules[ruleId].directionConstraint));
  strcat(ruleStr, " ");
  for (int patternId = 0; patternId < pd.rules[ruleId].matchPatternCount; patternId++) {
    strcat(ruleStr, "[ ");
    for (int partId = 0; partId < pd.rules[ruleId].matchPatterns[patternId].partCount; partId++) {
      for (int identId = 0; identId < pd.rules[ruleId].matchPatterns[patternId].parts[partId].identityCount; identId++) {
        Direction ruleDir = pd.rules[ruleId].matchPatterns[patternId].parts[partId].identity[identId].direction;
        int legendId = pd.rules[ruleId].matchPatterns[patternId].parts[partId].identity[identId].legendId;
        if (ruleDir != UNSPECIFIED) {
          strcat(ruleStr, dirName(ruleDir));
          strcat(ruleStr, " ");
        }
        strcat(ruleStr, aliasLegendKey(legendId));
        strcat(ruleStr, " ");
      }
      if (partId + 1 < pd.rules[ruleId].matchPatterns[patternId].partCount) {
        strcat(ruleStr, " | ");
      }
    }
    strcat(ruleStr, " ]");
  }

  strcat(ruleStr, " -> ");

  for (int patternId = 0; patternId < pd.rules[ruleId].resultPatternCount; patternId++) {
    strcat(ruleStr, "[ ");
    for (int partId = 0; partId < pd.rules[ruleId].resultPatterns[patternId].partCount; partId++) {
      for (int identId = 0; identId < pd.rules[ruleId].resultPatterns[patternId].parts[partId].identityCount; identId++) {
        Direction ruleDir = pd.rules[ruleId].resultPatterns[patternId].parts[partId].identity[identId].direction;
        int legendId = pd.rules[ruleId].resultPatterns[patternId].parts[partId].identity[identId].legendId;
        if (ruleDir != UNSPECIFIED) {
          strcat(ruleStr, dirName(ruleDir));
          strcat(ruleStr, " ");
        }
        strcat(ruleStr, aliasLegendKey(legendId));
          strcat(ruleStr, " ");
      }
      if (partId + 1 < pd.rules[ruleId].resultPatterns[patternId].partCount) {
        strcat(ruleStr, " | ");
      }
    }
    strcat(ruleStr, " ]");
  }

  for (int i = 0; i < pd.rules[ruleId].commandCount; i++) {
    strcat(ruleStr, commandName(pd.rules[ruleId].commands[i]));
  }
  return ruleStr;
}

void printObjects() {
  for (int objectId = 0; objectId < pd.objectCount; objectId++) {
    printf("%s (%i)\n", pd.objects[objectId].name, objectId);
  }
}

void printLegends() {
  for (int legendId = 0; legendId < pd.aliasLegendCount; legendId++) {
    printf("%s (%i): ", pd.aliasLegend[legendId].key, legendId);
    // TODO: we can print the relation here: `and/or`
    for (int objectId = 0; objectId < pd.aliasLegend[legendId].objectCount; objectId++) {
      printf("%s (%i) ", objectName(pd.aliasLegend[legendId].objects[objectId]), pd.aliasLegend[legendId].objects[objectId]);
    }
    printf("\n");
  }
}

void printRules() {
  char * rstr;
  for (int ruleId = 0; ruleId < pd.ruleCount; ruleId++) {
    rstr = ruleString(ruleId);
    printf("%i: %s\n", ruleId, rstr);
    free(rstr);
  }
}

void printLayers() {
  for (int layerId = 0; layerId < pd.layerCount; layerId++) {
    printf("%i: ", layerId);
    for (int layerObjectId = 0; layerObjectId < pd.layers[layerId].count; layerObjectId++) {
      printf("%s (%i) ", objectName(pd.layers[layerId].objectIds[layerObjectId]), pd.layers[layerId].objectIds[layerObjectId]);
    }
    printf("\n");
  }
}

void printDebug() {
  printf("Objects:\n");
  printObjects();
  printf("Legend:\n");
  printLegends();
  printf("Layers:\n");
  printLayers();
  printf("Rules:\n");
  printRules();
  // printWinConditions();
  // printLevels();
}


void initRuleIdentity(RuleIdentity * ruleIdent) {
  ruleIdent->direction = UNSPECIFIED;
  ruleIdent->legendId = 1; // TODO: this is empty, but that isn't clear
}

void initPart(RulePart * part) {
  part->identityCount = 0;
  part->identityCapacity = 1;
  part->isSpread = 0;
  part->identity = malloc(sizeof(RuleIdentity) * part->identityCapacity);
  for (int ident = 0; ident < part->identityCapacity; ident++) {
    initRuleIdentity(&part->identity[ident]);
  }
}

void initPattern(Pattern * ruleState) {
  ruleState->partCount = 0;
  ruleState->partCapacity = 1;
  ruleState->parts = malloc(sizeof(RulePart) * ruleState->partCapacity);
  for (int partId = 0; partId < ruleState->partCapacity; partId++) {
    initPart(&ruleState->parts[partId]);
  }
}

void initRule(Rule * rule) {
  rule->lineNo = 0;
  rule->hasSpread = 0;
  rule->hasRelativeDirection = 0;
  rule->hasMultipleParts = 0;
  rule->hasCompoundDirection = 0;

  rule->directionConstraint = NONE;
  rule->executionTime = NORMAL;
  rule->matchPatternCount = 0;
  rule->matchPatternCapacity = 1;
  rule->commandCount = 0;
  rule->matchPatterns = malloc(sizeof(Pattern) * rule->matchPatternCapacity);
  for (int patternId = 0; patternId < rule->matchPatternCapacity; patternId++) {
    initPattern(&rule->matchPatterns[patternId]);
  }

  rule->resultPatternCount = 0;
  rule->resultPatternCapacity = 1;
  rule->resultPatterns = malloc(sizeof(Pattern) * rule->resultPatternCapacity);
  for (int patternId = 0; patternId < rule->resultPatternCapacity; patternId++) {
    initPattern(&rule->resultPatterns[patternId]);
  }
}

void resetRule(Rule * rule) {
  rule->lineNo = 0;
  rule->hasSpread = 0;

  rule->directionConstraint = NONE;
  rule->executionTime = NORMAL;
  rule->matchPatternCount = 0;
  rule->matchPatternCapacity = 1;
  rule->commandCount = 0;
  free(rule->matchPatterns);
  rule->matchPatterns = malloc(sizeof(Pattern) * rule->matchPatternCapacity);
  for (int patternId = 0; patternId < rule->matchPatternCapacity; patternId++) {
    initPattern(&rule->matchPatterns[patternId]);
  }

  rule->resultPatternCount = 0;
  rule->resultPatternCapacity = 1;
  free(rule->resultPatterns);
  rule->resultPatterns = malloc(sizeof(Pattern) * rule->resultPatternCapacity);
  for (int patternId = 0; patternId < rule->resultPatternCapacity; patternId++) {
    initPattern(&rule->resultPatterns[patternId]);
  }
}


Rule * reallocRule(Rule * rules, int newSize) {
  return realloc(rules, sizeof(Rule) * newSize);
}

void incRule() {
  if (pd.ruleCount + 1 >= pd.ruleCapacity) {
    fprintf(stderr, "RULE REALLOC\n");
    pd.ruleCapacity += PUZZLE_MALLOC_INC;
    pd.rules = reallocRule(pd.rules, pd.ruleCapacity);
    printf("rc: %i rcap: %i\n", pd.ruleCount, pd.ruleCapacity);
    for (int ruleId = pd.ruleCount + 1; ruleId < pd.ruleCapacity; ruleId++) {
      initRule(&pd.rules[ruleId]);
    }
  }
  pd.ruleCount++;
}

void incRuleMatchState(Rule * rule) {
  if (rule->matchPatternCount + 1 >= rule->matchPatternCapacity) {
    rule->matchPatternCapacity += PUZZLE_MALLOC_INC;
    rule->matchPatterns = realloc(rule->matchPatterns, sizeof(Pattern) * rule->matchPatternCapacity);
    for (int patternId = rule->matchPatternCount + 1; patternId < rule->matchPatternCapacity; patternId++) {
      initPattern(&rule->matchPatterns[patternId]);
    }
  }
  rule->matchPatternCount++;
}

void incRuleResultState(Rule * rule) {
  if (rule->resultPatternCount + 1 >= rule->resultPatternCapacity) {
    rule->resultPatternCapacity += PUZZLE_MALLOC_INC;
    rule->resultPatterns = realloc(rule->resultPatterns, sizeof(Pattern) * rule->resultPatternCapacity);

    for (int patternId = rule->resultPatternCount + 1; patternId < rule->resultPatternCapacity; patternId++) {
      initPattern(&rule->resultPatterns[patternId]);
    }
  }
  rule->resultPatternCount++;
}

void incRulePart(Pattern * ruleState) {
  if (ruleState->partCount + 1 >= ruleState->partCapacity) {
    ruleState->partCapacity += PUZZLE_MALLOC_INC;
    ruleState->parts = realloc(ruleState->parts, sizeof(RulePart) * ruleState->partCapacity);
    for (int partId = ruleState->partCount + 1; partId < ruleState->partCapacity; partId++) {
      initPart(&ruleState->parts[partId]);
    }
  }
  ruleState->partCount++;
}

void incRuleIdent(RulePart * part) {
  if (part->identityCount + 1 >= part->identityCapacity) {
    part->identityCapacity += PUZZLE_MALLOC_INC;
    part->identity = realloc(part->identity, sizeof(RuleIdentity) * part->identityCapacity);
    for (int ident = part->identityCount + 1; ident < part->identityCapacity; ident++) {
      initRuleIdentity(&part->identity[ident]);
    }
  }
  part->identityCount++;
}

int ruleCommandCount(int ruleId) {
  return pd.rules[ruleId].commandCount;
}

int ruleCommandContains(Rule * rule, Command cmd) {
  for (int i = 0; i < rule->commandCount; i++) {
    if (rule->commands[i] == cmd) {
      return 1;
    }
  }
  return 0;
}


void incWinCondition() {
  if (pd.winConditionCount + 1 >= pd.winConditionCapacity) {
    fprintf(stderr, "winCondition REALLOC\n");
    pd.winConditionCapacity += PUZZLE_MALLOC_INC;
    pd.winConditions = realloc(pd.winConditions, sizeof(WinCondition) * pd.winConditionCapacity);
  }
  pd.winConditionCount++;
}

void incLevel() {
  if (pd.levelCount + 1 >= pd.levelCapacity) {
    fprintf(stderr, "level REALLOC\n");
    pd.levelCapacity += PUZZLE_MALLOC_INC;
    pd.levels = realloc(pd.levels, sizeof(Level) * pd.levelCapacity);
    for (int i = pd.levelCount + 1; i < pd.levelCapacity; i++) {
      pd.levels[i].tileIndex = 0;
      pd.levels[i].tileCapacity = 0;
      pd.levels[i].height = 0;
      pd.levels[i].width = 0;
    }
  }
  pd.levelCount++;
}

void addTile(int levelId, char tileValue) {
  if (pd.levels[levelId].tileCapacity == 0) {
    pd.levels[levelId].tileCapacity += PUZZLE_MALLOC_INC;
    pd.levels[levelId].tiles = malloc(sizeof(char) * pd.levels[levelId].tileCapacity);
  } else if (pd.levels[levelId].tileIndex + 1 >= pd.levels[levelId].tileCapacity) {
    pd.levels[levelId].tileCapacity += PUZZLE_MALLOC_INC;
    pd.levels[levelId].tiles = realloc(pd.levels[levelId].tiles, sizeof(char) * pd.levels[levelId].tileCapacity);
  }

  pd.levels[pd.levelCount].tiles[pd.levels[pd.levelCount].tileIndex] = tileValue;
  pd.levels[pd.levelCount].tileIndex++;
}

void initStarterObjects() {
  pd.aliasLegend[pd.aliasLegendCount].key = "...";
  pd.aliasLegend[pd.aliasLegendCount].objectCount = 1;
  pd.aliasLegend[pd.aliasLegendCount].objects[0] = pd.objectCount;
  pd.objects[pd.objectCount].name = "_Spread_";
  incObject();
  incAliasLegend();

  pd.aliasLegend[pd.aliasLegendCount].key = "_EMPTY_";
  pd.aliasLegend[pd.aliasLegendCount].objectCount = 1;
  pd.aliasLegend[pd.aliasLegendCount].objects[0] = pd.objectCount;
  pd.objects[pd.objectCount].name = "_Empty_";
  incObject();
  incAliasLegend();
}

void initPuzzleData() {
  pd.noAction = 0;
  pd.noRepeatAction = 0;
  pd.noUndo = 0;
  pd.noRestart = 0;
  pd.requirePlayerMovement = 0;
  pd.throttleMovement = 0;
  pd.runRulesOnLevelStart = 0;
  pd.setRealtimeInterval = 0;
  pd.realTimeInterval = 0;
  pd.setAgainInterval = 0;
  pd.againInterval = 0.1f;
  pd.doesFlickScreen = 0;
  pd.doesZoomScreen = 0;
  pd.title = (char *) malloc(10);
  strcpy(pd.title, "Puzzle");
  pd.author = (char *) malloc(10);
  strcpy(pd.author, "Me");
  pd.homepage = (char *) malloc(20);
  strcpy(pd.homepage, "example.com");
  pd.backgroundColor = (char *) malloc(10);
  strcpy(pd.backgroundColor, "black");
  pd.textColor = (char *) malloc(10);
  strcpy(pd.textColor, "white");

  pd.objectCount = 0;
  pd.objectCapacity = 100;
  pd.objects = malloc(sizeof(Object) * pd.objectCapacity);
  for (int i = 0; i < pd.objectCapacity; i++) {
    pd.objects[i].colorCount = 0;
    pd.objects[i].height = 0;
    pd.objects[i].width = 0;

    // TODO: this should have been done already.
    for (int j = 0; j < 25; j++) {
      pd.objects[i].sprite[j] = '0';
    }
  }

  pd.aliasLegendCount = 0;
  pd.aliasLegendCapacity = 500;
  pd.aliasLegend = malloc(sizeof(AliasLegend) * pd.aliasLegendCapacity);
  for (int i = 0; i < pd.aliasLegendCapacity; i++) {
    pd.aliasLegend[i].objectRelation = LEGEND_RELATION_UNKNOWN;
    pd.aliasLegend[i].objectCount = 0;
    pd.aliasLegend[i].objectCapacity = 10;
    pd.aliasLegend[i].objects = malloc(sizeof(int) * pd.aliasLegend[i].objectCapacity);
  }

  pd.glyphLegendCount = 0;
  pd.glyphLegendCapacity = 300;
  pd.glyphLegend = malloc(sizeof(GlyphLegend) * pd.glyphLegendCapacity);
  for (int i = 0; i < pd.glyphLegendCapacity; i++) {
    pd.glyphLegend[i].objectRelation = LEGEND_RELATION_UNKNOWN;
    pd.glyphLegend[i].objectCount = 0;
    pd.glyphLegend[i].objectCapacity = 10;
    pd.glyphLegend[i].objects = malloc(sizeof(int) * pd.glyphLegend[i].objectCapacity);
  }


  pd.layerCount = 0;
  pd.layerCapacity = 400;
  pd.layers = malloc(sizeof(Layer) * pd.layerCapacity);
  for (int i = 0; i < pd.layerCapacity; i++) {
    pd.layers[i].count = 0;
    pd.layers[i].objectCapacity = 10;
    pd.layers[i].objectIds = malloc(sizeof(int) * pd.layers[i].objectCapacity);
  }

  pd.ruleCount = 0;
  pd.ruleCapacity = 500;
  pd.rules = malloc(sizeof(Rule) * pd.ruleCapacity);
  for (int ruleId = 0; ruleId < pd.ruleCapacity; ruleId++) {
    initRule(&pd.rules[ruleId]);
  }

  pd.winConditionCount = 0;
  pd.winConditionCapacity = 10;
  pd.winConditions = malloc(sizeof(WinCondition) * pd.winConditionCapacity);

  pd.levelCount = 0;
  pd.levelCapacity = 500;
  pd.levels = malloc(sizeof(Level) * pd.levelCapacity);
  for (int i = 0; i < pd.levelCapacity; i++) {
    pd.levels[i].tileIndex = 0;
    pd.levels[i].tileCapacity = 0;
    pd.levels[i].height = 0;
    pd.levels[i].width = 0;
  }
  initStarterObjects();
}

Direction realDirection(Direction applicationDirection, Direction ruleDir) {
  switch (ruleDir) {
  case UP:
  case DOWN:
  case LEFT:
  case RIGHT:
  case USE:
  case RANDOMDIR:
  case STATIONARY:
  case UNSPECIFIED:
  case COND_NO:
  case NONE:
  case RANDOM:
    return ruleDir;
  case HORIZONTAL:
    if (applicationDirection == UP) {
      return LEFT;
    } else if (applicationDirection == DOWN) {
      return RIGHT;
    } else {
      return applicationDirection;
    }
  case VERTICAL:
    if (applicationDirection == LEFT) {
      return UP;
    } else if (applicationDirection == RIGHT) {
      return DOWN;
    } else {
      return applicationDirection;
    }
  case MOVING:
    return applicationDirection;
  case REL_RIGHT:
    return (Direction)((applicationDirection + 0) % 4);
  case REL_UP:
    return (Direction)((applicationDirection + 1) % 4);
  case REL_LEFT:
    return (Direction)((applicationDirection + 2) % 4);
  case REL_DOWN:
    return (Direction)((applicationDirection + 3) % 4);
  case PARALLEL:
    return (Direction)((applicationDirection + 0) % 4);
  case PERPENDICULAR:
    return (Direction)((applicationDirection + 1) % 4);
  default:
    fprintf(stderr, "err: (realDirection) unsupported direction (ad: %i rd: %i)\n", applicationDirection, ruleDir);
    return NONE;
  }
}

void copyPart(RulePart * sourcePart, RulePart * targetPart, Direction appDir, int rotate) {
  for (int identId = 0; identId < sourcePart->identityCount; identId++) {
    targetPart->identity[identId].legendId = sourcePart->identity[identId].legendId;


    Direction ruleDir = sourcePart->identity[identId].direction;
    if (rotate && ruleDir != UNSPECIFIED && ruleDir != COND_NO) {
      targetPart->identity[identId].direction = ((realDirection(appDir, ruleDir) + 2) % 4);
    } else {
      targetPart->identity[identId].direction = realDirection(appDir, ruleDir);
    }
    incRuleIdent(targetPart);
  }
}

int buildRule(Direction appDir, Direction dirCategory, Rule * targetRule, Rule * sourceRule, int rotate) {
  int x = 0;
  if (sourceRule->directionConstraint == NONE ||
      sourceRule->directionConstraint == appDir ||
      sourceRule->directionConstraint == dirCategory) {
    if (appDir == LEFT) {
      targetRule->directionConstraint = RIGHT;
    } else if (appDir == UP) {
      targetRule->directionConstraint = DOWN;
    } else {
      targetRule->directionConstraint = appDir;
    }

    // COPY BASIC VALUES
    targetRule->executionTime = sourceRule->executionTime;
    targetRule->lineNo = sourceRule->lineNo;
    targetRule->hasSpread = sourceRule->hasSpread;
    targetRule->executionTime = sourceRule->executionTime;

    // COPY COMMANDS
    targetRule->commandCount = sourceRule->commandCount;
    for (int commandId = 0; commandId < sourceRule->commandCount; commandId++) {
      targetRule->commands[commandId] = sourceRule->commands[commandId];
    }

    // COPY MATCHES
    for (int patternId = 0; patternId < sourceRule->matchPatternCount; patternId++) {
      if ((appDir == UP || appDir == LEFT) && sourceRule->matchPatterns[patternId].partCount > 0) {
        x = 0;
        for (int partId = sourceRule->matchPatterns[patternId].partCount - 1; partId >= 0; partId--) {
          targetRule->matchPatterns[patternId].parts[x].isSpread = sourceRule->matchPatterns[patternId].parts[partId].isSpread;
          RulePart * sourcePart = &sourceRule->matchPatterns[patternId].parts[partId];
          RulePart * targetPart = &targetRule->matchPatterns[patternId].parts[x];
          copyPart(sourcePart, targetPart, appDir, rotate);
          incRulePart(&targetRule->matchPatterns[patternId]);
          x++;
        }
      } else {
        for (int partId = 0; partId < sourceRule->matchPatterns[patternId].partCount; partId++) {
          targetRule->matchPatterns[patternId].parts[partId].isSpread = sourceRule->matchPatterns[patternId].parts[partId].isSpread;
          RulePart * sourcePart = &sourceRule->matchPatterns[patternId].parts[partId];
          RulePart * targetPart = &targetRule->matchPatterns[patternId].parts[partId];
          copyPart(sourcePart, targetPart, appDir, rotate);
          incRulePart(&targetRule->matchPatterns[patternId]);
        }
      }

      incRuleMatchState(targetRule);
    }

    if (sourceRule->resultPatternCount == 0) {
      return 1;
    }

    // COPY RESULTS
    for (int patternId = 0; patternId < sourceRule->resultPatternCount; patternId++) {
      if ((appDir == UP || appDir == LEFT) && sourceRule->resultPatterns[patternId].partCount > 0) {
        x = 0;
        for (int partId = sourceRule->resultPatterns[patternId].partCount - 1; partId >= 0; partId--) {
          targetRule->resultPatterns[patternId].parts[x].isSpread = sourceRule->resultPatterns[patternId].parts[partId].isSpread;
          RulePart * sourcePart = &sourceRule->resultPatterns[patternId].parts[partId];
          RulePart * targetPart = &targetRule->resultPatterns[patternId].parts[x];
          copyPart(sourcePart, targetPart, appDir, rotate);
          incRulePart(&targetRule->resultPatterns[patternId]);
          x++;
        }
      } else {
        for (int partId = 0; partId < sourceRule->resultPatterns[patternId].partCount; partId++) {
          targetRule->resultPatterns[patternId].parts[x].isSpread = sourceRule->resultPatterns[patternId].parts[partId].isSpread;
          RulePart * sourcePart = &sourceRule->resultPatterns[patternId].parts[partId];
          RulePart * targetPart = &targetRule->resultPatterns[patternId].parts[partId];
          copyPart(sourcePart, targetPart, appDir, rotate);
          incRulePart(&targetRule->resultPatterns[patternId]);
        }
      }

      incRuleResultState(targetRule);
    }
    return 1;
  }
  return 0;
}

void freeRules() {
  for (int ruleId = 0; ruleId < pd.ruleCapacity; ruleId++) {
    for (int patternId = 0; patternId < pd.rules[ruleId].matchPatternCapacity; patternId++) {
      for (int partId = 0; partId < pd.rules[ruleId].matchPatterns[patternId].partCapacity; partId++) {
        free(pd.rules[ruleId].matchPatterns[patternId].parts[partId].identity);
      }
      free(pd.rules[ruleId].matchPatterns[patternId].parts);
    }
    free(pd.rules[ruleId].matchPatterns);

    for (int patternId = 0; patternId < pd.rules[ruleId].resultPatternCapacity; patternId++) {
      for (int partId = 0; partId < pd.rules[ruleId].resultPatterns[patternId].partCapacity; partId++) {
        free(pd.rules[ruleId].resultPatterns[patternId].parts[partId].identity);
      }
        free(pd.rules[ruleId].resultPatterns[patternId].parts);
    }
    free(pd.rules[ruleId].resultPatterns);
  }
  free(pd.rules);
  pd.ruleCapacity = 0;
  pd.ruleCount = 0;
}

void expandRules() {
  int ruleCount = 0;
  int ruleCapacity = 800;

  Rule * rules = malloc(sizeof(Rule) * ruleCapacity);
  for (int ruleId = 0; ruleId < ruleCapacity; ruleId++) {
    initRule(&rules[ruleId]);
  }

  for (int ruleId = 0; ruleId < pd.ruleCount; ruleId++) {
    if (buildRule(DOWN, VERTICAL, &rules[ruleCount], &pd.rules[ruleId], 0)) {
      if (ruleCount + 1 >= ruleCapacity) {
        ruleCapacity += PUZZLE_MALLOC_INC;
        rules = reallocRule(rules, ruleCapacity);
      }
      rules[ruleCount].id = ruleCount;
      ruleCount++;
    }

    if (pd.rules[ruleId].hasMultipleParts || pd.rules[ruleId].hasRelativeDirection || pd.rules[ruleId].directionConstraint != NONE) {
      if (buildRule(UP, VERTICAL, &rules[ruleCount], &pd.rules[ruleId], 0)) {
        if (ruleCount + 1 >= ruleCapacity) {
          ruleCapacity += PUZZLE_MALLOC_INC;
          rules = reallocRule(rules, ruleCapacity);
        }
        rules[ruleCount].id = ruleCount;
        ruleCount++;
      }

      if (buildRule(LEFT, HORIZONTAL, &rules[ruleCount], &pd.rules[ruleId], 0)) {
        if (ruleCount + 1 >= ruleCapacity) {
          ruleCapacity += PUZZLE_MALLOC_INC;
          rules = reallocRule(rules, ruleCapacity);
        }
        rules[ruleCount].id = ruleCount;
        ruleCount++;
      }
      if (buildRule(RIGHT, HORIZONTAL, &rules[ruleCount], &pd.rules[ruleId], 0)) {
        if (ruleCount + 1 >= ruleCapacity) {
          ruleCapacity += PUZZLE_MALLOC_INC;
          rules = reallocRule(rules, ruleCapacity);
        }
        rules[ruleCount].id = ruleCount;
        ruleCount++;
      }

      // Reverse rules for
      if (pd.rules[ruleId].hasCompoundDirection) {
        if (buildRule(UP, VERTICAL, &rules[ruleCount], &pd.rules[ruleId], 1)) {
          if (ruleCount + 1 >= ruleCapacity) {
            ruleCapacity += PUZZLE_MALLOC_INC;
            rules = reallocRule(rules, ruleCapacity);
          }
          rules[ruleCount].id = ruleCount;
          ruleCount++;
        }

        if (buildRule(DOWN, VERTICAL, &rules[ruleCount], &pd.rules[ruleId], 1)) {
          if (ruleCount + 1 >= ruleCapacity) {
            ruleCapacity += PUZZLE_MALLOC_INC;
            rules = reallocRule(rules, ruleCapacity);
          }
          rules[ruleCount].id = ruleCount;
          ruleCount++;
        }

        if (buildRule(LEFT, HORIZONTAL, &rules[ruleCount], &pd.rules[ruleId], 1)) {
          if (ruleCount + 1 >= ruleCapacity) {
            ruleCapacity += PUZZLE_MALLOC_INC;
            rules = reallocRule(rules, ruleCapacity);
          }
          rules[ruleCount].id = ruleCount;
          ruleCount++;
        }

        if (buildRule(RIGHT, HORIZONTAL, &rules[ruleCount], &pd.rules[ruleId], 1)) {
          if (ruleCount + 1 >= ruleCapacity) {
            ruleCapacity += PUZZLE_MALLOC_INC;
            rules = reallocRule(rules, ruleCapacity);
          }
          rules[ruleCount].id = ruleCount;
          ruleCount++;
        }
      }
    }
  }

  freeRules();
  pd.ruleCount = ruleCount;
  pd.ruleCapacity = ruleCapacity;
  pd.rules = rules;
}

void makeLegendMasks() {
  for (int legendId = 0; legendId < pd.aliasLegendCount; legendId++) {
    pd.aliasLegend[legendId].mask = calloc(pd.objectCount/8+1, 1);
    for (int objectId = 0; objectId < pd.aliasLegend[legendId].objectCount; objectId++) {
      unsigned int element = pd.aliasLegend[legendId].objects[objectId];
      unsigned int byte_index = element/8;
      unsigned int bit_index = element % 8;
      unsigned int bit_mask = (1 << bit_index);

      if ((pd.aliasLegend[legendId].mask[byte_index] & bit_mask) == 0) {
        pd.aliasLegend[legendId].mask[byte_index] |= bit_mask;
      }
    }
  }
}

PuzzleData * parsePuzzle(FILE * file) {
  initPuzzleData();
  yyin = file;
  yyparse();

  expandRules();
  makeLegendMasks();

  if (pd.debug) {
    printDebug();
  }

  return &pd;
}

void freePuzzle() {
  // TODO: we start at 2 because the first two objects are "static" basic objects.
  // We can make that explicit
  free(pd.title);
  free(pd.author);
  free(pd.homepage);
  free(pd.textColor);
  free(pd.backgroundColor);

  /* for (int i = 2; i < pd.objectCount; i++) { */
  /*   free(pd.objects[i].name); */
  /* } */

  for (int i = 2; i < pd.objectCount; i++) {
    free(pd.objects[i].name);
    if (pd.objects[i].colorCount > 0) {
      for (int c = 0; c < pd.objects[i].colorCount; c++) {
        free(pd.objects[i].colors[c]);
      }
    }
  }
  free(pd.objects);

  for (int i = 0; i < pd.aliasLegendCapacity; i++) {
    free(pd.aliasLegend[i].objects);
  }

  for (int i = 2; i < pd.aliasLegendCount; i++) {
    free(pd.aliasLegend[i].key);
    free(pd.aliasLegend[i].mask);
  }
  free(pd.aliasLegend);

  for (int i = 0; i < pd.glyphLegendCapacity; i++) {
    free(pd.glyphLegend[i].objects);
  }
  free(pd.glyphLegend);

  for (int i = 0; i < pd.layerCapacity; i++) {
    free(pd.layers[i].objectIds);
  }
  free(pd.layers);

  freeRules();

  /* for (int i = 0; i < pd.levelCapacity; i++) { */
  /*     free(pd.levels[i].tiles); */
  /* } */

  for (int i = 0; i < pd.levelCount; i++) {
    if (pd.levels[i].levelType == SQUARES) {
      if (pd.levels[i].tileCapacity > 0) {
        free(pd.levels[i].tiles);
      }
    } else {
      free(pd.levels[i].message);
    }
  }
  free(pd.levels);

  free(pd.winConditions);
}


Object * object(int id) {
  return &pd.objects[id];
}

char * objectName(int id) {
  return pd.objects[id].name;
}

int objectSpriteTile(int id, int i) {
  return pd.objects[id].sprite[i];
}

char * objectColor(int id, int index) {
  if (index < pd.objects[id].colorCount) {
    return pd.objects[id].colors[index];
  } else {
    fprintf(stderr, "Color out of bounds\n");
    return "!";
  }

}

int glyphLegendContains(int legendId, int objId) {
  for (int i = 0; i < pd.glyphLegend[legendId].objectCount; i++) {
    if (objId == pd.glyphLegend[legendId].objects[i]) {
      return 1;
    }
  }
  return 0;
}

char objectGlyph(int objId) {
  for (int i = 0; i < pd.glyphLegendCount; i++) {
    if (pd.glyphLegend[i].objectCount == 1 && glyphLegendContains(i, objId) == 1) {
      return pd.glyphLegend[i].key;
    }
  }
  fprintf(stderr, "err: no key found '%s' (%i)\n", objectName(objId), objId);
  return '!';
}

int legendIdForGlyph(char glyph) {
  for (int i = 0; i < pd.glyphLegendCount; i++) {
    if (tolower(pd.glyphLegend[i].key) == tolower(glyph)) {
      return i;
    }
  }

  fprintf(stderr, "err: no id found '%c'\n", glyph);
  return -1;
}

int aliasLegendContains(int legendId, int objId) {
  int byte_index = objId/8;
  int bit_index = objId % 8;
  int bit_mask = (1 << bit_index);
  return ((pd.aliasLegend[legendId].mask[byte_index] & bit_mask) != 0);
}

int objectLayer(int objId) {
  // TODO: we should catch this earlier
  if (objId == -1 || aliasLegendContains(EMPTY_ID, objId)) {
    return -1;
  }

  for (int i = 0; i < pd.layerCount; i++) {
    for (int j = 0; j < pd.layers[i].count; j++) {
      if (objId == pd.layers[i].objectIds[j]) {
        return i;
      }
    }
  }
  fprintf(stderr, "err: layer not found for objid: '%s' (%i) \n", objectName(objId), objId);
  return -1;
}

int levelTileCount(int levelIndex) {
  return pd.levels[levelIndex].tileIndex;
}

int levelTile(int levelIndex, int tileIndex) {
  return pd.levels[levelIndex].tiles[tileIndex];
}

LevelType levelType(int levelIndex) {
  return pd.levels[levelIndex].levelType;
}

char * levelMessage(int levelIndex) {
  return pd.levels[levelIndex].message;
}

int layerIncludes(int layerId, int objId) {
  for (int i = 0; i < pd.layers[layerId].count; i++) {
    if (pd.layers[layerId].objectIds[i] == objId) {
      return 1;
    }
  }
  return 0;
}

int objectCount() {
  return pd.objectCount;
}

/* int aliasLegendCount() { */
/*   return pd.aliasLegendCount; */
/* } */

/* int glyphLegendCount() { */
/*   return pd.glyphLegendCount; */
/* } */

char * aliasLegendKey(int id) {
  if (id == 1) {
    return "";
  }
  return pd.aliasLegend[id].key;
}

char glyphLegendKey(int id) {
  return pd.glyphLegend[id].key;
}

int winConditionCount() {
  return pd.winConditionCount;
}

WinCondition * winCondition(int winConditionIndex) {
   return &pd.winConditions[winConditionIndex];
}
