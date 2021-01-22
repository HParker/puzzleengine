// Interface to puzzle data extracted from the puzzlescript file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "puzzleData.h"

extern PuzzleData pd;

#define PUZZLE_MALLOC_INC 10;

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
      pd.layers[i].width = 0;
      pd.layers[i].objectCapacity = 100;
      pd.layers[i].objectIds = malloc(sizeof(int) * pd.layers[i].objectCapacity);
    }
  }
  pd.layerCount++;
}

void addObjectsToLayer(char * name) {
  if (pd.layers[pd.layerCount].width + 1 >= pd.layers[pd.layerCount].objectCapacity) {
    pd.layers[pd.layerCount].objectCapacity += PUZZLE_MALLOC_INC;
    pd.layers[pd.layerCount].objectIds = realloc(pd.layers[pd.layerCount].objectIds, sizeof(int) * pd.layers[pd.layerCount].objectCapacity);
  }

  int legId = aliasLegendId(name);
  for (int i = 0; i < pd.aliasLegend[legId].objectCount; i++) {
    pd.layers[pd.layerCount].objectIds[pd.layers[pd.layerCount].width] = pd.aliasLegend[legId].objects[i];
    pd.layers[pd.layerCount].width++;
  }
}

char * ruleString(int ruleId) {
  char * ruleStr = malloc(sizeof(char) * 4096);

  char lineNumber[80];
  sprintf(lineNumber, "%d. ", pd.rules[ruleId].lineNo);
  strcpy(ruleStr, lineNumber);

  strcat(ruleStr, dirName(pd.rules[ruleId].directionConstraint));
  strcat(ruleStr, " ");
  for (int stateId = 0; stateId < pd.rules[ruleId].matchStateCount; stateId++) {
    strcat(ruleStr, "[ ");
    for (int partId = 0; partId < pd.rules[ruleId].matchStates[stateId].partCount; partId++) {
      for (int identId = 0; identId < pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCount; identId++) {
        Direction ruleDir = pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity[identId].direction;
        int legendId = pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity[identId].legendId;
        if (ruleDir != UNSPECIFIED) {
          strcat(ruleStr, dirName(ruleDir));
          strcat(ruleStr, " ");
        }
        strcat(ruleStr, aliasLegendKey(legendId));

        if (partId + 1 < pd.rules[ruleId].matchStates[stateId].partCount) {
          strcat(ruleStr, " | ");
        }
      }
    }
    strcat(ruleStr, " ]");
  }

  strcat(ruleStr, " -> ");

  for (int stateId = 0; stateId < pd.rules[ruleId].resultStateCount; stateId++) {
    strcat(ruleStr, "[ ");
    for (int partId = 0; partId < pd.rules[ruleId].resultStates[stateId].partCount; partId++) {
      for (int identId = 0; identId < pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCount; identId++) {
        Direction ruleDir = pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity[identId].direction;
        int legendId = pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity[identId].legendId;
        if (ruleDir != UNSPECIFIED) {
          strcat(ruleStr, dirName(ruleDir));
          strcat(ruleStr, " ");
        }
        strcat(ruleStr, aliasLegendKey(legendId));

        if (partId + 1 < pd.rules[ruleId].resultStates[stateId].partCount) {
          strcat(ruleStr, " | ");
        }
      }
    }
    strcat(ruleStr, " ]");
  }

  for (int i = 0; i < pd.rules[ruleId].commandCount; i++) {
    strcat(ruleStr, commandName(pd.rules[ruleId].commands[i]));
  }
  return ruleStr;
}

void printRules() {
  char * rstr;
  for (int ruleId = 0; ruleId < pd.ruleCount; ruleId++) {
    rstr = ruleString(ruleId);
    printf("%i: %s\n", ruleId, rstr);
    free(rstr);
  }
}

void initRuleIdentity(RuleIdentity * ruleIdent) {
  ruleIdent->direction = UNSPECIFIED;
  ruleIdent->legendId = 1; // TODO: this is empty, but that isn't clear
}

void initPart(RulePart * part) {
  part->ruleIdentityCount = 0;
  part->ruleIdentityCapacity = 1;
  part->isSpread = 0;
  part->ruleIdentity = malloc(sizeof(RuleIdentity) * part->ruleIdentityCapacity);
  for (int ident = 0; ident < part->ruleIdentityCapacity; ident++) {
    initRuleIdentity(&part->ruleIdentity[ident]);
  }
}

