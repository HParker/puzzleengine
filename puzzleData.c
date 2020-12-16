// Interface to puzzle data extracted from the puzzlescript file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "puzzleData.h"

extern PuzzleData pd;

#define PUZZLE_MALLOC_INC 10;

int noAction() {
  return pd.noAction;
}

int noRestart() {
  return pd.noAction;
}

int noUndo() {
  return pd.noUndo;
}

int noRepeatAction() {
  return pd.noRepeatAction;
}

int debug() {
  return pd.debug;
}

int verboseLogging() {
  return pd.verboseLogging;
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
      pd.aliasLegend[i].objects = malloc(sizeof(LegendValue) * pd.aliasLegend[i].objectCapacity);
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
      pd.glyphLegend[i].objects = malloc(sizeof(LegendValue) * pd.glyphLegend[i].objectCapacity);
    }
  }
  pd.glyphLegendCount++;
}

void incAliasLegendObject(int legendId) {
  if (pd.aliasLegend[legendId].objectCount + 1 >= pd.aliasLegend[legendId].objectCapacity) {
    pd.aliasLegend[legendId].objectCapacity += PUZZLE_MALLOC_INC;
    pd.aliasLegend[legendId].objects = realloc(pd.aliasLegend[legendId].objects, sizeof(LegendValue) * pd.aliasLegend[legendId].objectCapacity);
  }

  pd.aliasLegend[legendId].objectCount++;
}

void incGlyphLegendObject(int legendId) {
  if (pd.glyphLegend[legendId].objectCount + 1 >= pd.glyphLegend[legendId].objectCapacity) {
    pd.glyphLegend[legendId].objectCapacity += PUZZLE_MALLOC_INC;
    pd.glyphLegend[legendId].objects = realloc(pd.glyphLegend[legendId].objects, sizeof(LegendValue) * pd.glyphLegend[legendId].objectCapacity);
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

void incLayerWidth(int layerId) {
  if (pd.layers[layerId].width + 1 >= pd.layers[layerId].objectCapacity) {
    pd.layers[layerId].objectCapacity += PUZZLE_MALLOC_INC;
    pd.layers[layerId].objectIds = realloc(pd.layers[layerId].objectIds, sizeof(int) * pd.layers[layerId].objectCapacity);
  }
  pd.layers[layerId].width++;
}

void incRule() {
  if (pd.ruleCount + 1 >= pd.ruleCapacity) {
    fprintf(stderr, "RULE REALLOC\n");
    pd.ruleCapacity += PUZZLE_MALLOC_INC;
    pd.rules = realloc(pd.rules, sizeof(Rule) * pd.ruleCapacity);
    for (int ruleId = pd.ruleCount + 1; ruleId < pd.ruleCapacity; ruleId++) {
      pd.rules[ruleId].directionConstraint = NONE;
      pd.rules[ruleId].executionTime = NORMAL;

      pd.rules[ruleId].matchStateCount = 0;
      pd.rules[ruleId].matchStateCapacity = 1;
      pd.rules[ruleId].matchStates = malloc(sizeof(RuleState) * pd.rules[ruleId].matchStateCapacity);

      for (int stateId = 0; stateId < pd.rules[ruleId].matchStateCapacity; stateId++) {
        pd.rules[ruleId].matchStates[stateId].partCount = 0;
        pd.rules[ruleId].matchStates[stateId].partCapacity = 1;
        pd.rules[ruleId].matchStates[stateId].parts = malloc(sizeof(RuleStatePart) * pd.rules[ruleId].matchStates[stateId].partCapacity);
        for (int partId = 0; partId < pd.rules[ruleId].matchStates[stateId].partCapacity; partId++) {
          pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCount = 0;
          pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity = 5;
          pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity = malloc(sizeof(RuleIdentity) * pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity);
          for (int ident = 0; ident < pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity; ident++) {
            pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity[ident].direction = NONE;
            pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity[ident].legendId = -1;
          }
        }
      }

      pd.rules[ruleId].resultStateCount = 0;
      pd.rules[ruleId].resultStateCapacity = 1;
      pd.rules[ruleId].resultStates = malloc(sizeof(RuleState) * pd.rules[ruleId].resultStateCapacity);

      for (int stateId = 0; stateId < pd.rules[ruleId].resultStateCapacity; stateId++) {
        pd.rules[ruleId].resultStates[stateId].partCount = 0;
        pd.rules[ruleId].resultStates[stateId].partCapacity = 100;
        pd.rules[ruleId].resultStates[stateId].parts = malloc(sizeof(RuleStatePart) * pd.rules[ruleId].matchStates[stateId].partCapacity);
        for (int partId = 0; partId < pd.rules[ruleId].resultStates[stateId].partCapacity; partId++) {
          pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCount = 0;
          pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity = 5;
          pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity = malloc(sizeof(RuleIdentity) * pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity);
          for (int ident = 0; ident < pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity; ident++) {
            pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity[ident].direction = NONE;
            pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity[ident].legendId = -1;
          }
        }
      }
    }
  }
  pd.ruleCount++;
}

void incRuleMatchState(int ruleId) {
  if (pd.rules[ruleId].matchStateCount + 1 >= pd.rules[ruleId].matchStateCapacity) {
    pd.rules[ruleId].matchStateCapacity += PUZZLE_MALLOC_INC;
    pd.rules[ruleId].matchStates = realloc(pd.rules[ruleId].matchStates, sizeof(RuleState) * pd.rules[ruleId].matchStateCapacity);
    for (int stateId = pd.rules[ruleId].matchStateCount + 1; stateId < pd.rules[ruleId].matchStateCapacity; stateId++) {
      pd.rules[ruleId].matchStates[stateId].partCount = 0;
      pd.rules[ruleId].matchStates[stateId].partCapacity = 100;
      pd.rules[ruleId].matchStates[stateId].parts = malloc(sizeof(RuleStatePart) * pd.rules[ruleId].matchStates[stateId].partCapacity);
      for (int partId = 0; partId < pd.rules[ruleId].matchStates[stateId].partCapacity; partId++) {
        pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCount = 0;
        pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity = 5;
        pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity = malloc(sizeof(RuleIdentity) * pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity);
        for (int ident = 0; ident < pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity; ident++) {
          pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity[ident].direction = NONE;
          pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity[ident].legendId = -1;
        }
      }
    }
  }
  pd.rules[ruleId].matchStateCount++;
}

void incRuleResultState(int ruleId) {
  if (pd.rules[ruleId].resultStateCount + 1 >= pd.rules[ruleId].resultStateCapacity) {
    pd.rules[ruleId].resultStateCapacity += PUZZLE_MALLOC_INC;
    pd.rules[ruleId].resultStates = realloc(pd.rules[ruleId].resultStates, sizeof(RuleState) * pd.rules[ruleId].resultStateCapacity);

    for (int stateId = pd.rules[ruleId].resultStateCount + 1; stateId < pd.rules[ruleId].resultStateCapacity; stateId++) {
      pd.rules[ruleId].resultStates[stateId].partCount = 0;
      pd.rules[ruleId].resultStates[stateId].partCapacity = 1;
      pd.rules[ruleId].resultStates[stateId].parts = malloc(sizeof(RuleStatePart) * pd.rules[ruleId].resultStates[stateId].partCapacity);
      for (int partId = 0; partId < pd.rules[ruleId].resultStates[stateId].partCapacity; partId++) {
        pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCount = 0;
        pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity = 5;
        pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity = malloc(sizeof(RuleIdentity) * pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity);
        for (int ident = 0; ident < pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity; ident++) {
          pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity[ident].direction = NONE;
          pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity[ident].legendId = -1;
        }
      }
    }
  }
  pd.rules[ruleId].resultStateCount++;
}

void incMatchPart(int ruleId, int stateId) {
  if (pd.rules[ruleId].matchStates[stateId].partCount + 1 >= pd.rules[ruleId].matchStates[stateId].partCapacity) {
    pd.rules[ruleId].matchStates[stateId].partCapacity += PUZZLE_MALLOC_INC;
    pd.rules[ruleId].matchStates[stateId].parts = realloc(pd.rules[ruleId].matchStates[stateId].parts, sizeof(RuleStatePart) * pd.rules[ruleId].matchStates[stateId].partCapacity);
    for (int partId = pd.rules[ruleId].matchStates[stateId].partCount + 1 ; partId < pd.rules[ruleId].matchStates[stateId].partCapacity; partId++) {
      pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCount = 0;
      pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity = 5;
      pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity = malloc(sizeof(RuleIdentity) * pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity);
      for (int ident = 0; ident < pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity; ident++) {
        pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity[ident].direction = NONE;
        pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity[ident].legendId = -1;
      }
    }
  }
  pd.rules[ruleId].matchStates[stateId].partCount++;
}

void incResultPart(int ruleId, int stateId) {
  if (pd.rules[ruleId].resultStates[stateId].partCount + 1 >= pd.rules[ruleId].resultStates[stateId].partCapacity) {
    pd.rules[ruleId].resultStates[stateId].partCapacity += PUZZLE_MALLOC_INC;
    pd.rules[ruleId].resultStates[stateId].parts = realloc(pd.rules[ruleId].resultStates[stateId].parts, sizeof(RuleStatePart) * pd.rules[ruleId].resultStates[stateId].partCapacity);
    for (int partId = pd.rules[ruleId].resultStates[stateId].partCount + 1; partId < pd.rules[ruleId].resultStates[stateId].partCapacity; partId++) {
      pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCount = 0;
      pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity = 5;
      pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity = malloc(sizeof(RuleIdentity) * pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity);
      for (int ident = 0; ident < pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity; ident++) {
        pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity[ident].direction = NONE;
        pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity[ident].legendId = -1;
      }
    }
  }
  pd.rules[ruleId].resultStates[stateId].partCount++;
}

int ruleCommandCount(int ruleId) {
  return pd.rules[ruleId].commandCount;
}

int ruleCommandContains(int ruleId, Command cmd) {
  for (int i = 0; i < pd.rules[ruleId].commandCount; i++) {
    if (pd.rules[ruleId].commands[i] == cmd) {
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
  pd.aliasLegend[pd.aliasLegendCount].key = strdup("...");
  pd.aliasLegend[pd.aliasLegendCount].objectCount = 1;
  pd.aliasLegend[pd.aliasLegendCount].objects[0].id = pd.objectCount;
  pd.objects[pd.objectCount].name = strdup("_Spread_");
  incObject();
  incAliasLegend();

  pd.aliasLegend[pd.aliasLegendCount].key = strdup("_EMPTY_");
  pd.aliasLegend[pd.aliasLegendCount].objectCount = 1;
  pd.aliasLegend[pd.aliasLegendCount].objects[0].id = pd.objectCount;
  pd.objects[pd.objectCount].name = strdup("_Empty_");
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

  pd.objectCount = 0;
  pd.objectCapacity = 100;
  pd.objects = malloc(sizeof(Object) * pd.objectCapacity);
  for (int i = 0; i < pd.objectCapacity; i++) {
    pd.objects[i].colorCount = 0;
    pd.objects[i].height = 0;
    pd.objects[i].width = 0;
  }

  pd.aliasLegendCount = 0;
  pd.aliasLegendCapacity = 100;
  pd.aliasLegend = malloc(sizeof(AliasLegend) * pd.aliasLegendCapacity);
  for (int i = 0; i < pd.aliasLegendCapacity; i++) {
    pd.aliasLegend[i].objectRelation = LEGEND_RELATION_UNKNOWN;
    pd.aliasLegend[i].objectCount = 0;
    pd.aliasLegend[i].objectCapacity = 10;
    pd.aliasLegend[i].objects = malloc(sizeof(LegendValue) * pd.aliasLegend[i].objectCapacity);
  }

  pd.glyphLegendCount = 0;
  pd.glyphLegendCapacity = 100;
  pd.glyphLegend = malloc(sizeof(GlyphLegend) * pd.glyphLegendCapacity);
  for (int i = 0; i < pd.glyphLegendCapacity; i++) {
    pd.glyphLegend[i].objectRelation = LEGEND_RELATION_UNKNOWN;
    pd.glyphLegend[i].objectCount = 0;
    pd.glyphLegend[i].objectCapacity = 10;
    pd.glyphLegend[i].objects = malloc(sizeof(LegendValue) * pd.glyphLegend[i].objectCapacity);
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
    pd.rules[ruleId].commandCount = 0;

    pd.rules[ruleId].directionConstraint = NONE;
    pd.rules[ruleId].executionTime = NORMAL;

    pd.rules[ruleId].matchStateCount = 0;
    pd.rules[ruleId].matchStateCapacity = 1;
    pd.rules[ruleId].matchStates = malloc(sizeof(RuleState) * pd.rules[ruleId].matchStateCapacity);
    for (int stateId = 0; stateId < pd.rules[ruleId].matchStateCapacity; stateId++) {
      pd.rules[ruleId].matchStates[stateId].partCount = 0;
      pd.rules[ruleId].matchStates[stateId].partCapacity = 1;
      pd.rules[ruleId].matchStates[stateId].parts = malloc(sizeof(RuleStatePart) * pd.rules[ruleId].matchStates[stateId].partCapacity);
      for (int partId = 0; partId < pd.rules[ruleId].matchStates[stateId].partCapacity; partId++) {
        pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCount = 0;
        pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity = 5;
        pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity = malloc(sizeof(RuleIdentity) * pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity);
        for (int ident = 0; ident < pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentityCapacity; ident++) {
          pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity[ident].direction = NONE;
          pd.rules[ruleId].matchStates[stateId].parts[partId].ruleIdentity[ident].legendId = 1; // TODO: this is empty, but that isn't clear
        }
      }
    }

    pd.rules[ruleId].resultStateCount = 0;
    pd.rules[ruleId].resultStateCapacity = 1;
    pd.rules[ruleId].resultStates = malloc(sizeof(RuleState) * pd.rules[ruleId].resultStateCapacity);
    for (int stateId = 0; stateId < pd.rules[ruleId].resultStateCapacity; stateId++) {
      pd.rules[ruleId].resultStates[stateId].partCount = 0;
      pd.rules[ruleId].resultStates[stateId].partCapacity = 1;
      pd.rules[ruleId].resultStates[stateId].parts = malloc(sizeof(RuleStatePart) * pd.rules[ruleId].resultStates[stateId].partCapacity);
      for (int partId = 0; partId < pd.rules[ruleId].resultStates[stateId].partCapacity; partId++) {
        pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCount = 0;
        pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity = 5;
        pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity = malloc(sizeof(RuleIdentity) * pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity);
        for (int ident = 0; ident < pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentityCapacity; ident++) {
          pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity[ident].direction = NONE;
          pd.rules[ruleId].resultStates[stateId].parts[partId].ruleIdentity[ident].legendId = -1;
        }
      }
    }
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

PuzzleData * parsePuzzle(FILE * file) {
  initPuzzleData();
  yyin = file;
  yyparse();

  return &pd;
}

void freePuzzle() {

  for (int i = 0; i < pd.objectCount; i++) {
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

  for (int i = 0; i < pd.levelCapacity; i++) {
    free(pd.levels[i].cells);
  }
  free(pd.winConditions);
}

int requirePlayerMovement() {
  return pd.requirePlayerMovement;
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

char objectGlyph(int objId) {
  for (int i = 0; i < pd.glyphLegendCount; i++) {
    if (glyphLegendObjectCount(i) == 1 && glyphLegendContains(i, objId) == 1) {
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

int layerCount() {
  return pd.layerCount;
}

int layerWidth(int layerId) {
  return pd.layers[layerId].width;
}

int layerObjectId(int layerId, int objectIndex) {
  return pd.layers[layerId].objectIds[objectIndex];
}

int aliasLegendObjectCount(int id) {
  return pd.aliasLegend[id].objectCount;
}

int glyphLegendObjectCount(int id) {
  return pd.glyphLegend[id].objectCount;
}

int aliasLegendObjectId(int legendId, int objectIndex) {
  return pd.aliasLegend[legendId].objects[objectIndex].id;
}

int glyphLegendObjectId(int legendId, int objectIndex) {
  return pd.glyphLegend[legendId].objects[objectIndex].id;
}

int aliasLegendContains(int legendId, int objId) {
  int count = aliasLegendObjectCount(legendId);
  for (int i = 0; i < count; i++) {
    if (objId == aliasLegendObjectId(legendId, i)) {
      return 1;
    }
  }
  return 0;
}

int glyphLegendContains(int legendId, int objId) {
  int count = glyphLegendObjectCount(legendId);
  for (int i = 0; i < count; i++) {
    if (objId == glyphLegendObjectId(legendId, i)) {
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
        return pd.glyphLegend[i].objects[0].id;
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

int aliasLegendCount() {
  return pd.aliasLegendCount;
}

int glyphLegendCount() {
  return pd.glyphLegendCount;
}

char * aliasLegendKey(int id) {
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