void initState(RuleState * ruleState) {
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
  rule->matchStateCount = 0;
  rule->matchStateCapacity = 1;
  rule->commandCount = 0;
  rule->matchStates = malloc(sizeof(RuleState) * rule->matchStateCapacity);
  for (int stateId = 0; stateId < rule->matchStateCapacity; stateId++) {
    initState(&rule->matchStates[stateId]);
  }

  rule->resultStateCount = 0;
  rule->resultStateCapacity = 1;
  rule->resultStates = malloc(sizeof(RuleState) * rule->resultStateCapacity);
  for (int stateId = 0; stateId < rule->resultStateCapacity; stateId++) {
    initState(&rule->resultStates[stateId]);
  }
}

void resetRule(Rule * rule) {
  rule->lineNo = 0;
  rule->hasSpread = 0;

  rule->directionConstraint = NONE;
  rule->executionTime = NORMAL;
  rule->matchStateCount = 0;
  rule->matchStateCapacity = 1;
  rule->commandCount = 0;
  free(rule->matchStates);
  rule->matchStates = malloc(sizeof(RuleState) * rule->matchStateCapacity);
  for (int stateId = 0; stateId < rule->matchStateCapacity; stateId++) {
    initState(&rule->matchStates[stateId]);
  }

  rule->resultStateCount = 0;
  rule->resultStateCapacity = 1;
  free(rule->resultStates);
  rule->resultStates = malloc(sizeof(RuleState) * rule->resultStateCapacity);
  for (int stateId = 0; stateId < rule->resultStateCapacity; stateId++) {
    initState(&rule->resultStates[stateId]);
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
  if (rule->matchStateCount + 1 >= rule->matchStateCapacity) {
    rule->matchStateCapacity += PUZZLE_MALLOC_INC;
    rule->matchStates = realloc(rule->matchStates, sizeof(RuleState) * rule->matchStateCapacity);
    for (int stateId = rule->matchStateCount + 1; stateId < rule->matchStateCapacity; stateId++) {
      initState(&rule->matchStates[stateId]);
    }
  }
  rule->matchStateCount++;
}

void incRuleResultState(Rule * rule) {
  if (rule->resultStateCount + 1 >= rule->resultStateCapacity) {
    rule->resultStateCapacity += PUZZLE_MALLOC_INC;
    rule->resultStates = realloc(rule->resultStates, sizeof(RuleState) * rule->resultStateCapacity);

    for (int stateId = rule->resultStateCount + 1; stateId < rule->resultStateCapacity; stateId++) {
      initState(&rule->resultStates[stateId]);
    }
  }
  rule->resultStateCount++;
}

void incRulePart(RuleState * ruleState) {
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
  if (part->ruleIdentityCount + 1 >= part->ruleIdentityCapacity) {
    part->ruleIdentityCapacity += PUZZLE_MALLOC_INC;
    part->ruleIdentity = realloc(part->ruleIdentity, sizeof(RuleIdentity) * part->ruleIdentityCapacity);
    for (int ident = part->ruleIdentityCount + 1; ident < part->ruleIdentityCapacity; ident++) {
      initRuleIdentity(&part->ruleIdentity[ident]);
    }
  }
  part->ruleIdentityCount++;
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
      pd.levels[i].cellIndex = 0;
      pd.levels[i].cellCapacity = 100;
      pd.levels[i].cells = malloc(sizeof(char) * pd.levels[i].cellCapacity);
      pd.levels[i].height = 0;
      pd.levels[i].width = 0;
    }
  }
  pd.levelCount++;
}

void incCellIndex(int levelId) {
  if (pd.levels[levelId].cellIndex + 1 >= pd.levels[levelId].cellCapacity) {
    pd.levels[levelId].cellCapacity += PUZZLE_MALLOC_INC;
    pd.levels[levelId].cells = realloc(pd.levels[levelId].cells, sizeof(char) * pd.levels[levelId].cellCapacity);
  }
  pd.levels[levelId].cellIndex++;
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
  pd.realTimeInterval = 0;
  pd.backgroundColor = "black";
  pd.textColor = "white";

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
  pd.aliasLegendCapacity = 100;
  pd.aliasLegend = malloc(sizeof(AliasLegend) * pd.aliasLegendCapacity);
  for (int i = 0; i < pd.aliasLegendCapacity; i++) {
    pd.aliasLegend[i].objectRelation = LEGEND_RELATION_UNKNOWN;
    pd.aliasLegend[i].objectCount = 0;
    pd.aliasLegend[i].objectCapacity = 10;
    pd.aliasLegend[i].objects = malloc(sizeof(int) * pd.aliasLegend[i].objectCapacity);
  }

  pd.glyphLegendCount = 0;
  pd.glyphLegendCapacity = 100;
  pd.glyphLegend = malloc(sizeof(GlyphLegend) * pd.glyphLegendCapacity);
  for (int i = 0; i < pd.glyphLegendCapacity; i++) {
    pd.glyphLegend[i].objectRelation = LEGEND_RELATION_UNKNOWN;
    pd.glyphLegend[i].objectCount = 0;
    pd.glyphLegend[i].objectCapacity = 10;
    pd.glyphLegend[i].objects = malloc(sizeof(int) * pd.glyphLegend[i].objectCapacity);
  }


  pd.layerCount = 0;
  pd.layerCapacity = 100;
  pd.layers = malloc(sizeof(Layer) * pd.layerCapacity);
  for (int i = 0; i < pd.layerCapacity; i++) {
    pd.layers[i].width = 0;
    pd.layers[i].objectCapacity = 10;
    pd.layers[i].objectIds = malloc(sizeof(int) * pd.layers[i].objectCapacity);
  }

  pd.ruleCount = 0;
  pd.ruleCapacity = 100;
  pd.rules = malloc(sizeof(Rule) * pd.ruleCapacity);
  for (int ruleId = 0; ruleId < pd.ruleCapacity; ruleId++) {
    initRule(&pd.rules[ruleId]);
  }

  pd.winConditionCount = 0;
  pd.winConditionCapacity = 10;
  pd.winConditions = malloc(sizeof(WinCondition) * pd.winConditionCapacity);

  pd.levelCount = 0;
  pd.levelCapacity = 100;
  pd.levels = malloc(sizeof(Level) * pd.levelCapacity);
  for (int i = 0; i < pd.levelCapacity; i++) {
    pd.levels[i].cellIndex = 0;
    pd.levels[i].cellCapacity = 1;
    pd.levels[i].cells = malloc(sizeof(char) * pd.levels[i].cellCapacity);
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

int buildRule(Direction appDir, Direction dirCategory, Rule * targetRule, Rule * sourceRule) {
  if (sourceRule->directionConstraint == NONE ||
      sourceRule->directionConstraint == appDir ||
      sourceRule->directionConstraint == dirCategory) {
    targetRule->directionConstraint = appDir;
    targetRule->executionTime = sourceRule->executionTime;
    targetRule->lineNo = sourceRule->lineNo;

    targetRule->commandCount = sourceRule->commandCount;
    for (int commandId = 0; commandId < sourceRule->commandCount; commandId++) {
      targetRule->commands[commandId] = sourceRule->commands[commandId];
    }
    targetRule->directionConstraint = appDir;
    targetRule->executionTime = sourceRule->executionTime;

    for (int stateId = 0; stateId < sourceRule->matchStateCount; stateId++) {
      for (int partId = 0; partId < sourceRule->matchStates[stateId].partCount; partId++) {
        targetRule->matchStates[stateId].parts[partId].isSpread = sourceRule->matchStates[stateId].parts[partId].isSpread;
        for (int identId = 0; identId < sourceRule->matchStates[stateId].parts[partId].ruleIdentityCount; identId++) {
          Direction ruleDir = sourceRule->matchStates[stateId].parts[partId].ruleIdentity[identId].direction;
          int legendId = sourceRule->matchStates[stateId].parts[partId].ruleIdentity[identId].legendId;
          targetRule->matchStates[stateId].parts[partId].ruleIdentity[identId].direction = realDirection(appDir, ruleDir);
          targetRule->matchStates[stateId].parts[partId].ruleIdentity[identId].legendId = legendId;
          incRuleIdent(&targetRule->matchStates[stateId].parts[partId]);
        }
        incRulePart(&targetRule->matchStates[stateId]);
      }
      incRuleMatchState(targetRule);
    }

    if (sourceRule->resultStateCount == 0) {
      return 1;
    }

    for (int stateId = 0; stateId < sourceRule->resultStateCount; stateId++) {
      for (int partId = 0; partId < sourceRule->resultStates[stateId].partCount; partId++) {
        targetRule->resultStates[stateId].parts[partId].isSpread = sourceRule->resultStates[stateId].parts[partId].isSpread;
        for (int identId = 0; identId < sourceRule->resultStates[stateId].parts[partId].ruleIdentityCount; identId++) {
          Direction ruleDir = sourceRule->resultStates[stateId].parts[partId].ruleIdentity[identId].direction;
          int legendId = sourceRule->resultStates[stateId].parts[partId].ruleIdentity[identId].legendId;
          targetRule->resultStates[stateId].parts[partId].ruleIdentity[identId].direction = realDirection(appDir, ruleDir);
          targetRule->resultStates[stateId].parts[partId].ruleIdentity[identId].legendId = legendId;
          incRuleIdent(&targetRule->resultStates[stateId].parts[partId]);
        }
        incRulePart(&targetRule->resultStates[stateId]);
      }
      incRuleResultState(targetRule);
    }
    return 1;
  }
  return 0;
}

int buildReverseRule(Direction appDir, Direction dirCategory, Rule * targetRule, Rule * sourceRule) {
  if (sourceRule->directionConstraint == NONE ||
      sourceRule->directionConstraint == appDir ||
      sourceRule->directionConstraint == dirCategory) {
    targetRule->directionConstraint = appDir;
    targetRule->executionTime = sourceRule->executionTime;
    targetRule->lineNo = sourceRule->lineNo;

    targetRule->commandCount = sourceRule->commandCount;
    for (int commandId = 0; commandId < sourceRule->commandCount; commandId++) {
      targetRule->commands[commandId] = sourceRule->commands[commandId];
    }
    targetRule->directionConstraint = appDir;
    targetRule->executionTime = sourceRule->executionTime;

    for (int stateId = 0; stateId < sourceRule->matchStateCount; stateId++) {
      for (int partId = 0; partId < sourceRule->matchStates[stateId].partCount; partId++) {
        targetRule->matchStates[stateId].parts[partId].isSpread = sourceRule->matchStates[stateId].parts[partId].isSpread;
        for (int identId = 0; identId < sourceRule->matchStates[stateId].parts[partId].ruleIdentityCount; identId++) {
          Direction ruleDir = sourceRule->matchStates[stateId].parts[partId].ruleIdentity[identId].direction;
          int legendId = sourceRule->matchStates[stateId].parts[partId].ruleIdentity[identId].legendId;
          // This is the part that is different from `buildRule`
          targetRule->matchStates[stateId].parts[partId].ruleIdentity[identId].direction = ((realDirection(appDir, ruleDir) + 2) % 4);
          targetRule->matchStates[stateId].parts[partId].ruleIdentity[identId].legendId = legendId;
          incRuleIdent(&targetRule->matchStates[stateId].parts[partId]);

        }
        incRulePart(&targetRule->matchStates[stateId]);
      }
      incRuleMatchState(targetRule);
    }

    if (sourceRule->resultStateCount == 0) {
      return 1;
    }

    for (int stateId = 0; stateId < sourceRule->resultStateCount; stateId++) {
      for (int partId = 0; partId < sourceRule->resultStates[stateId].partCount; partId++) {
        targetRule->resultStates[stateId].parts[partId].isSpread = sourceRule->resultStates[stateId].parts[partId].isSpread;
        for (int identId = 0; identId < sourceRule->resultStates[stateId].parts[partId].ruleIdentityCount; identId++) {
          Direction ruleDir = sourceRule->resultStates[stateId].parts[partId].ruleIdentity[identId].direction;
          int legendId = sourceRule->resultStates[stateId].parts[partId].ruleIdentity[identId].legendId;
          // This is the part that is different from `buildRule`
          targetRule->resultStates[stateId].parts[partId].ruleIdentity[identId].direction = ((realDirection(appDir, ruleDir) + 2) % 4);
          targetRule->resultStates[stateId].parts[partId].ruleIdentity[identId].legendId = legendId;
          incRuleIdent(&targetRule->resultStates[stateId].parts[partId]);

        }
        incRulePart(&targetRule->resultStates[stateId]);
      }
      incRuleResultState(targetRule);
    }
    return 1;
  }
  return 0;
}

void freeRules() {
  for (int ruleId = 0; ruleId < pd.ruleCapacity; ruleId++) {
    for (int stateId = 0; stateId < pd.rules[ruleId].matchStateCapacity; stateId++) {
      for (int partId = 0; partId < pd.rules[ruleId].matchStates[stateId].partCapacity; partId++) {
        free(pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity);
      }
      free(pd.rules[ruleId].matchStates[stateId].parts);
    }
    free(pd.rules[ruleId].matchStates);

    for (int stateId = 0; stateId < pd.rules[ruleId].resultStateCapacity; stateId++) {
      for (int partId = 0; partId < pd.rules[ruleId].resultStates[stateId].partCapacity; partId++) {
        free(pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity);
      }
        free(pd.rules[ruleId].resultStates[stateId].parts);
    }
    free(pd.rules[ruleId].resultStates);
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
    if (buildRule(DOWN, VERTICAL, &rules[ruleCount], &pd.rules[ruleId])) {
      if (ruleCount + 1 >= ruleCapacity) {
        ruleCapacity += PUZZLE_MALLOC_INC;
        rules = reallocRule(rules, ruleCapacity);
      }
      rules[ruleCount].id = ruleCount;
      ruleCount++;
    } else {
      resetRule(&rules[ruleCount]);
    }

    if (pd.rules[ruleId].hasMultipleParts || pd.rules[ruleId].hasRelativeDirection || pd.rules[ruleId].directionConstraint != NONE) {
      if (buildRule(UP, VERTICAL, &rules[ruleCount], &pd.rules[ruleId])) {
        if (ruleCount + 1 >= ruleCapacity) {
          ruleCapacity += PUZZLE_MALLOC_INC;
          rules = reallocRule(rules, ruleCapacity);
        }
        rules[ruleCount].id = ruleCount;
        ruleCount++;
      } else {
        resetRule(&rules[ruleCount]);
      }

      if (buildRule(LEFT, HORIZONTAL, &rules[ruleCount], &pd.rules[ruleId])) {
        if (ruleCount + 1 >= ruleCapacity) {
          ruleCapacity += PUZZLE_MALLOC_INC;
          rules = reallocRule(rules, ruleCapacity);
        }
        rules[ruleCount].id = ruleCount;
        ruleCount++;
      } else {
        resetRule(&rules[ruleCount]);
      }
      if (buildRule(RIGHT, HORIZONTAL, &rules[ruleCount], &pd.rules[ruleId])) {
        if (ruleCount + 1 >= ruleCapacity) {
          ruleCapacity += PUZZLE_MALLOC_INC;
          rules = reallocRule(rules, ruleCapacity);
        }
        rules[ruleCount].id = ruleCount;
        ruleCount++;
      } else {
        resetRule(&rules[ruleCount]);
      }

      // Reverse rules for
      if (pd.rules[ruleId].hasCompoundDirection) {
        if (buildReverseRule(UP, VERTICAL, &rules[ruleCount], &pd.rules[ruleId])) {
          if (ruleCount + 1 >= ruleCapacity) {
            ruleCapacity += PUZZLE_MALLOC_INC;
            rules = reallocRule(rules, ruleCapacity);
          }
          rules[ruleCount].id = ruleCount;
          ruleCount++;
        } else {
          resetRule(&rules[ruleCount]);
        }

        if (buildReverseRule(DOWN, VERTICAL, &rules[ruleCount], &pd.rules[ruleId])) {
          if (ruleCount + 1 >= ruleCapacity) {
            ruleCapacity += PUZZLE_MALLOC_INC;
            rules = reallocRule(rules, ruleCapacity);
          }
          rules[ruleCount].id = ruleCount;
          ruleCount++;
        } else {
          resetRule(&rules[ruleCount]);
        }

        if (buildReverseRule(LEFT, HORIZONTAL, &rules[ruleCount], &pd.rules[ruleId])) {
          if (ruleCount + 1 >= ruleCapacity) {
            ruleCapacity += PUZZLE_MALLOC_INC;
            rules = reallocRule(rules, ruleCapacity);
          }
          rules[ruleCount].id = ruleCount;
          ruleCount++;
        } else {
          resetRule(&rules[ruleCount]);
        }

        if (buildReverseRule(RIGHT, HORIZONTAL, &rules[ruleCount], &pd.rules[ruleId])) {
          if (ruleCount + 1 >= ruleCapacity) {
            ruleCapacity += PUZZLE_MALLOC_INC;
            rules = reallocRule(rules, ruleCapacity);
          }
          rules[ruleCount].id = ruleCount;
          ruleCount++;
        } else {
          resetRule(&rules[ruleCount]);
        }
      }
    }
  }

  freeRules();
  pd.ruleCount = ruleCount;
  pd.ruleCapacity = ruleCapacity;
  pd.rules = rules;
}

PuzzleData * parsePuzzle(FILE * file) {
  initPuzzleData();
  yyin = file;
  yyparse();

  expandRules();

  if (pd.debug) {
    printRules();
  }

  return &pd;
}

void freePuzzle() {
  // TODO: we start at 2 because the first two objects are "static" basic objects.
  // We can make that explicit
  for (int i = 2; i < pd.objectCount; i++) {
    free(pd.objects[i].name);
  }
  free(pd.objects);

  /* for (int i = 0; i < pd.aliasLegendCount; i++) { */
  /*   free(pd.aliasLegend[i].key); */
  /* } */

  for (int i = 0; i < pd.aliasLegendCapacity; i++) {
    free(pd.aliasLegend[i].objects);
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

  for (int i = 0; i < pd.levelCapacity; i++) {
    free(pd.levels[i].cells);
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

int objectSpriteCell(int id, int i) {
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
    if (toupper(pd.glyphLegend[i].key) == toupper(glyph)) {
      return i;
    }
  }
  fprintf(stderr, "err: no id found '%c'\n", glyph);
  return -1;
}

int aliasLegendContains(int legendId, int objId) {
  for (int i = 0; i < pd.aliasLegend[legendId].objectCount; i++) {
    if (objId == pd.aliasLegend[legendId].objects[i]) {
      return 1;
    }
  }
  return 0;
}

int idForGlyph(char glyph) {
  for (int i = 0; i < pd.glyphLegendCount; i++) {
    if (toupper(pd.glyphLegend[i].key) == toupper(glyph)) {
      if (pd.glyphLegend[i].objectCount > 1) {
        fprintf(stderr, "err: multi object key '%c'\n", glyph);
      } else {
        return pd.glyphLegend[i].objects[0];
      }
    }
  }
  fprintf(stderr, "err: no id found '%c'\n", glyph);
  return -1;
}

int objectLayer(int objId) {
  if (aliasLegendContains(aliasLegendId("_Empty_"), objId) || objId == -1) {
    // TODO: we should catch this earlier
    return -1;
  }

  for (int i = 0; i < pd.layerCount; i++) {
    for (int j = 0; j < pd.layers[i].width; j++) {
      if (objId == pd.layers[i].objectIds[j]) {
        return i;
      }
    }
  }
  fprintf(stderr, "err: layer not found for objid: '%s' (%i) \n", objectName(objId), objId);
  return -1;
}

int levelHeight(int levelIndex) {
  if (levelIndex < pd.levelCount) {
    return pd.levels[levelIndex].height;
  } else {
    fprintf(stderr, "err: asked for level out of bounds\n");
    return -1;
  }
}

int levelWidth(int levelIndex) {
  if (levelIndex < pd.levelCount) {
    return pd.levels[levelIndex].width;
  } else {
    fprintf(stderr, "err: asked for level out of bounds\n");
    return -1;
  }
}

int levelCellCount(int levelIndex) {
  return pd.levels[levelIndex].cellIndex;
}

int levelCell(int levelIndex, int cellIndex) {
  return pd.levels[levelIndex].cells[cellIndex];
}

LevelType levelType(int levelIndex) {
  return pd.levels[levelIndex].levelType;
}

char * levelMessage(int levelIndex) {
  return pd.levels[levelIndex].message;
}

int layerIncludes(int layerId, int objId) {
  for (int i = 0; i < pd.layers[layerId].width; i++) {
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


int ruleCount() {
  return pd.ruleCount;
}

Rule * rule(int index) {
  return &pd.rules[index];
}


int levelCount() {
  return pd.levelCount;
}

int winConditionCount() {
  return pd.winConditionCount;
}

WinCondition * winCondition(int winConditionIndex) {
   return &pd.winConditions[winConditionIndex];
}
